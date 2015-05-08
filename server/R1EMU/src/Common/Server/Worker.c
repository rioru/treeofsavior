/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Worker.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "Worker.h"
#include "Router.h"
#include "Common/Redis/Fields/RedisSocketSession.h"
#include "Common/Redis/Fields/RedisGameSession.h"


// ------ Structure declaration -------


// ------ Static declaration -------
/**
 * @brief Handle a PING request from any entity.
 * @return a zframe_t containing the PONG. Never returns NULL.
 */
static zframe_t *
Worker_handlePingPacket (
    void
);


/**
 * @brief Handle a client request.
 *        The first frame contains client entity, the second frame contains packet data.
 * @param self An allocated Worker structure
 * @param msg The message of the client
 * @return true on success, false otherwise
 */
static bool
Worker_processClientPacket (
    Worker *self,
    zmsg_t *msg
);


/**
 * @brief Handle a request from the public ports
 * @param self An allocated Worker structure
 * @param worker The socket listening on the public port
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
Worker_handlePublicRequest (
    Worker *self,
    zsock_t *worker
);

/**
 * @brief Handle a request from the private ports (coming from global server)
 * @param self An allocated Worker structure
 * @param worker The socket listening on the public port
 * @return -2 on end of stream, -1 on error, 0 on success
*/
static int
Worker_handlePrivateRequest (
    Worker *self,
    zsock_t *global
);

/**
 * @brief Process a message from the the global server
 * @param self An allocated Worker structure
 * @param msg The message coming from the global server
*/
static void
Worker_processGlobalPacket (
    Worker *self,
    zmsg_t *msg
);



// ------ Extern function implementation -------

Worker *
Worker_new (
    uint16_t workerId,
    uint16_t serverId,
    char *globalServerIp,
    int globalServerPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo,
    PacketHandler *packetHandlers,
    int packetHandlersCount
) {
    Worker *self;

    if ((self = calloc (1, sizeof (Worker))) == NULL) {
        return NULL;
    }

    if (!Worker_init (self, workerId, serverId, globalServerIp, globalServerPort, sqlInfo, redisInfo, packetHandlers, packetHandlersCount)) {
        Worker_destroy (&self);
        error ("Worker failed to initialize.");
        return NULL;
    }

    return self;
}


bool
Worker_init (
    Worker *self,
    uint16_t workerId,
    uint16_t serverId,
    char *globalServerIp,
    int globalServerPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo,
    PacketHandler *packetHandlers,
    int packetHandlersCount
) {
    self->workerId = workerId;
    self->serverId = serverId;
    self->globalServerIp = globalServerIp;
    self->globalServerPort = globalServerPort;
    self->packetHandlers = packetHandlers;
    self->packetHandlersCount = packetHandlersCount;

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

bool
Worker_sendToClients (
    Worker *self,
    zlist_t *clients,
    unsigned char *packet,
    size_t packetLen
) {
    zmsg_t *msg = zmsg_new ();

    if (zmsg_addmem (msg, PACKET_HEADER (ROUTER_WORKER_MULTICAST), sizeof (ROUTER_WORKER_MULTICAST)) != 0
    ||  zmsg_addmem (msg, packet, packetLen) != 0
    ) {
        error ("Cannot build the multicast packet.");
        return false;
    }

    // [1 frame data] + [1 frame identity] + [1 frame identity] + ...
    char *identityKey;
    while ((identityKey = zlist_pop (clients)) != NULL) {
        // Add all the clients to the packet
        unsigned char identityBytes[5];
        SocketSession_genId (identityKey, identityBytes);
        zmsg_addmem (msg, identityBytes, sizeof (identityBytes));
    }

    if (zmsg_send (&msg, self->publisher) != 0) {
        error ("Cannot send the multicast packet.");
        return false;
    }

    return true;
}

zlist_t *
Worker_getClientsWithinDistance (
    Worker *self,
    Session *session,
    float x, float y, float z,
    float range
) {
    return Redis_getClientsWithinDistance (self->redis, session->socket.serverId, session->socket.mapId, x, y, z, range);
}

static zframe_t *
Worker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (ROUTER_PONG), sizeof (ROUTER_PONG));
}

