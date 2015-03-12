/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ZoneWorker.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "ZoneWorker.h"
#include "ZoneServer/ZoneServer.h"
#include "ZoneServer/ZoneHandler/ZoneHandler.h"
#include "Common/ClientSession/ClientSession.h"
#include "Common/Packet/Packet.h"
#include "Common/Crypto/Crypto.h"


// ------ Structure declaration -------


// ------ Static declaration -------

/**
 * @brief Handle a PING request from any entity.
 * @return a zframe_t containing the PONG. Never returns NULL.
 */
static zframe_t *
ZoneWorker_handlePingPacket (
    void
);


/**
 * @brief Handle a client request.
 *        The first frame contains client entity, the second frame contains packet data.
 * @param self An allocated ZoneWorker structure
 * @param msg The message of the client
 * @return
 */
static void
ZoneWorker_processClientPacket (
    ZoneWorker *self,
    zmsg_t *msg
);


/**
 * @brief Handle a intern packet request.
 * @param self An allocated ZoneWorker structure
 * @param msg The message to process
 * @return true on success, false otherwise
 */
static void
ZoneWorker_processInternPacket (
    ZoneWorker *self,
    zmsg_t *msg
);



// ------ Extern function implementation -------

ZoneWorker *
ZoneWorker_new (
    int workerId,
    int zoneServerId,
    int frontendPort,
    int sessionServerFrontendPort
) {
    ZoneWorker *self;

    if ((self = calloc (1, sizeof (ZoneWorker))) == NULL) {
        return NULL;
    }

    if (!ZoneWorker_init (self, workerId, zoneServerId, frontendPort, sessionServerFrontendPort)) {
        ZoneWorker_destroy (&self);
        error ("ZoneWorker failed to initialize.");
        return NULL;
    }

    return self;
}


bool
ZoneWorker_init (
    ZoneWorker *self,
    int workerId,
    int zoneServerId,
    int frontendPort,
    int sessionServerFrontendPort
) {
    self->workerId = workerId;
    self->zoneServerId = zoneServerId;
    self->frontendPort = frontendPort;
    self->sessionServerFrontendPort = sessionServerFrontendPort;

    return true;
}


static ZoneHandlerState
ZoneWorker_buildReply (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    ZoneHandlerFunction handler;

    // Preconditions
    if (packetSize < sizeof (CryptPacketHeader)) {
        error ("The packet received is too small to be read. Ignore request.");
        return ZONE_HANDLER_ERROR;
    }

    // Unwrap the crypt packet header
    CryptPacketHeader cryptHeader;
    CryptPacket_unwrapHeader (&packet, &cryptHeader);
    if (packetSize - sizeof (cryptHeader) != cryptHeader.size) {
        error ("The real packet size (0x%x) doesn't match with the packet size in the header (0x%x). Ignore request.",
            packetSize, cryptHeader.size);
        return ZONE_HANDLER_ERROR;
    }

    // Uncrypt the packet
    if (!Crypto_uncryptPacket (&cryptHeader, &packet)) {
        error ("Cannot uncrypt the client packet. Ignore request.");
        return ZONE_HANDLER_ERROR;
    }

    // Read the packet
    ClientPacketHeader header;
    ClientPacket_unwrapHeader (&packet, &header);
    size_t dataSize = packetSize - sizeof (CryptPacketHeader) - sizeof (ClientPacketHeader);

    // Get the corresponding packet handler
    if (header.type > sizeof_array (zoneHandlers)) {
        error ("Invalid packet type. Ignore request.");
        return ZONE_HANDLER_ERROR;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = zoneHandlers [header.type].handler)) {
        error ("Cannot find handler for the requested packet type : %s",
            (header.type <PACKET_TYPES_MAX_INDEX) ?
               packetTypeInfo.packets[header.type].string : "UNKNOWN"
        );
        return ZONE_HANDLER_ERROR;
    }

    // Call the handler
    dbg ("Calling [%s] handler", packetTypeInfo.packets[header.type].string);
    return handler (session, packet, dataSize, reply);
}

static zframe_t *
ZoneWorker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (ZONE_SERVER_PONG), sizeof (ZONE_SERVER_PONG));
}

