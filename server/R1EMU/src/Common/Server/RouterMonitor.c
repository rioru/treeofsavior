/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file RouterMonitor.c
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ------ Includes -------
#include "RouterMonitor.h"
#include "Router.h"
#include "Common/Session/Session.h"
#include "Common/Redis/Fields/RedisSocketSession.h"

// ------ Structure declaration -------
/**
 * @brief RouterMonitor contains needed information about client connection
 */
struct RouterMonitor {
    RouterMonitorStartupInfo info;

    /** FD => Socket Id hashtable */
    zhash_t *connected;

    /** Database connection */
    Redis *redis;
    MySQL *sql;
};

// ------ Static declaration -------
/**
 * @brief Router Monitor handler
 * @param loop The reactor handler
 * @param frontend The monitor socket
 * @param self The RouterMonitor
 * @return 0 on success, -1 on error
 */
static int
RouterMonitor_monitor (
    zloop_t *loop,
    zsock_t *monitor,
    void *_self
);

/**
 * @brief Router Subscriber handler
 * @param loop The reactor handler
 * @param frontend The monitor socket
 * @param self The RouterMonitor
 * @return 0 on success, -1 on error
 */
static int
RouterMonitor_subscribe (
    zloop_t *loop,
    zsock_t *monitor,
    void *_self
);


// ------ Extern functions implementation -------
RouterMonitor *
RouterMonitor_new (
    RouterMonitorStartupInfo *info
) {
    RouterMonitor *self;

    if ((self = calloc (1, sizeof (RouterMonitor))) == NULL) {
        return NULL;
    }

    if (!RouterMonitor_init (self, info)) {
        RouterMonitor_destroy (&self);
        error ("RouterMonitor failed to initialize.");
        return NULL;
    }

    return self;
}


bool
RouterMonitor_init (
    RouterMonitor *self,
    RouterMonitorStartupInfo *info
) {
    memcpy (&self->info, info, sizeof (self->info));
    RouterMonitorStartupInfo_destroy (&info);

    // Allocate the connected clients hashtable
    if (!(self->connected = zhash_new ())) {
        error ("Cannot allocate a new connected clients hashtable.");
        return false;
    }

    if (!(self->redis = Redis_new (&self->info.redisInfo))) {
        error ("Cannot initialize the Redis connection.");
        return false;
    }

    if (!(self->sql = MySQL_new (&self->info.sqlInfo))) {
        error ("Cannot initialize the MySQL connection.");
        return false;
    }

    return true;
}


RouterMonitorStartupInfo *
RouterMonitorStartupInfo_new (
    zsock_t *frontend,
    uint16_t routerId,
    RedisStartupInfo *redisInfo,
    MySQLStartupInfo *sqlInfo
) {
    RouterMonitorStartupInfo *self;

    if ((self = calloc (1, sizeof (RouterMonitorStartupInfo))) == NULL) {
        return NULL;
    }

    if (!RouterMonitorStartupInfo_init (self, frontend, routerId, redisInfo, sqlInfo)) {
        RouterMonitorStartupInfo_destroy (&self);
        error ("RouterMonitorStartupInfo failed to initialize.");
        return NULL;
    }

    return self;
}


bool
RouterMonitorStartupInfo_init (
    RouterMonitorStartupInfo *self,
    zsock_t *frontend,
    uint16_t routerId,
    RedisStartupInfo *redisInfo,
    MySQLStartupInfo *sqlInfo
) {
    self->frontend = frontend;
    self->routerId = routerId;

    memcpy (&self->redisInfo, redisInfo, sizeof (self->redisInfo));
    memcpy (&self->sqlInfo, sqlInfo, sizeof (self->sqlInfo));

    return true;
}


