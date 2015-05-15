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
#include "Common/utils/random.h"
#include "Common/Redis/Fields/RedisSession.h"
#include "Common/Redis/Fields/RedisSocketSession.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Crypto/Crypto.h"


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

/**
 * @brief Build a reply for a given packet
 * @param[in] self A pointer to the current worker
 * @param[in] packetHandlers The packet handlers
 * @param[in] handlersCount The number of handlers in the handlers array
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @return PacketHandlerState
 */
static bool
Worker_buildReply (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
);

/**
 * @brief Read the packet and reply based on the packet handler
 * @param[in] self A pointer to the current worker
 * @param[in] packetHandlers The packet handlers
 * @param[in] handlersCount The number of handlers in the handlers array
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @return PacketHandlerState
 */
static PacketHandlerState
Worker_handlePacket (
    Worker *self,
    const PacketHandler *packetHandlers,
    size_t handlersCount,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
);


/**
 * @brief Build a reply based on a single request
 * @param[in] self A pointer to the current worker
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @param[in] headerAnswer The header of the answer message
 * @return true on success, false otherwise
 */
static bool
Worker_processOneRequest (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
);


/**
 * @brief Build a reply based on a multiple requests
 * @param[in] self A pointer to the current worker
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @param[in] headerAnswer The header of the answer message
 * @return true on success, false otherwise
 */
static bool
Worker_processMultipleRequests (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
);

/**
 * @brief Check the size of a packet based on the packet header
 * @param packetSize The size of the packet
 * @param packet The packet
 * @param[out] cryptHeader an allocated CryptPacketHeader
 */
static bool
Worker_checkPacketSize (
    size_t packetSize,
    unsigned char *packet,
    CryptPacketHeader *cryptHeader
);

// ------ Extern function implementation -------

Worker *
Worker_new (
    WorkerStartupInfo *info
) {
    Worker *self;

    if ((self = calloc (1, sizeof (Worker))) == NULL) {
        return NULL;
    }

    if (!Worker_init (self, info)) {
        Worker_destroy (&self);
        error ("Worker failed to initialize.");
        return NULL;
    }

    return self;
}

bool
Worker_init (
    Worker *self,
    WorkerStartupInfo *info
) {
    memcpy (&self->info, info, sizeof (self->info));

    // ===================================
    //          Initialize MySQL
    // ===================================
    if (!(self->sqlConn = MySQL_new (&info->sqlInfo))) {
        error ("Cannot initialize a new MySQL connection.");
        return false;
    }
    MySQL_connect (self->sqlConn);

    // ===================================
    //     Initialize Redis connection
    // ===================================
    if (!(self->redis = Redis_new (&info->redisInfo))) {
        error ("Cannot initialize a new Redis connection.");
        return false;
    }
    Redis_connect (self->redis);

    // Initialize random seed
    self->seed = R1EMU_seed_random (self->info.routerId);

    return true;
}

