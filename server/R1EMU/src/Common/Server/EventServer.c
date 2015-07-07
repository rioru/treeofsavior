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
#include "EventServer.h"
#include "EventHandler.h"
#include "Common/Redis/Redis.h"
#include "Common/Redis/Fields/RedisGameSession.h"

// ------ Structure declaration -------
/**
 * @brief EventServer communicates with the Router and broadcast packets to the clients
 */
struct EventServer
{
    /** Socket connected to the workers */
    zsock_t *workers;

    /** Frontend connected to the router */
    zsock_t *router;

    /** Redis connection **/
    Redis *redis;

    /** EventServer information */
    EventServerStartupInfo info;
};


// ------ Static declaration ------
static bool
EventServer_subscribe (
    EventServer *self
);


// ------ Extern function implementation ------

EventServer *
EventServer_new (
    EventServerStartupInfo *info
) {
    EventServer *self;

    if ((self = calloc (1, sizeof (EventServer))) == NULL) {
        return NULL;
    }

    if (!EventServer_init (self, info)) {
        EventServer_destroy (&self);
        error ("EventServer failed to initialize.");
        return NULL;
    }

    return self;
}


bool
EventServer_init (
    EventServer *self,
    EventServerStartupInfo *info
) {
    memcpy (&self->info, info, sizeof (self->info));

    // Create a unique publisher for the EventServer
    if (!(self->workers = zsock_new (ZMQ_SUB))) {
        error ("Cannot allocate a new Server SUBSCRIBER");
        return false;
    }

    // Create a connection to the router
    if (!(self->router = zsock_new (ZMQ_PUB))) {
        error ("Cannot create zsock to the router.");
        return false;
    }

    // Initialize Redis connection
    if (!(self->redis = Redis_new (&info->redisInfo))) {
        error ("Cannot initialize a new Redis connection.");
        return false;
    }

    return true;
}

bool
EventServerStartupInfo_init (
    EventServerStartupInfo *self,
    uint16_t routerId,
    uint16_t workersCount,
    char *redisHostname,
    int redisPort
) {
    memset (self, 0, sizeof (EventServerStartupInfo));

    self->routerId = routerId;
    self->workersCount = workersCount;

    if (!(RedisStartupInfo_init (&self->redisInfo, redisHostname, redisPort))) {
        error ("Cannot initialize Redis startup.");
        return false;
    }

    return true;
}

static bool
EventServer_handleEvent (
    EventServer *self,
    zmsg_t *msg
) {
    zframe_t *eventTypeFrame;

    // Get the event type frame
    if (!(eventTypeFrame = zmsg_first (msg))) {
        error ("Event type cannot be retrieved.");
        return false;
    }

    // Convert the event type frame to a EventServerType
    EventServerType eventType = *((EventServerType *) zframe_data (eventTypeFrame));
    zframe_destroy (&eventTypeFrame);

    // Get the event data
    void *eventData = zframe_data (zmsg_next (msg));

    switch (eventType)
    {
        case EVENT_SERVER_TYPE_COMMANDER_MOVE : {
            GameEventCommanderMove *event = (GameEventCommanderMove *) eventData;
            if (!(EventHandler_commanderMove (self, event))) {
                error ("Event failed.");
                return false;
            }
        } break;

        case EVENT_SERVER_TYPE_MOVE_STOP : {
            GameEventMoveStop *event = (GameEventMoveStop *) eventData;
            if (!(EventHandler_moveStop (self, event))) {
                error ("Event failed.");
                return false;
            }
        } break;

        case EVENT_SERVER_TYPE_REST_SIT : {
            GameEventRestSit *event = (GameEventRestSit *) eventData;
            if (!(EventHandler_restSit (self, event))) {
                error ("Event failed.");
                return false;
            }
        } break;

        case EVENT_SERVER_TYPE_ENTER_PC : {
            GameEventPcEnter *event = (GameEventPcEnter *) eventData;
            if (!(EventHandler_enterPc (self, event))) {
                error ("Event failed.");
                return false;
            }
        } break;

        default :
            error ("Unknown event type received : %d", eventType);
        break;
    }

    return true;
}

