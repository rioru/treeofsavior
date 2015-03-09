/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "BarrackWorker.h"
#include "BarrackServer/BarrackServer.h"
#include "BarrackServer/BarrackHandler/BarrackHandler.h"
#include "Common/Packet/Packet.h"
#include "Common/Crypto/Crypto.h"


// ------ Structure declaration -------


// ------ Static declaration -------
/**
 * @brief
 * @param[in] session The session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @return BarrackHandlerState
 */
static BarrackHandlerState
BarrackWorker_buildReply (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
);

// ------ Extern declaration -------

BarrackWorker *
BarrackWorker_new (
    int workerId,
    int sessionServerFrontendPort
) {
    BarrackWorker *self;

    if ((self = calloc (1, sizeof (BarrackWorker))) == NULL) {
        return NULL;
    }

    if (!BarrackWorker_init (self, workerId, sessionServerFrontendPort)) {
        BarrackWorker_destroy (&self);
        error ("BarrackWorker failed to initialize.");
        return NULL;
    }

    return self;
}


bool
BarrackWorker_init (
    BarrackWorker *self,
    int workerId,
    int sessionServerFrontendPort
) {
    self->workerId = workerId;
    self->sessionServerFrontendPort = sessionServerFrontendPort;

    return true;
}


static BarrackHandlerState
BarrackWorker_buildReply (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    char *rawPacket = packet;
    BarrackHandlerFunction handler;

    // Preconditions
    if (packetSize < sizeof (CryptPacketHeader)) {
        error ("The packet received is too small to be read. Ignore request.");
        return BARRACK_HANDLER_ERROR;
    }

    // Unwrap the crypt packet header
    CryptPacketHeader cryptHeader;
    CryptPacket_unwrapHeader (&packet, &cryptHeader);
    if (packetSize - sizeof (cryptHeader) != cryptHeader.size) {
        error ("The real packet size (%d) doesn't match with the packet size in the header (%d). Ignore request.",
            packetSize, cryptHeader.size);
        buffer_print (rawPacket, packetSize, NULL);
        return BARRACK_HANDLER_ERROR;
    }

    // Uncrypt the packet
    if (!Crypto_uncryptPacket (&cryptHeader, &packet)) {
        error ("Cannot uncrypt the client packet. Ignore request.");
        buffer_print (rawPacket, packetSize, NULL);
        return BARRACK_HANDLER_ERROR;
    }

    // Read the packet
    ClientPacketHeader header;
    ClientPacket_unwrapHeader (&packet, &header);
    size_t dataSize = packetSize - sizeof (CryptPacketHeader) - sizeof (ClientPacketHeader);

    // Get the corresponding packet handler
    if (header.type > sizeof_array (barrackHandlers)) {
        error ("Invalid packet type. Ignore request.");
        buffer_print (rawPacket, packetSize, NULL);
        return BARRACK_HANDLER_ERROR;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = barrackHandlers [header.type].handler)) {
        error ("Cannot find handler for the requested packet type : %s",
            (header.type <PACKET_TYPES_MAX_INDEX) ?
               packetTypeInfo.packets[header.type].string : "UNKNOWN"
        );
        buffer_print (rawPacket, packetSize, NULL);
        return BARRACK_HANDLER_ERROR;
    }

    // Call the handler
    dbg ("Calling [%s] handler", packetTypeInfo.packets[header.type].string);
    return handler (session, packet, dataSize, reply);
}