bool
WorkerStartupInfo_init (
    WorkerStartupInfo *self,
    uint16_t workerId,
    uint16_t routerId,
    char *globalServerIp,
    int globalServerPort,
    MySQLStartupInfo *sqlInfo,
    RedisStartupInfo *redisInfo,
    const PacketHandler *packetHandlers,
    int packetHandlersCount
) {
    self->workerId = workerId;
    self->routerId = routerId;
    self->globalServerIp = globalServerIp;
    self->globalServerPort = globalServerPort;
    memcpy (&self->sqlInfo, sqlInfo, sizeof (self->sqlInfo));
    memcpy (&self->redisInfo, redisInfo, sizeof (self->redisInfo));
    self->packetHandlers = packetHandlers;
    self->packetHandlersCount = packetHandlersCount;

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
    return Redis_getClientsWithinDistance (self->redis, session->socket.routerId, session->socket.mapId, x, y, z, range);
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
    zframe_t *packetFrame = zmsg_next (msg);

    // Convert the frame to socketId
    unsigned char socketId [SOCKET_SESSION_ID_SIZE];

    // Generate the socketId key
    SocketSession_genSocketId (zframe_data (socketIdFrame), socketId);

    // Request the Session
    RedisSessionKey sessionKey = {
        .socketKey = {
            .routerId = self->info.routerId,
            .socketId = socketId
        }
    };
    if (!(Redis_getSession (self->redis, &sessionKey, &session))) {
        error ("Cannot retrieve a Game Session.");
        return false;
    }

    // We don't need the client packet in the reply
    zmsg_remove (msg, packetFrame);

    // Consider the message as a "normal" message by default
    zframe_t *headerAnswer = zframe_new (PACKET_HEADER (ROUTER_WORKER_NORMAL), sizeof (ROUTER_WORKER_NORMAL));
    zmsg_push (msg, headerAnswer);

    // === Build the message reply ===
    unsigned char *packet = zframe_data (packetFrame);
    size_t packetSize = zframe_size (packetFrame);

    if (!(Worker_buildReply (self, &session, packet, packetSize, msg, headerAnswer))) {
        error ("Cannot build a reply for the following packet :");
        buffer_print (packet, packetSize, NULL);
        return false;
    }

    // Cleanup
    zframe_destroy (&packetFrame);

    return true;
}

static bool
Worker_buildReply (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
) {
    // Check the packet size
    CryptPacketHeader cryptHeader;

    if (!(Worker_checkPacketSize (packetSize, packet, &cryptHeader))) {
        error ("Wrong packet size, cannot build a reply");
        return false;
    }

    // A single packet may contain multiple requests
    else if ((packetSize - sizeof (CryptPacketHeader)) > cryptHeader.size) {
        if (!(Worker_processMultipleRequests (self, session, packet, packetSize, msg, headerAnswer))) {
            error ("Cannot process properly one of multiple requests.");
            return false;
        }
    }

    // Everything normal here, a single packet contains a single request
    else {
        if (!(Worker_processOneRequest (self, session, packet, packetSize, msg, headerAnswer))) {
            error ("Cannot process properly a reply.");
            return false;
        }
    }

    return true;
}


static bool
Worker_processMultipleRequests (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
) {
    CryptPacketHeader cryptHeader;

    // Divide them and treat them sequentially
    size_t packetPos = 0;
    size_t packetSizeRemaining = packetSize;
    while (packetSizeRemaining > 0)
    {
        if (!(Worker_checkPacketSize (packetSizeRemaining, &packet[packetPos], &cryptHeader))) {
            error ("Wrong sub packet size.");
            return false;
        }

        // Copy to another packet the sub request
        size_t subPacketSize = cryptHeader.size + sizeof (CryptPacketHeader);
        unsigned char *subPacket = alloca (subPacketSize);
        memcpy (subPacket, &packet[packetPos], subPacketSize);

        if ((!(Worker_processOneRequest (self, session, subPacket, subPacketSize, msg, headerAnswer)))) {
            error ("Cannot process properly a reply.");
            return false;
        }

        // == Iterate to the next reply ==
        packetPos += subPacketSize;
        packetSizeRemaining -= subPacketSize;
    }

    return true;
}

static bool
Worker_checkPacketSize (
    size_t packetSize,
    unsigned char *packet,
    CryptPacketHeader *cryptHeader
) {
    // Check if we can read a CryptPacketHeader
    if (packetSize < sizeof (CryptPacketHeader)) {
        error ("The packet received is too small to be read. Ignore request.");
        return false;
    }

    // Unwrap the crypt packet header, and check the cryptHeader size
    CryptPacket_getHeader (packet, cryptHeader);
    if ((packetSize - sizeof (CryptPacketHeader)) < cryptHeader->size) {
        error ("The real packet size (0x%x) doesn't match with the packet size in the header (0x%x). Ignore request.",
            packetSize, cryptHeader->size);
        return false;
    }

    return true;
}