static bool
Worker_processClientPacket (
    Worker *self,
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

    // === Build the reply ===
    // We don't need the client packet in the reply
    zmsg_remove (msg, packet);

    // Consider the message as a "normal" message by default
    zframe_t *headerAnswer = zframe_new (PACKET_HEADER (ROUTER_WORKER_NORMAL), sizeof (ROUTER_WORKER_NORMAL));
    zmsg_push (msg, headerAnswer);

    switch (PacketHandler_buildReply (self->packetHandlers, self->packetHandlersCount, &session, zframe_data (packet), zframe_size (packet), msg, self))
    {
        case PACKET_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (zframe_data (packet), zframe_size (packet), NULL);
            zframe_reset (headerAnswer, PACKET_HEADER (ROUTER_WORKER_ERROR), sizeof (ROUTER_WORKER_ERROR));
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
            // TODO
        break;
    }

    // Cleanup
    zframe_destroy (&packet);

    return true;
}

void
Worker_handleRequest (
    Worker *self,
    zmsg_t *msg
) {
    // Extract the request
    zframe_t *identityFrame = zmsg_pop (msg); (void) identityFrame;
    zframe_t *emptyFrame = zmsg_pop (msg); (void) emptyFrame;
    zframe_t *headerFrame = zmsg_pop (msg);
    zframe_t *requestAnswer = NULL;

    RouterRecvHeader header = *((RouterRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case ROUTER_PING:
            requestAnswer = Worker_handlePingPacket ();
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
Worker_worker (
    void *arg
) {
    zframe_t *readyFrame;
    zpoller_t *poller;
    zsock_t *worker, *global;
    bool isRunning = true;

    Worker *self = (Worker *) arg;

    // ============================
    //    Initialize connections
    // ============================

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, ROUTER_BACKEND_ENDPOINT, self->serverId) == -1
    ) {
        error ("[serverId=%d][WorkerId=%d] cannot connect to the backend socket.", self->serverId, self->workerId);
        return NULL;
    }
    info ("[serverId=%d][WorkerId=%d] connected to the backend %s.",
          self->serverId, self->workerId, zsys_sprintf (ROUTER_BACKEND_ENDPOINT, self->serverId));


    // Create and connect a socket to the global server
    if (!(global = zsock_new (ZMQ_REQ))
    ||  zsock_connect (global, ROUTER_GLOBAL_ENDPOINT, self->globalServerIp, self->globalServerPort) == -1
    ) {
        error ("[serverId=%d][WorkerId=%d] cannot bind to the global server %s:%d.", self->serverId, self->globalServerIp, self->globalServerPort);
        return NULL;
    }
    info ("[serverId=%d][WorkerId=%d] connected to the global server %s.",
          self->serverId, self->workerId, zsys_sprintf (ROUTER_GLOBAL_ENDPOINT, self->globalServerIp, self->globalServerPort));


    // Create and bind a publisher to send asynchronous messages to the Router
    if (!(self->publisher = zsock_new (ZMQ_PUB))
    ||  zsock_bind (self->publisher, ROUTER_SUBSCRIBER_ENDPOINT, self->serverId, self->workerId) == -1
    ) {
        error ("[serverId=%d][WorkerId=%d] cannot bind to the subscriber endpoint.", self->serverId);
        return NULL;
    }
    info ("[serverId=%d][WorkerId=%d] bind to the subscriber endpoint %s",
          self->serverId, self->workerId, zsys_sprintf (ROUTER_SUBSCRIBER_ENDPOINT, self->serverId, self->workerId));


    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (PACKET_HEADER (ROUTER_WORKER_READY), sizeof (ROUTER_WORKER_READY)))
    ||  zframe_send (&readyFrame, worker, 0) == -1
    ) {
        error ("[serverId=%d][WorkerId=%d] cannot send a correct ZONE_WORKER_READY state.",
               self->serverId, self->workerId);
        return NULL;
    }


    // Define a poller with the global and the worker socket
    if (!(poller = zpoller_new (global, worker, NULL))) {
        error ("[serverId=%d][WorkerId=%d] cannot create a poller.", self->serverId, self->workerId);
        return NULL;
    }
    dbg ("[serverId=%d][WorkerId=%d] is running and waiting for messages.", self->serverId, self->workerId);


    // TODO : Refactor zpoller into zreactor ?
    while (isRunning) {

        zsock_t *actor = zpoller_wait (poller, -1);
        typedef int (*WorkerRequestHandler) (Worker *self, zsock_t *actor);
        WorkerRequestHandler handler;

        // Get the correct handler based on the actor
        if (actor == worker) {
            handler = Worker_handlePublicRequest;
        } else if (actor == global) {
            handler = Worker_handlePrivateRequest;
        }
        else {
            warning ("[serverId=%d][WorkerId=%d] received a message from an unknown actor. Maybe it is a SIGINT signal?", self->serverId, self->workerId);
            break;
        }

        switch (handler (self, actor)) {
            case -1: // ERROR
                error ("[serverId=%d][WorkerId=%d] encountered an error when handling a request.", self->serverId, self->workerId);
            case -2: // Connection stopped
                isRunning = false;
            break;

            case 0: /* OK */ break;
        }
    }

    // Cleanup
    zsock_destroy (&worker);
    zsock_destroy (&global);

    dbg ("[serverId=%d][WorkerId=%d] exits.", self->serverId, self->workerId);
    return NULL;
}

