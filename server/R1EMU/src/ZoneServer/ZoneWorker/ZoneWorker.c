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
#include "Common/Redis/Fields/RedisSocketSession.h"
#include "Common/Redis/Fields/RedisGameSession.h"


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
 * @return true on success, false otherwise
 */
static bool
ZoneWorker_processClientPacket (
    ZoneWorker *self,
    zmsg_t *msg
);


/**
 * @brief Handle a request from the public ports
 * @param self An allocated ZoneWorker structure
 * @param worker The socket listening on the public port
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
ZoneWorker_handlePublicRequest (
    ZoneWorker *self,
    zsock_t *worker
);

/**
 * @brief Handle a request from the private ports (coming from global server)
 * @param self An allocated ZoneWorker structure
 * @param worker The socket listening on the public port
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
ZoneWorker_handlePrivateRequest (
    ZoneWorker *self,
    zsock_t *global
);

/**
 * @brief Process a message from the the global server
 * @param self An allocated ZoneWorker structure
 * @param msg The message coming from the global server
*/
static void
ZoneWorker_processGlobalPacket (
    ZoneWorker *self,
    zmsg_t *msg
);



// ------ Extern function implementation -------

ZoneWorker *
ZoneWorker_new (
    int workerId,
    int zoneServerId,
    int frontendPort,
    int globalPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
) {
    ZoneWorker *self;

    if ((self = calloc (1, sizeof (ZoneWorker))) == NULL) {
        return NULL;
    }

    if (!ZoneWorker_init (self, workerId, zoneServerId, frontendPort, globalPort, sqlInfo, redisInfo)) {
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
    int serverId,
    int frontendPort,
    int globalPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
) {
    self->workerId = workerId;
    self->serverId = serverId;
    self->frontendPort = frontendPort;
    self->globalPort = globalPort;

    // ===================================
    //          Initialize MySQL
    // ===================================
    if (!(self->sqlConn = MySQL_new ())) {
        error ("Cannot initialize a new MySQL connection.");
        return false;
    }
    MySQL_connect (self->sqlConn, sqlInfo);

    // ===================================
    //     Initialize Redis connection
    // ===================================
    if (!(self->redis = Redis_new ())) {
        error ("Cannot initialize a new Redis connection.");
        return false;
    }
    Redis_connect (self->redis, redisInfo);

    return true;
}

zlist_t *
ZoneWorker_getClientsWithinDistance (
    ZoneWorker *self,
    Session *session,
    float x, float y, float z,
    float range
) {
    return Redis_getClientsWithinDistance (self->redis, session->socket.zoneId, session->socket.mapId, x, y, z, range);
}

static zframe_t *
ZoneWorker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (ZONE_SERVER_PONG), sizeof (ZONE_SERVER_PONG));
}