static int
RouterMonitor_monitor (
    zloop_t *loop,
    zsock_t *monitor,
    void *_self
) {
    RouterMonitor *self = _self;
    zmsg_t *msg;
    zframe_t *header;

    // Receive the message from the Router
    if (!(msg = zmsg_recv (monitor))) {
        // Interrupt
        return 0;
    }

    // Get the frame header of the message
    if (!(header = zmsg_first (msg))) {
        error ("Frame header cannot be retrieved.");
        return -1;
    }

    if (zframe_streq (header, "ACCEPTED")) {
        // A client just connected to the Router.

        // Get the socket file descriptor
        zframe_t *fdFrame = zmsg_next (msg);
        uint64_t fdClient = strtoll (zframe_data (fdFrame), NULL, 10);

        // Check if this file descriptor is still used
        unsigned char fdClientKey [ROUTER_MONITOR_FDKEY_SIZE];
        RouterMonitor_genKey (fdClient, fdClientKey);

        zframe_t *clientFrame;
        // Check if it already exists in the table
        if ((clientFrame = zhash_lookup (self->connected, fdClientKey)) != NULL) {
            unsigned char sessionKey [ROUTER_MONITOR_FDKEY_SIZE];
            SocketSession_genKey (zframe_data (clientFrame), sessionKey);
            error ("The client FD=%d just connected, but another client has still this FD (previously : %s)",
                   fdClient, sessionKey);
            // TODO : Decide what to do in this case
            return -1;
        }
    }

    else if (zframe_streq (header, "DISCONNECTED")) {
        // A client just disconnected from the Router.

        // Get the socket file descriptor
        zframe_t *fdFrame = zmsg_next (msg);
        uint64_t fdClient = strtoll (zframe_data (fdFrame), NULL, 10);

        // Check if this file descriptor is already used
        unsigned char fdClientKey [ROUTER_MONITOR_FDKEY_SIZE];
        RouterMonitor_genKey (fdClient, fdClientKey);

        zframe_t *clientFrame;
        if ((clientFrame = zhash_lookup (self->connected, fdClientKey)) == NULL) {
            // The client just disconnected, but no client has been registred using this fd
            // It happens when the client connects but send no data to the server
            // TODO : Decide what to do in this case, probably nothing
            warning ("Cannot find the clientFrame when disconnecting.");
            dbg ("DISCONNECTED : Key = %s", fdClientKey);
        }
        else {
            // Everything is okay here, disconnect gracefully the client
            unsigned char sessionKey [SOCKET_SESSION_KEY_SIZE];
            SocketSession_genKey (zframe_data (clientFrame), sessionKey);

            // TODO : Flush the session here
            Redis_flushSocketSession (self->redis, self->info.routerId, sessionKey);

            // Remove the key from the "connected" hashtable
            zhash_delete (self->connected, fdClientKey);
            zframe_destroy (&clientFrame);

            info ("%s session successfully flushed !", sessionKey);
        }
    }

    // Cleanup
    zmsg_destroy (&msg);

    return 0;
}


static int
RouterMonitor_subscribe (
    zloop_t *loop,
    zsock_t *monitor,
    void *_self
) {
    RouterMonitor *self = _self;
    zmsg_t *msg;
    zframe_t *header;

    // Receive the message from the Router
    if (!(msg = zmsg_recv (monitor))) {
        // Interrupt
        return 0;
    }

    // Get the frame header of the message
    if (!(header = zmsg_first (msg))) {
        error ("Frame header cannot be retrieved.");
        return -1;
    }

    // Convert the frame to a RouterMonitorHeader
    RouterMonitorHeader packetHeader = *((RouterMonitorHeader *) zframe_data (header));

    switch (packetHeader) {

        case ROUTER_MONITOR_ADD_FD: {
            zframe_t *fdFrame = zmsg_next (msg);

            // Get the socket file descriptor
            uint64_t fdClient = *((uint64_t *) (zframe_data (fdFrame)));

            // Check if this file descriptor is still used
            unsigned char fdClientKey [ROUTER_MONITOR_FDKEY_SIZE];
            RouterMonitor_genKey (fdClient, fdClientKey);

            zframe_t *clientFrame;
            if ((clientFrame = zhash_lookup (self->connected, fdClientKey)) == NULL) {
                // The client just sent its first message, add the identity frame to the hashtable
                clientFrame = zframe_dup (zmsg_next (msg));
                zhash_insert (self->connected, fdClientKey, clientFrame);
            } else {
                // Check if the fd <=> identity association is always the same
                if (!zframe_eq (clientFrame, zmsg_next (msg))) {
                    // Huho.
                    // TODO : Decide what to do
                    error ("==================================================================");
                    error ("The registred fd changed identity. That's really not a good thing.");
                    error ("==================================================================");
                    return -1;
                } else {
                    // It's alright, the client kept the same identity
                }
            }
        } break;

        default:
            warning ("Server subscriber received an unknown header : %x", packetHeader);
        break;
    }

    zmsg_destroy (&msg);

    return 0;
}