static int
Worker_handlePrivateRequest (
    Worker *self,
    zsock_t *global
) {
    zmsg_t *msg;

    // Process messages as they arrive
    if (!(msg = zmsg_recv (global))) {
        dbg ("[serverId=%d][WorkerId=%d] stops working.", self->serverId, self->workerId);
        return -2;
    }

    Worker_processGlobalPacket (self, msg);

    // Reply back to the sender
    if (zmsg_send (&msg, global) != 0) {
        warning ("[serverId=%d][WorkerId=%d] failed to send a message to the backend.", self->serverId, self->workerId);
        return -1;
    }

    return 0;
}

static void
Worker_processGlobalPacket (
    Worker *self,
    zmsg_t *msg
) {
    // Extract the request
    zframe_t *headerFrame = zmsg_pop (msg);
    zframe_t *requestAnswer = NULL;

    RouterRecvHeader header = *((RouterRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case ROUTER_PING:
            requestAnswer = Worker_handlePingPacket ();
        break;

        default:
            error ("[serverId=%d][WorkerId=%d] : Packet type %d not handled.", self->serverId, self->workerId, header);
        break;
    }

    // Rebuild the answer message
    zmsg_add (msg, requestAnswer);

    // Clean up
    zframe_destroy (&headerFrame);
}

static int
Worker_handlePublicRequest (
    Worker *self,
    zsock_t *worker
) {
    zmsg_t *msg;

    // Process messages as they arrive
    if (!(msg = zmsg_recv (worker))) {
        dbg ("[serverId=%d][WorkerId=%d] stops working.", self->serverId, self->workerId);
        return -2;
    }

    // No message should be with less than 3 frames
    if (zmsg_size (msg) < 2) {
        error ("[serverId=%d][WorkerId=%d] received a malformed message.", self->serverId, self->workerId);
        zmsg_destroy (&msg);
        return -1;
    }

    // Only ToS clients send messages with 2 frames
    if (zmsg_size (msg) == 2) {
        // The first frame is the client identity
        // The second frame is the data of the packet
        Worker_processClientPacket (self, msg);
    }

    // Reply back to the sender
    if (zmsg_send (&msg, worker) != 0) {
        warning ("[serverId=%d][WorkerId=%d] failed to send a message to the backend.", self->serverId, self->workerId);
    }

    return 0;
}

void
Worker_destroy (
    Worker **_self
) {
    Worker *self = *_self;

    free (self);
    *_self = NULL;
}