static bool
Worker_processOneRequest (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *msg,
    zframe_t *headerAnswer
) {
    // Unwrap the crypt packet header, and check the cryptHeader size
    // In a single request process, it must match exactly the same size
    CryptPacketHeader cryptHeader;
    CryptPacket_unwrapHeader (&packet, &cryptHeader);
    if ((packetSize - sizeof (cryptHeader)) != cryptHeader.size) {
        error ("The real packet size (0x%x) doesn't match with the packet size in the header (0x%x). Ignore request.",
            packetSize, cryptHeader.size);
        return false;
    }

    // Uncrypt the packet if everything is OK
    if (!Crypto_uncryptPacket (&cryptHeader, &packet)) {
        error ("Cannot uncrypt the client packet. Ignore request.");
        return false;
    }

    // Answer
    switch (Worker_handlePacket (self, self->info.packetHandlers, self->info.packetHandlersCount, session, packet, packetSize, msg))
    {
        case PACKET_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (packet, packetSize, NULL);
            zframe_reset (headerAnswer, PACKET_HEADER (ROUTER_WORKER_ERROR), sizeof (ROUTER_WORKER_ERROR));
        break;

        case PACKET_HANDLER_OK:
        break;

        case PACKET_HANDLER_UPDATE_SESSION:
            if (!(Redis_updateSession (self->redis, session))) {
                error ("Cannot update the Session.");
                return false;
            }
        break;

        case PACKET_HANDLER_DELETE_SESSION: {
            RedisSessionKey sessionKey = {
                .socketKey = {
                    .routerId = session->socket.routerId,
                    .socketId = session->socket.socketId
                }
            };
            if (!(Redis_flushSession (self->redis, &sessionKey))) {
                error ("Cannot delete the Session.");
                return false;
            }
        }
        break;
    }

    return true;
}


static PacketHandlerState
Worker_handlePacket (
    Worker *self,
    const PacketHandler *packetHandlers,
    size_t handlersCount,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    PacketHandlerFunction handler;

    // Read the packet
    ClientPacketHeader header;
    ClientPacket_unwrapHeader (&packet, &header);
    size_t dataSize = packetSize - sizeof (CryptPacketHeader) - sizeof (ClientPacketHeader);

    // Get the corresponding packet handler
    if (header.type > handlersCount) {
        error ("Invalid packet type (%#x / %#x). Ignore request.", header.type, handlersCount);
        return PACKET_HANDLER_ERROR;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = packetHandlers [header.type].handler)) {
        error ("Cannot find handler for the requested packet type : %s",
            (header.type <PACKET_TYPES_MAX_INDEX) ?
               packetTypeInfo.packets[header.type].string : "UNKNOWN"
        );
        return PACKET_HANDLER_ERROR;
    }

    // Call the handler
    special ("Calling [%s] handler", packetTypeInfo.packets[header.type].string);
    return handler (self, session, packet, dataSize, reply);
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

bool
Worker_start (
    Worker *self
) {
    if (zthread_new (Worker_mainLoop, self) != 0) {
        error ("Cannot create Server worker ID = %d.", self->info.workerId);
        return false;
    }

    return true;
}