bool
EventServer_getGameSessionBySocketId (
    EventServer *self,
    uint16_t routerId,
    uint8_t *socketId,
    GameSession *gameSession
) {
    return Redis_getGameSessionBySocketId (self->redis, routerId, socketId, gameSession);
}

static bool
EventServer_subscribe (
    EventServer *self
) {
    zmsg_t *msg;
    zframe_t *header;

    // Receive the message from the publisher socket
    if (!(msg = zmsg_recv (self->workers))) {
        // Interrupt
        return false;
    }

    // Get the header frame of the message
    if (!(header = zmsg_pop (msg))) {
        error ("Frame header cannot be retrieved.");
        return false;
    }

    // Convert the header frame to a EventServerHeader
    EventServerHeader packetHeader = *((EventServerHeader *) zframe_data (header));
    zframe_destroy (&header);

    switch (packetHeader)
    {
        case EVENT_SERVER_EVENT :
            if (!(EventServer_handleEvent (self, msg))) {
                error ("Cannot handle the event properly.");
                return false;
            }
        break;

        default:
            warning ("Server subscriber received an unknown header : %x", packetHeader);
        break;
    }

    // Cleanup
    zmsg_destroy (&msg);

    return true;
}

bool
EventServer_sendToClients (
    EventServer *self,
    zlist_t *clients,
    uint8_t *packet,
    size_t packetLen
) {
    bool result = true;
    zmsg_t *msg = NULL;

    if ((!(msg = zmsg_new ()))
    ||  zmsg_addmem (msg, PACKET_HEADER (ROUTER_WORKER_MULTICAST), sizeof (ROUTER_WORKER_MULTICAST)) != 0
    ||  zmsg_addmem (msg, packet, packetLen) != 0
    ) {
        error ("Cannot build the multicast packet.");
        result = false;
        goto cleanup;
    }

    // [1 frame data] + [1 frame identity] + [1 frame identity] + ...
    char *identityKey;
    for (identityKey = zlist_first (clients); identityKey != NULL; identityKey = zlist_next (clients)) {
        // Add all the clients to the packet
        uint8_t identityBytes[5];
        SocketSession_genId (identityKey, identityBytes);
        if (zmsg_addmem (msg, identityBytes, sizeof (identityBytes)) != 0) {
            error ("Cannot add the identity in the message.");
            result = false;
            goto cleanup;
        }
    }

    if (zmsg_send (&msg, self->router) != 0) {
        error ("Cannot send the multicast packet to the Router.");
        result = false;
        goto cleanup;
    }

cleanup:
    zmsg_destroy (&msg);
    return result;
}


uint16_t
EventServer_getRouterId (
    EventServer *self
) {
    return self->info.routerId;
}

zlist_t *
EventServer_getClientsWithinRange (
    EventServer *self,
    uint16_t routerId,
    uint32_t mapId,
    uint8_t *ignoredSocketId,
    PositionXZ *center,
    float range
) {
    return Redis_getClientsWithinDistance (self->redis, routerId, mapId, center, range, ignoredSocketId);
}

bool
EventServer_start (
    EventServer *self
) {
    // Start Redis
    if (!(Redis_connect (self->redis))) {
        error ("Cannot connect to the Redis Server.");
        return false;
    }

    // Bind the connection to the router
    if (zsock_bind (self->router, ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId) != 0) {
        error ("Failed to bind to the subscriber endpoint.");
        return false;
    }

    // Initialize subscribers
    for (int workerId = 0; workerId < self->info.workersCount; workerId++) {
        if (zsock_connect (self->workers, EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId) != 0) {
            error ("Failed to connect to the subscriber endpoint %d:%d.", self->info.routerId, workerId);
            return false;
        }
        info ("EventServer subscribed to %s", zsys_sprintf (EVENT_SERVER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId));
        // Subscribe to all messages, without any filter
        zsock_set_subscribe (self->workers, "");
    }

    // Listen to the subscriber socket
    while (EventServer_subscribe (self)) {

    }

    info ("EventServer exits...");
    return true;
}


void
EventServer_destroy (
    EventServer **_self
) {
    EventServer *self = *_self;

    free (self);
    *_self = NULL;
}
