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


// ------ Structure declaration -------


// ------ Static declaration -------
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


// ------ Extern function implementation ------

BarrackWorker *
BarrackWorker_new (
    int workerId
) {
    BarrackWorker *self;

    if ((self = calloc (1, sizeof (BarrackWorker))) == NULL) {
        return NULL;
    }

    if (!BarrackWorker_init (self, workerId)) {
        BarrackWorker_destroy (&self);
        error ("BarrackWorker failed to initialize.");
        return NULL;
    }

    return self;
}

bool
BarrackWorker_init (
    BarrackWorker *self,
    int workerId
) {
    // Initialize seed
    self->seed = R1EMU_seed_random (workerId);
    self->workerId = workerId;

    return true;
}

static void
BarrackWorker_handlePingPacket (
    zframe_t *headerFrame
) {
    zframe_reset (headerFrame, PACKET_HEADER (BARRACK_SERVER_PONG), sizeof (BARRACK_SERVER_PONG));
}

static void
BarrackWorker_processClientPacket (
    BarrackWorker *self,
    zmsg_t *msg
) {
    ClientSession *session;
    zframe_t *sessionFrame;

    // Read the message
    zframe_t *clientIdentity = zmsg_first (msg);
    zframe_t *packet = zmsg_next (msg);

    // Request a session
    sessionFrame = ClientSession_getSession (self->sessionServer, clientIdentity);
    session = (ClientSession *) zframe_data (sessionFrame);

    // Build the reply
    // We don't need the client packet in the reply
    zmsg_remove (msg, packet);
    // Consider the message as a "normal" message by default
    zframe_t *headerAnswer = zframe_new (PACKET_HEADER (BARRACK_SERVER_WORKER_NORMAL), sizeof (BARRACK_SERVER_WORKER_NORMAL));
    zmsg_push (msg, headerAnswer);

    switch (PacketHandler_buildReply (barrackHandlers, sizeof_array (barrackHandlers), session, zframe_data (packet), zframe_size (packet), msg, self))
    {
        case PACKET_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (zframe_data (packet), zframe_size (packet), NULL);
            zframe_reset (headerAnswer, PACKET_HEADER (ZONE_SERVER_WORKER_ERROR), sizeof (ZONE_SERVER_WORKER_ERROR));
        break;

        case PACKET_HANDLER_OK:
        break;

        case PACKET_HANDLER_UPDATE_SESSION:
            if (!ClientSession_updateSession (self->sessionServer, clientIdentity, session)) {
                error ("Cannot update the following session");
                ClientSession_print (session);
            }
        break;

        case PACKET_HANDLER_DELETE_SESSION:
            if (!ClientSession_deleteSession (self->sessionServer, clientIdentity)) {
                error ("Cannot delete the following session");
                ClientSession_print (session);
            }
        break;
    }

    // Cleanup
    zframe_destroy (&sessionFrame);
    zframe_destroy (&packet);
}


static void
BarrackWorker_processInternPacket (
    BarrackWorker *self,
    zmsg_t *msg
) {
    // Read the message
    zframe_t *clientIdentityFrame = zmsg_first (msg); (void) clientIdentityFrame;
    zframe_t *headerFrame = zmsg_next (msg);

    // Handle the request
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

    // Cleanup
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
    ||  zsock_connect (self->sessionServer, SESSION_SERVER_FRONTEND_ENDPOINT, 0) == -1
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
            error ("Barrack worker ID = %d received a malformed message.", self->workerId);
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
            // TODO : Check the identity of the sender (it shouldn't be a client)
            BarrackWorker_processInternPacket (self, msg);
        }

        // Reply back to the sender
        if (zmsg_send (&msg, self->worker) != 0) {
            warning ("Barrack worker ID %d failed to send a message.", self->workerId);
        }
    }

    // Cleanup
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