void *
Worker_mainLoop (
    void *arg
) {
    zpoller_t *poller;
    zsock_t *worker, *global;
    bool isRunning = true;

    Worker *self = (Worker *) arg;

    // ============================
    //    Initialize connections
    // ============================

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, ROUTER_BACKEND_ENDPOINT, self->info.routerId) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot connect to the backend socket.", self->info.routerId, self->info.workerId);
        return NULL;
    }
    info ("[routerId=%d][WorkerId=%d] connected to the backend %s.",
          self->info.routerId, self->info.workerId, zsys_sprintf (ROUTER_BACKEND_ENDPOINT, self->info.routerId));

    // Create and connect a socket to the global server
    if (!(global = zsock_new (ZMQ_REQ))
    ||  zsock_connect (global, ROUTER_GLOBAL_ENDPOINT, self->info.globalServerIp, self->info.globalServerPort) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot bind to the global server %s:%d.", self->info.routerId, self->info.workerId, self->info.globalServerIp, self->info.globalServerPort);
        return NULL;
    }
    info ("[routerId=%d][WorkerId=%d] connected to the global server %s.",
          self->info.routerId, self->info.workerId, zsys_sprintf (ROUTER_GLOBAL_ENDPOINT, self->info.globalServerIp, self->info.globalServerPort));


    // Create and bind a publisher to send asynchronous messages to the Router
    if (!(self->publisher = zsock_new (ZMQ_PUB))
    ||  zsock_bind (self->publisher, ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId, self->info.workerId) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot bind to the subscriber endpoint.", self->info.routerId);
        return NULL;
    }
    info ("[routerId=%d][WorkerId=%d] bind to the subscriber endpoint %s",
          self->info.routerId, self->info.workerId, zsys_sprintf (ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId, self->info.workerId));

    // Tell to the broker we're ready for work
    zmsg_t *readyMsg = zmsg_new ();
    if (zmsg_addmem (readyMsg, PACKET_HEADER (ROUTER_WORKER_READY), sizeof (ROUTER_WORKER_READY)) == -1
    ||  zmsg_addmem (readyMsg, PACKET_HEADER (self->info.workerId), sizeof (self->info.workerId)) == -1
    ||  zmsg_send (&readyMsg, worker) == -1
    ) {
        error ("[routerId=%d][WorkerId=%d] cannot send a correct ROUTER_WORKER_READY state.",
               self->info.routerId, self->info.workerId);
        return NULL;
    }

    // Define a poller with the global and the worker socket
    if (!(poller = zpoller_new (global, worker, NULL))) {
        error ("[routerId=%d][WorkerId=%d] cannot create a poller.", self->info.routerId, self->info.workerId);
        return NULL;
    }
    info ("[routerId=%d][WorkerId=%d] is running and waiting for messages.", self->info.routerId, self->info.workerId);


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
            warning ("[routerId=%d][WorkerId=%d] received a message from an unknown actor. Maybe it is a SIGINT signal?", self->info.routerId, self->info.workerId);
            break;
        }

        switch (handler (self, actor)) {
            case -1: // ERROR
                error ("[routerId=%d][WorkerId=%d] encountered an error when handling a request.", self->info.routerId, self->info.workerId);
            case -2: // Connection stopped
                isRunning = false;
            break;

            case 0: /* OK */ break;
        }
    }

    // Cleanup
    zsock_destroy (&worker);
    zsock_destroy (&global);

    dbg ("[routerId=%d][WorkerId=%d] exits.", self->info.routerId, self->info.workerId);
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
        dbg ("[routerId=%d][WorkerId=%d] stops working.", self->info.routerId, self->info.workerId);
        return -2;
    }

    Worker_processGlobalPacket (self, msg);

    // Reply back to the sender
    if (zmsg_send (&msg, global) != 0) {
        warning ("[routerId=%d][WorkerId=%d] failed to send a message to the backend.", self->info.routerId, self->info.workerId);
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
            error ("[routerId=%d][WorkerId=%d] : Packet type %d not handled.", self->info.routerId, self->info.workerId, header);
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
        dbg ("[routerId=%d][WorkerId=%d] stops working.", self->info.routerId, self->info.workerId);
        return -2;
    }

    // No message should be with less than 3 frames
    if (zmsg_size (msg) < 2) {
        error ("[routerId=%d][WorkerId=%d] received a malformed message.", self->info.routerId, self->info.workerId);
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
        warning ("[routerId=%d][WorkerId=%d] failed to send a message to the backend.", self->info.routerId, self->info.workerId);
    }

    return 0;
}

void
WorkerStartupInfo_free (
    WorkerStartupInfo *self
) {
    MySQLStartupInfo_free (&self->sqlInfo);
    RedisStartupInfo_free (&self->redisInfo);
    free (self->globalServerIp);

}

void
Worker_destroy (
    Worker **_self
) {
    Worker *self = *_self;

    Redis_destroy (&self->redis);
    MySQL_destroy (&self->sqlConn);

    free (self);
    *_self = NULL;
}