void *
BarrackWorker_worker (
    void *arg
) {
    zsock_t *worker, *sessionServerFrontend;
    zmsg_t *packetMsg;
    zframe_t *readyFrame;
    zframe_t *requestSessionFromId;
    zframe_t *sessionFrame;
    zmsg_t *sessionMsg;
    ClientSession sessionRollback;

    BarrackWorker * self = (BarrackWorker *) arg;

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, BARRACK_SERVER_BACKEND_ENDPOINT) == -1
    ) {
        error ("Barrack worker ID = %d cannot connect to the backend socket.", self->workerId);
        return NULL;
    }

    // Create and connect a socket to the session server frontend
    if (!(sessionServerFrontend = zsock_new (ZMQ_REQ))
    ||  zsock_connect (sessionServerFrontend, SESSION_SERVER_FRONTEND_ENDPOINT, self->sessionServerFrontendPort) == -1
    ) {
        error ("Barrack worker ID = %d cannot connect to the Session Server.", self->workerId);
        return NULL;
    }

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (BARRACK_SERVER_WORKER_READY, 1))
    ||  zframe_send (&readyFrame, worker, 0) == -1
    ) {
        error ("Barrack worker ID = %d cannot send a correct BARRACK_SERVER_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("Barrack worker ID %d is running and waiting for messages.", self->workerId);

    while (true) {

        // Process messages as they arrive
        if (!(packetMsg = zmsg_recv (worker))) {
            dbg ("Barrack worker ID %d stops working.", self->workerId);
            break; // Interrupted
        }

        // Retrieve data from the message
        zframe_t *clientIdentity = zmsg_first (packetMsg);
        zframe_t *packet = zmsg_last (packetMsg);

        /// Handle PING-PONG messages
        if (memcmp (zframe_data (packet), BARRACK_SERVER_PING, sizeof (BARRACK_SERVER_PING)) == 0) {
            zframe_reset (packet, BARRACK_SERVER_PONG, sizeof (BARRACK_SERVER_PONG));

            // Send the reply back to the backend
            if (zmsg_send (&packetMsg, worker) != 0) {
                warning ("Barrack worker ID %d failed to send a message to the backend.", self->workerId);
            }

            continue;
        }

        /// Handle requests for the session from the session server
        if (!(requestSessionFromId = zframe_dup (clientIdentity))) {
            error ("Failed to duplicate the client identity. Request dropped.");
            continue;
        }
        if (zframe_send (&requestSessionFromId, sessionServerFrontend, 0) != 0) {
            error ("Cannot send the request to the session server. Request dropped.");
            continue;
        }

        // Receive the session data
        if (!(sessionMsg = zmsg_recv (sessionServerFrontend))) {
            error ("Session server interrupted the request. Request dropped.");
            continue;
        }

        if (!(sessionFrame = zmsg_pop (sessionMsg))) {
            error ("Cannot get the session data from the session msg. Request dropped.");
            zmsg_destroy (&sessionMsg);
            continue;
        }
        zmsg_destroy (&sessionMsg);


        /// Build the reply packet
        ClientSession *session = (ClientSession *) zframe_data (sessionFrame);
        memcpy (&sessionRollback, session, sizeof (ClientSession));

        // Remove the client packet from the reply message, and add our frames to it
        zmsg_remove (packetMsg, packet);
        switch (BarrackWorker_buildReply (session, zframe_data (packet), zframe_size (packet), packetMsg)) {
            case BARRACK_HANDLER_ERROR:
                // Don't process the packet : rollback the changes
                memcpy (session, &sessionRollback, sizeof (ClientSession));
            break;

            case BARRACK_HANDLER_OK:
            // Nothing much to do, everything is OK
            break;

            case BARRACK_HANDLER_UPDATE_SESSION:
            // The session needs to be updated to the database
            break;

            default:
                warning ("The barrack handler returned an unknown state");
            break;
        }

        // We don't need the session anymore
        zframe_destroy (&sessionFrame);

        // The client hasn't been allowed to receive a proper answer
        if (zmsg_size (packetMsg) == 1) { // 1 = only the identity frame, no data frame
            // We send to the client an invalid packet because the barrack backend is waiting for an answer
            zframe_reset (packet, BARRACK_SERVER_INVALID_PACKET, sizeof (BARRACK_SERVER_INVALID_PACKET));
        }

        if (zmsg_send (&packetMsg, worker) != 0) {
            warning ("Barrack worker ID %d failed to send a message to the backend.", self->workerId);
        }
    }

    zsock_destroy (&worker);
    zsock_destroy (&sessionServerFrontend);

    dbg ("Barrack worker ID %d exits.", self->workerId);
    return NULL;
}


void
BarrackWorker_destroy (
    BarrackWorker **_self
) {
    BarrackWorker *self = *_self;

    free (self);
    *_self = NULL;
}
