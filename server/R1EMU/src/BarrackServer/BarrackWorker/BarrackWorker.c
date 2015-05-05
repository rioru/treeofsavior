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
#include "Common/Redis/Fields/RedisSocketSession.h"
#include "Common/Redis/Fields/RedisGameSession.h"


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
 * @return true on sucess, false otherwise
 */
static bool
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
    int workerId,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
) {
    BarrackWorker *self;

    if ((self = calloc (1, sizeof (BarrackWorker))) == NULL) {
        return NULL;
    }

    if (!BarrackWorker_init (self, workerId, sqlInfo, redisInfo)) {
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
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
) {
    // Initialize seed
    self->seed = R1EMU_seed_random (workerId);
    self->workerId = workerId;

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

static void
BarrackWorker_handlePingPacket (
    zframe_t *headerFrame
) {
    zframe_reset (headerFrame, PACKET_HEADER (BARRACK_SERVER_PONG), sizeof (BARRACK_SERVER_PONG));
}

static bool
BarrackWorker_processClientPacket (
    BarrackWorker *self,
    zmsg_t *msg
) {
    Session session = {{0}};

    // Read the message
    zframe_t *socketIdFrame = zmsg_first (msg);
    zframe_t *packet = zmsg_next (msg);

    // Request a Game Session from the redis server (in the barrack zone)
    if (!(Redis_requestSession (self->redis, BARRACK_SERVER_ZONE_ID, zframe_data (socketIdFrame), &session))) {
        error ("Cannot retrieve a Game Session.");
        return false;
    }

    // Build the reply
    // We don't need the client packet in the reply
    zmsg_remove (msg, packet);

    // Consider the message as a "normal" message by default
    zframe_t *headerAnswer = zframe_new (PACKET_HEADER (BARRACK_SERVER_WORKER_NORMAL), sizeof (BARRACK_SERVER_WORKER_NORMAL));
    zmsg_push (msg, headerAnswer);

    switch (PacketHandler_buildReply (barrackHandlers, sizeof_array (barrackHandlers), &session, zframe_data (packet), zframe_size (packet), msg, self))
    {
        case PACKET_HANDLER_ERROR:
            error ("The following packet produced an error :");
            buffer_print (zframe_data (packet), zframe_size (packet), NULL);
            zframe_reset (headerAnswer, PACKET_HEADER (BARRACK_SERVER_WORKER_ERROR), sizeof (BARRACK_SERVER_WORKER_ERROR));
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


static void
BarrackWorker_processInternPacket (
    BarrackWorker *self,
    zmsg_t *msg
) {
    // Read the message
    zframe_t *socketIdFrameFrame = zmsg_first (msg); (void) socketIdFrameFrame;
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
    zsock_t *worker;
    zframe_t *readyFrame;

    BarrackWorker *self = (BarrackWorker *) arg;

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, BARRACK_SERVER_BACKEND_ENDPOINT) == -1
    ) {
        error ("Barrack worker ID = %d cannot connect to the backend socket.", self->workerId);
        return NULL;
    }

    // Create a publisher to send asynchronous messages to the barrack server
    if (!(self->publisher = zsock_new (ZMQ_PUB))
    ||  zsock_bind (self->publisher, BARRACK_SERVER_SUBSCRIBER_ENDPOINT, self->workerId) == -1
    ) {
        error ("Barrack worker ID = %d cannot bind to the subscriber endpoint.", self->workerId);
        return NULL;
    }
    info ("Barrack worker ID %d bind to the subscriber endpoint %s", self->workerId, zsys_sprintf (BARRACK_SERVER_SUBSCRIBER_ENDPOINT, self->workerId));

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (PACKET_HEADER (BARRACK_SERVER_WORKER_READY), sizeof (BARRACK_SERVER_WORKER_READY)))
    ||  zframe_send (&readyFrame, worker, 0) == -1
    ) {
        error ("Barrack worker ID = %d cannot send a correct BARRACK_SERVER_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("Barrack worker ID %d is running and waiting for messages.", self->workerId);

    while (true)
    {
        // Process messages as they arrive
        if (!(msg = zmsg_recv (worker))) {
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
            if (!(BarrackWorker_processClientPacket (self, msg))) {
                error ("Cannot process the client packet.");
            }
        }

        else {
            // This is a message from intern entities of the network
            //! Nothing too much important should be here, as the clients
            //! could craft a fake packet with 3 frames
            // TODO : Check the identity of the sender (it shouldn't be a client)
            BarrackWorker_processInternPacket (self, msg);
        }

        // Reply back to the sender
        if (zmsg_send (&msg, worker) != 0) {
            warning ("Barrack worker ID %d failed to send a message.", self->workerId);
        }
    }

    // Cleanup
    zsock_destroy (&worker);

    dbg ("Barrack worker ID %d exits.", self->workerId);
    return NULL;
}


bool
BarrackWorker_sendToClients (
    BarrackWorker *self,
    zlist_t *clients,
    unsigned char *packet,
    size_t packetLen
) {
    zmsg_t *msg = zmsg_new ();

    if (zmsg_addmem (msg, PACKET_HEADER (BARRACK_SERVER_WORKER_MULTICAST), sizeof (BARRACK_SERVER_WORKER_MULTICAST)) != 0
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

void
BarrackWorker_destroy (
    BarrackWorker **_self
) {
    BarrackWorker *self = *_self;

    free (self);
    *_self = NULL;
}