static zframe_t *
ZoneWorker_requestSession (
    ZoneWorker *self,
    zframe_t *clientIdentity
) {
    ClientSession *session;
    zframe_t *sessionFrame;
    zmsg_t *msg;

    // Build a session request message
    if (!(msg = zmsg_new ())
    ||  zmsg_addmem (msg, PACKET_HEADER (SESSION_SERVER_REQUEST_SESSION), sizeof (SESSION_SERVER_REQUEST_SESSION)) != 0
    ||  zmsg_addmem (msg, zframe_data (clientIdentity), zframe_size (clientIdentity)) != 0
    ||  zmsg_send (&msg, self->sessionServer) != 0
    ) {
        error ("Cannot build and send a session message for the session server");
        return NULL;
    }

    // Wait for the session server answer
    if (!(msg = zmsg_recv (self->sessionServer))) {
        error ("Cannot receive a session from the session server");
        return NULL;
    }

    // Extract the session from the answer
    if (!(sessionFrame = zmsg_pop (msg))
    ||  !(session = (ClientSession *) zframe_data (sessionFrame))
    ||  !(sizeof (ClientSession) == zframe_size (sessionFrame))
    ) {
        error ("Cannot extract correctly the session from the session server");
        return NULL;
    }

    // Cleanup
    zmsg_destroy (&msg);

    return sessionFrame;
}

static bool
ZoneWorker_updateSession (
    ZoneWorker *self,
    zframe_t *clientIdentity,
    ClientSession *session
) {
    zframe_t *answerFrame;
    SessionServerSendHeader answer;
    zmsg_t *msg;

    // Build the update sesion packet, and send it to the server
    if (!(msg = zmsg_new ())
    ||  zmsg_addmem (msg, PACKET_HEADER (SESSION_SERVER_UPDATE_SESSION), sizeof (SESSION_SERVER_UPDATE_SESSION)) != 0
    ||  zmsg_addmem (msg, zframe_data (clientIdentity), zframe_size (clientIdentity)) != 0
    ||  zmsg_addmem (msg, session, sizeof (ClientSession)) != 0
    ||  zmsg_send (&msg, self->sessionServer) != 0
    ) {
        error ("Cannot build and send a session message for the session server");
        return false;
    }

    // Wait for the answer of the session server
    if (!(msg = zmsg_recv (self->sessionServer))) {
        error ("Cannot receive a session from the session server");
        return false;
    }

    // Extract the answer of the session server
    if (!(answerFrame = zmsg_pop (msg))
    ||  !(answer = *((SessionServerSendHeader *) zframe_data (answerFrame)))
    ||  !(sizeof (answer) == zframe_size (answerFrame))
    ) {
        dbg ("answer = %d", answer);
        dbg ("sizeof answer = %d", sizeof (answer));
        if (answerFrame) {
            zframe_print (answerFrame, "answerFrame = ");
            zframe_destroy (&answerFrame);
        }
        zmsg_print (msg);
        error ("Cannot extract correctly the answer from the session server");
        return false;
    }

    // Verify the status code
    if (answer != SESSION_SERVER_UPDATE_SESSION_OK) {
        error ("The session server failed to update the session");
        return false;
    }

    // Cleanup
    zframe_destroy (&answerFrame);
    zmsg_destroy (&msg);

    return true;
}

static void
ZoneWorker_processClientPacket (
    ZoneWorker *self,
    zmsg_t *msg
) {
    ClientSession *session;
    zframe_t *sessionFrame;

    // Read the message
    zframe_t *clientIdentity = zmsg_first (msg);
    zframe_t *packet = zmsg_next (msg);

    // Request a session
    sessionFrame = ZoneWorker_requestSession (self, clientIdentity);
    session = (ClientSession *) zframe_data (sessionFrame);

    // Build the reply
    zmsg_remove (msg, packet);
    switch (ZoneWorker_buildReply (session, zframe_data (packet), zframe_size (packet), msg))
    {
        case ZONE_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (zframe_data (packet), zframe_size (packet), NULL);
        break;

        case ZONE_HANDLER_OK:
        break;

        case ZONE_HANDLER_UPDATE_SESSION:
            if (!ZoneWorker_updateSession (self, clientIdentity, session)) {
                error ("Cannot update the following session");
                ClientSession_print (session);
            }
        break;
    }
}

