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


static zframe_t *
ZoneWorker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (ZONE_SERVER_PONG), sizeof (ZONE_SERVER_PONG));
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
    sessionFrame = ClientSession_getSession (self->sessionServer, clientIdentity);
    session = (ClientSession *) zframe_data (sessionFrame);

    // Build the reply
    zmsg_remove (msg, packet);
    switch (PacketHandler_buildReply (zoneHandlers, sizeof_array(zoneHandlers), session, zframe_data (packet), zframe_size (packet), msg))
    {
        case PACKET_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (zframe_data (packet), zframe_size (packet), NULL);
        break;

        case PACKET_HANDLER_OK:
        break;

        case PACKET_HANDLER_UPDATE_SESSION:
            if (!ClientSession_updateSession (self->sessionServer, clientIdentity, session)) {
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