void
RouterMonitor_start (
    zsock_t *pipe,
    void *info
) {
    RouterMonitor self;
    RouterMonitor_init (&self, info);

    zactor_t *servermon;
    zsock_t *requests;
    zloop_t *reactor;

    // Connect to the Redis database
    if (!(Redis_connect (self.redis))) {
        error ("Cannot connect to Redis.");
        return;
    }

    // Connect to the MySQL database
    if (!(MySQL_connect (self.sql))) {
        error ("Cannot connect to MySQL.");
        return;
    }

    // Set up the Server Monitor Actor
    if (!(servermon = zactor_new (zmonitor, self.info.frontend))) {
        error ("Cannot allocate a new server monitor actor.");
        return;
    }
    zstr_sendx (servermon, "LISTEN", "CONNECT_DELAYED",  "CONNECT_RETRIED", "CLOSED", "CONNECTED", "ACCEPTED", "DISCONNECTED", NULL);
    zstr_sendx (servermon, "START", NULL);
    zsock_wait (servermon);

    // Set up the Server Monitor Subscriber that will receive request from the Router
    if (!(requests = zsock_new (ZMQ_SUB))) {
        error ("Cannot allocate a request subscriber socket.");
        return;
    }

    if (zsock_connect (requests, ROUTER_MONITOR_SUBSCRIBER_ENDPOINT, self.info.routerId) != 0) {
        error ("Failed to connect to the monitor subscriber endpoint %d.", self.info.routerId);
        return;
    }
    info ("Monitor Subscriber connected to %s", zsys_sprintf (ROUTER_MONITOR_SUBSCRIBER_ENDPOINT, self.info.routerId));
    zsock_set_subscribe (requests, "");

    // ====================================
    //   Prepare a reactor and fire it up
    // ====================================
    if (!(reactor = zloop_new ())) {
        error ("Cannot allocate a new reactor.");
        return;
    }

    // Attach a callback to frontend and backend sockets
    if (zloop_reader (reactor, (zsock_t *) servermon, RouterMonitor_monitor, &self) == -1
    ||  zloop_reader (reactor, requests, RouterMonitor_subscribe, &self) == -1
    ) {
        error ("Cannot register the sockets with the reactor.");
        return;
    }

    info ("Router Monitor is ready and running.");
    // Signal to the parent thread that the monitor is ready
    zsock_signal (pipe, 0);
    zframe_send ((zframe_t *[]) {zframe_new (PACKET_HEADER (ROUTER_MONITOR_READY), sizeof (ROUTER_MONITOR_READY))}, pipe, 0);

    if (zloop_start (reactor) != 0) {
        error ("An error occurred in the reactor.");
        return;
    }

    RouterMonitor_free (&self);
    zactor_destroy (&servermon);
    zsock_destroy (&requests);
    zloop_destroy (&reactor);
}

void
RouterMonitor_genKey (
    uint64_t fd,
    unsigned char fdKey[ROUTER_MONITOR_FDKEY_SIZE]
) {
    // Format the fdKey from the fd
    snprintf (fdKey, ROUTER_MONITOR_FDKEY_SIZE, "%llx", fd);
}

void
RouterMonitor_free (
    RouterMonitor *self
) {
    zhash_destroy (&self->connected);
}

void
RouterMonitor_destroy (
    RouterMonitor **_self
) {
    RouterMonitor *self = *_self;

    RouterMonitor_free (self);

    free (self);
    *_self = NULL;
}

void
RouterMonitorStartupInfo_destroy (
    RouterMonitorStartupInfo **_self
) {
    RouterMonitorStartupInfo *self = *_self;

    free (self);
    *_self = NULL;
}
