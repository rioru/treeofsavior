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
#include "Common/ClientSession/ClientSession.h"
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

/**
 * @brief Handle a PING request from any entity.
 *        This function only replace the "PING" signal to a "PONG" one in the message.
 * @param headerFrame The header frame containing the "PING" request
 */
static void
BarrackWorker_handlePingPacket (
    zframe_t *headerFrame
);


/**
 * @brief Handle a client request.
 *        The first frame contains client entity, the second frame contains packet data.
 * @param self An allocated BarrackWorker structure
 * @param msg The message of the client
 * @return
 */
static void
BarrackWorker_processClientPacket (
    BarrackWorker *self,
    zmsg_t *msg
);


/**
 * @brief Handle a intern packet request.
 * @param self An allocated BarrackWorker structure
 * @param msg The message to process
 * @return true on success, false otherwise
 */
static void
BarrackWorker_processInternPacket (
    BarrackWorker *self,
    zmsg_t *msg
);


/**
 * @brief Request a session from the session server
 * @param self An allocated BarrackWorker structure
 * @param clientIdentity A frame containing the identity of the client
 * @return a zframe_t containing a ClientSession on success, NULL otherwise
 */
static zframe_t *
BarrackWorker_requestSession (
    BarrackWorker *self,
    zframe_t *clientIdentity
);


/**
 * @brief Update a session for the session server
 * @param self An allocated BarrackWorker structure
 * @param clientIdentity A frame containing the identity of the client
 * @param session An allocated session to update
 * @return true on success, false otherwise
 */
static bool
BarrackWorker_updateSession (
    BarrackWorker *self,
    zframe_t *clientIdentity,
    ClientSession *session
);

// ------ Extern function implementation ------

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
        return BARRACK_HANDLER_ERROR;
    }

    // Uncrypt the packet
    if (!Crypto_uncryptPacket (&cryptHeader, &packet)) {
        error ("Cannot uncrypt the client packet. Ignore request.");
        return BARRACK_HANDLER_ERROR;
    }

    // Read the packet
    ClientPacketHeader header;
    ClientPacket_unwrapHeader (&packet, &header);
    size_t dataSize = packetSize - sizeof (CryptPacketHeader) - sizeof (ClientPacketHeader);

    // Get the corresponding packet handler
    if (header.type > sizeof_array (barrackHandlers)) {
        error ("Invalid packet type. Ignore request.");
        return BARRACK_HANDLER_ERROR;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = barrackHandlers [header.type].handler)) {
        error ("Cannot find handler for the requested packet type : %s",
            (header.type <PACKET_TYPES_MAX_INDEX) ?
               packetTypeInfo.packets[header.type].string : "UNKNOWN"
        );
        return BARRACK_HANDLER_ERROR;
    }

    // Call the handler
    dbg ("Calling [%s] handler", packetTypeInfo.packets[header.type].string);
    return handler (session, packet, dataSize, reply);
}

static void
BarrackWorker_handlePingPacket (
    zframe_t *headerFrame
) {
    zframe_reset (headerFrame, PACKET_HEADER (BARRACK_SERVER_PONG), sizeof (BARRACK_SERVER_PONG));
}

static zframe_t *
BarrackWorker_requestSession (
    BarrackWorker *self,
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

    zmsg_destroy (&msg);
    return sessionFrame;
}

static bool
BarrackWorker_updateSession (
    BarrackWorker *self,
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
BarrackWorker_processClientPacket (
    BarrackWorker *self,
    zmsg_t *msg
) {
    ClientSession *session;
    zframe_t *sessionFrame;

    zframe_t *clientIdentity = zmsg_first (msg);
    zframe_t *packet = zmsg_next (msg);

    sessionFrame = BarrackWorker_requestSession (self, clientIdentity);
    session = (ClientSession *) zframe_data (sessionFrame);

    zmsg_remove (msg, packet);

    switch (BarrackWorker_buildReply (session, zframe_data (packet), zframe_size (packet), msg))
    {
        case BARRACK_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (zframe_data (packet), zframe_size (packet), NULL);
        break;

        case BARRACK_HANDLER_OK:
        break;

        case BARRACK_HANDLER_UPDATE_SESSION:
            if (!BarrackWorker_updateSession (self, clientIdentity, session)) {
                error ("Cannot update the following session");
                ClientSession_print (session);
            }
        break;
    }

    zframe_destroy (&packet);
}


static void
BarrackWorker_processInternPacket (
    BarrackWorker *self,
    zmsg_t *msg
) {
    zframe_t *clientIdentityFrame = zmsg_first (msg); (void) clientIdentityFrame;
    zframe_t *headerFrame = zmsg_next (msg);
    BarrackServerRecvHeader header = *((BarrackServerRecvHeader *) zframe_data (headerFrame));

    switch (header)
    {
        case BARRACK_SERVER_PING:
            BarrackWorker_handlePingPacket (headerFrame);
        break;

        default:
            error ("Packet type %d not handled.", header);
        break;
    }
}


void *
BarrackWorker_worker (
    void *arg
) {
    zmsg_t *msg;
    zframe_t *readyFrame;

    BarrackWorker * self = (BarrackWorker *) arg;

    // Create and connect a socket to the backend
    if (!(self->worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (self->worker, BARRACK_SERVER_BACKEND_ENDPOINT) == -1
    ) {
        error ("Barrack worker ID = %d cannot connect to the backend socket.", self->workerId);
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
    if (!(readyFrame = zframe_new (PACKET_HEADER (BARRACK_SERVER_WORKER_READY), sizeof (BARRACK_SERVER_WORKER_READY)))
    ||  zframe_send (&readyFrame, self->worker, 0) == -1
    ) {
        error ("Barrack worker ID = %d cannot send a correct BARRACK_SERVER_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("Barrack worker ID %d is running and waiting for messages.", self->workerId);

    while (true)
    {
        // Process messages as they arrive
        if (!(msg = zmsg_recv (self->worker))) {
            dbg ("Barrack worker ID %d stops working.", self->workerId);
            break; // Interrupted
        }

        // No message should be with less than 2 frames
        if (zmsg_size (msg) < 2) {
            error ("A malformed message has been received.");
            zmsg_destroy (&msg);
            continue;
        }

        // Only ToS clients send messages with 2 frames
        if (zmsg_size (msg) == 2) {
            // The first frame is the client identity
            // The second frame is the data of the packet
            BarrackWorker_processClientPacket (self, msg);
        }

        else {
            // This is a message from intern entities of the network
            //! Nothing too much important should be here, as the clients
            //! could craft a fake packet with 3 frames
            BarrackWorker_processInternPacket (self, msg);
        }

        // Reply back to the sender
        if (zmsg_send (&msg, self->worker) != 0) {
            warning ("Barrack worker ID %d failed to send a message.", self->workerId);
        }
    }

    zsock_destroy (&self->worker);
    zsock_destroy (&self->sessionServer);

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