static void
ZoneWorker_processInternPacket (
    ZoneWorker *self,
    zmsg_t *msg
) {

}

void
ZoneWorker_handleRequest (
    ZoneWorker *self,
    zmsg_t *msg
) {
    // Extract the request
    zframe_t *identityFrame = zmsg_pop (msg); (void) identityFrame;
    zframe_t *emptyFrame = zmsg_pop (msg); (void) emptyFrame;
    zframe_t *headerFrame = zmsg_pop (msg);
    zframe_t *requestAnswer = NULL;

    ZoneServerRecvHeader header = *((ZoneServerRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case ZONE_SERVER_PING:
            requestAnswer = ZoneWorker_handlePingPacket ();
        break;

        default:
            error ("Packet type %d not handled.", header);
        break;
    }

    // Rebuild the answer message
    zmsg_add (msg, identityFrame);
    zmsg_add (msg, emptyFrame);
    zmsg_add (msg, requestAnswer);

    // Clean up
    zframe_destroy (&headerFrame);
}


void *
ZoneWorker_worker (
    void *arg
) {
    zframe_t *readyFrame;
    zmsg_t *msg;

    ZoneWorker *self = (ZoneWorker *) arg;

    // Create and connect a socket to the backend
    if (!(self->worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (self->worker, ZONE_SERVER_BACKEND_ENDPOINT, self->frontendPort) == -1
    ) {
        error ("[%d] Zone worker ID = %d cannot connect to the backend socket.", self->zoneServerId, self->workerId);
        return NULL;
    }

    // Create and connect a socket to the session server frontend
    if (!(self->sessionServer = zsock_new (ZMQ_REQ))
    ||  zsock_connect (self->sessionServer, SESSION_SERVER_FRONTEND_ENDPOINT, self->sessionServerFrontendPort) == -1
    ) {
        error ("Barrack worker ID = %d cannot connect to the Session Server.", self->workerId);
        return NULL;
    }

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (PACKET_HEADER (ZONE_SERVER_WORKER_READY), sizeof (ZONE_SERVER_WORKER_READY)))
    ||  zframe_send (&readyFrame, self->worker, 0) == -1
    ) {
        error ("[%d] Zone worker ID = %d cannot send a correct ZONE_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("[%d] Zone worker ID %d is running and waiting for messages.", self->zoneServerId, self->workerId);

    while (true) {

        // Process messages as they arrive
        if (!(msg = zmsg_recv (self->worker))) {
            dbg ("[%d] Zone worker ID = %d stops working.", self->zoneServerId, self->workerId);
            break; // Interrupted
        }

        // No message should be with less than 3 frames
        if (zmsg_size (msg) < 2) {
            error ("[%d] Zone worker ID = %d received a malformed message.", self->zoneServerId, self->workerId);
            zmsg_destroy (&msg);
            continue;
        }

        // Only ToS clients send messages with 2 frames
        if (zmsg_size (msg) == 2) {
            // The first frame is the client identity
            // The second frame is the data of the packet
            ZoneWorker_processClientPacket (self, msg);
        }

        else {
            // This is a message from intern entities of the network
            //! Nothing too much important should be here, as the clients
            //! could craft a fake packet with 3 frames
            // TODO : Check the identity of the sender (it shouldn't be a client)
            ZoneWorker_processInternPacket (self, msg);
        }


        // Reply back to the sender
        if (zmsg_send (&msg, self->worker) != 0) {
            warning ("[%d] Zone worker ID = %d failed to send a message to the backend.", self->zoneServerId, self->workerId);
        }
    }

    // Cleanup
    zsock_destroy (&self->worker);

    dbg ("[%d] Zone worker ID = %d exits.", self->zoneServerId, self->workerId);
    return NULL;
}

void
ZoneWorker_destroy (
    ZoneWorker **_self
) {
    ZoneWorker *self = *_self;

    free (self);
    *_self = NULL;
}