static bool
ZoneWorker_processClientPacket (
    ZoneWorker *self,
    zmsg_t *msg
) {
    Session session = {{0}};

    // Read the message
    zframe_t *socketIdFrame = zmsg_first (msg);
    zframe_t *packet = zmsg_next (msg);

    // Request a Game Session
    if (!(Redis_requestSession (self->redis, self->serverId, zframe_data (socketIdFrame), &session))) {
        error ("Cannot retrieve a Game Session.");
        return false;
    }

    // Build the reply
    // We don't need the client packet in the reply
    zmsg_remove (msg, packet);

    // Consider the message as a "normal" message by default
    zframe_t *headerAnswer = zframe_new (PACKET_HEADER (ZONE_SERVER_WORKER_NORMAL), sizeof (ZONE_SERVER_WORKER_NORMAL));
    zmsg_push (msg, headerAnswer);

    switch (PacketHandler_buildReply (zoneHandlers, sizeof_array (zoneHandlers), &session, zframe_data (packet), zframe_size (packet), msg, self))
    {
        case PACKET_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (zframe_data (packet), zframe_size (packet), NULL);
            zframe_reset (headerAnswer, PACKET_HEADER (ZONE_SERVER_WORKER_ERROR), sizeof (ZONE_SERVER_WORKER_ERROR));
        break;

        case PACKET_HANDLER_OK:
        break;

        case PACKET_HANDLER_UPDATE_SESSION:
            if (!Redis_updateSocketSession (self->redis, &session.socket)) {
                error ("Cannot update the socket session.");
                return false;
            }
            if (!Redis_updateGameSession (self->redis, &session.socket, &session.game)) {
                error ("Cannot update the game session");
                return false;
            }
        break;

        case PACKET_HANDLER_DELETE_SESSION:
            // TODO :
            /*
            if (!Redis_deleteGameSession (self->sessionServer, &gameSession)) {
                error ("Cannot delete the following session");
                GameSession_print (&gameSession);
            }
            */
        break;
    }

    // Cleanup
    zframe_destroy (&packet);

    return true;
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
    zsock_t *worker, *global;
    zpoller_t *poller;
    bool isRunning = true;

    ZoneWorker *self = (ZoneWorker *) arg;

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, ZONE_SERVER_BACKEND_ENDPOINT, self->serverId) == -1
    ) {
        error ("[%d] Zone worker ID = %d cannot connect to the backend socket.", self->serverId, self->workerId);
        return NULL;
    }
    dbg ("Session worker ID %d connected to %s", self->workerId, zsys_sprintf (ZONE_SERVER_BACKEND_ENDPOINT, self->serverId));

    // Create and listen to a socket with the global server
    if (!(global = zsock_new (ZMQ_REQ))
    ||  zsock_connect (global, ZONE_SERVER_GLOBAL_ENDPOINT, self->globalPort) == -1
    ) {
        error ("[%d] Zone worker ID = %d cannot bind to the port %d.", self->serverId, self->globalPort);
        return NULL;
    }
    info ("[%d] Zone worker ID = %d connected to the private global port %d.", self->serverId, self->workerId, self->globalPort);

    // Create and connect a socket to the session server frontend
    if (!(self->sessionServer = zsock_new (ZMQ_REQ))
    ||  zsock_connect (self->sessionServer, SESSION_SERVER_FRONTEND_ENDPOINT, self->serverId) == -1
    ) {
        error ("Barrack worker ID = %d cannot connect to the Session Server.", self->workerId);
        return NULL;
    }

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (PACKET_HEADER (ZONE_SERVER_WORKER_READY), sizeof (ZONE_SERVER_WORKER_READY)))
    ||  zframe_send (&readyFrame, worker, 0) == -1
    ) {
        error ("[%d] Zone worker ID = %d cannot send a correct ZONE_WORKER_READY state.", self->workerId);
        return NULL;
    }

    // Define a poller with the global and the worker socket
    if (!(poller = zpoller_new (global, worker, NULL))) {
        error ("[%d] Zone worker ID = %d cannot create a poller.", self->serverId, self->workerId);
        return NULL;
    }

    dbg ("[%d] Zone worker ID %d is running and waiting for messages.", self->serverId, self->workerId);

    while (isRunning) {

        zsock_t *actor = zpoller_wait (poller, -1);
        typedef int (*ZoneWorkerRequestHandler) (ZoneWorker *self, zsock_t *actor);
        ZoneWorkerRequestHandler handler;

        // Get the correct handler based on the actor
        if (actor == worker) {
            handler = ZoneWorker_handlePublicRequest;
        } else if (actor == global) {
            handler = ZoneWorker_handlePrivateRequest;
        }
        else {
            warning ("[%d] Unknown actor talked to the ZoneWorker ID = %d. Maybe SIGINT signal?", self->serverId, self->workerId);
            break;
        }

        switch (handler (self, actor)) {
            case -1: // ERROR
                error ("[%d] Zone worker ID %d encountered an error when handling a request.", self->serverId, self->workerId);
            case -2: // Connection stopped
                isRunning = false;
            break;

            case 0: /* OK */ break;
        }
    }

    // Cleanup
    zsock_destroy (&worker);

    dbg ("[%d] Zone worker ID = %d exits.", self->serverId, self->workerId);
    return NULL;
}

static int
ZoneWorker_handlePrivateRequest (
    ZoneWorker *self,
    zsock_t *global
) {
    zmsg_t *msg;

    // Process messages as they arrive
    if (!(msg = zmsg_recv (global))) {
        dbg ("[%d] Zone worker ID = %d stops working.", self->serverId, self->workerId);
        return -2;
    }

    ZoneWorker_processGlobalPacket (self, msg);

    // Reply back to the sender
    if (zmsg_send (&msg, global) != 0) {
        warning ("[%d] Zone worker ID = %d failed to send a message to the backend.", self->serverId, self->workerId);
        return -1;
    }

    return 0;
}

static void
ZoneWorker_processGlobalPacket (
    ZoneWorker *self,
    zmsg_t *msg
) {
    // Extract the request
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
    zmsg_add (msg, requestAnswer);

    // Clean up
    zframe_destroy (&headerFrame);
}

static int
ZoneWorker_handlePublicRequest (
    ZoneWorker *self,
    zsock_t *worker
) {
    zmsg_t *msg;

    // Process messages as they arrive
    if (!(msg = zmsg_recv (worker))) {
        dbg ("[%d] Zone worker ID = %d stops working.", self->serverId, self->workerId);
        return -2;
    }

    // No message should be with less than 3 frames
    if (zmsg_size (msg) < 2) {
        error ("[%d] Zone worker ID = %d received a malformed message.", self->serverId, self->workerId);
        zmsg_destroy (&msg);
        return -1;
    }

    // Only ToS clients send messages with 2 frames
    if (zmsg_size (msg) == 2) {
        // The first frame is the client identity
        // The second frame is the data of the packet
        ZoneWorker_processClientPacket (self, msg);
    }

    // Reply back to the sender
    if (zmsg_send (&msg, worker) != 0) {
        warning ("[%d] Zone worker ID = %d failed to send a message to the backend.", self->serverId, self->workerId);
    }

    return 0;
}

void
ZoneWorker_destroy (
    ZoneWorker **_self
) {
    ZoneWorker *self = *_self;

    free (self);
    *_self = NULL;
}
