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
#include "Router.h"
#include "Worker.h"
#include "RouterMonitor.h"

// ------ Structure declaration -------
typedef struct {
    /** The frame of the client, or NULL if the client is not being processed */
    zframe_t *frame;
    /** The fd of the client */
    int fd;
} ClientIdentity;

typedef struct {
    /** The frame of the Worker */
    zframe_t *frame;
    /** The WorkerId */
    uint16_t id;
} WorkerIdentity;

typedef struct {
    /** Worker identity */
    WorkerIdentity identity;
    /** Client identity current treated */
    ClientIdentity curClientId;
} WorkerState;

/**
 * @brief Router is the routing component of the network.
 * It accepts packets on a specific port, and routes them to the workers.
 */
struct Router
{
    // === Network ===
    /** Router frontend socket. */
    zsock_t *frontend;

    /** Router backend socket. */
    zsock_t *backend;

    /** Workers state. */
    WorkerState *workers;

    /** Publisher to the Router Monitor */
    zsock_t *monitor;

    /** Subscriber to the Event Server */
    zsock_t *eventServer;

    /** List of workers entities. */
    zlist_t *readyWorkers;

    /** Count the number of workers that sent a READY signal. */
    int workersReadyCount;

    /** Index of the worker in the worker array that is going to take the charge if there is an overload */
    int overloadWorker;

    // === Startup information ===
    /** Router information */
    RouterStartupInfo info;

    // === Handlers ===
    /** Array of packet handlers */
    PacketHandler *packetHandlers;

    /** Packet handlers entries count */
    int packetHandlersCount;
};


// ------ Static declaration ------

/**
 * @brief Frontend ROUTER handler
 * @param loop The reactor handler
 * @param frontend The frontend socket
 * @param self The Router
 * @return 0 on success, -1 on error
 */
static int
Router_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *self
);

/**
 * @brief Backend ROUTER handler
 * @param loop The reactor handler
 * @param backend The backend socket
 * @param self The Router
 * @return 0 on success, -1 on error
 */
static int
Router_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *self
);

/**
 * @brief SUBSCRIBER handler
 * @param loop The reactor handler
 * @param subscribe The subscriber socket
 * @param self The Router
 * @return 0 on success, -1 on error
 */
static int
Router_subscribe (
    zloop_t *loop,
    zsock_t *subscribe,
    void *self
);

/**
 * @brief Initialize the frontend of the Router
 * @param self The Router
 * @return true on success, false otherwise
 */
static bool
Router_initFrontend (
    Router *self
);


/**
 * @brief Initialize the monitor of the Router
 * @param self The Router
 * @return true on success, false otherwise
 */
static bool
Router_initMonitor (
    Router *self
);

/**
 * @brief Initialize the backend of the Router
 * @param self The Router
 * @return true on success, false otherwise
 */
static bool
Router_initBackend (
    Router *self
);

// ------ Extern function implementation ------

Router *
Router_new (
    RouterStartupInfo *info
) {
    Router *self;

    if ((self = calloc (1, sizeof (Router))) == NULL) {
        return NULL;
    }

    if (!Router_init (self, info)) {
        Router_destroy (&self);
        error ("Router failed to initialize.");
        return NULL;
    }

    return self;
}

bool
Router_init (
    Router *self,
    RouterStartupInfo *info
) {
    // Get a private copy of the Router Information
    if (!(RouterStartupInfo_init (
            &self->info, info->routerId, info->ip,
            info->ports, info->portsCount, info->workersCount,
            &info->redisInfo, &info->sqlInfo))
    ) {
        error ("Cannot initialize router start up info.");
        return false;
    }

    // No Worker is ready at the startup
    self->workersReadyCount = 0;

    // By default, the workerID = 0 is going to take the charge when the Router is overloaded.
    self->overloadWorker = 0;


    // ==========================
    //   Allocate ZMQ objects
    // ==========================

    // The frontend listens to a BSD socket, not a ØMQ socket.
    if (!(self->frontend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate ROUTER frontend");
        return false;
    }

    // Backend listens to a ØMQ ROUTER socket.
    if (!(self->backend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate ROUTER backend");
        return false;
    }

    // Router Monitor listens to a ØMQ PUBLISHER socket.
    if (!(self->monitor = zsock_new (ZMQ_PUB))) {
        error ("Cannot allocate ROUTER backend");
        return false;
    }

    // EventServer publish messages to the Router for asynchronous messages.
    if (!(self->eventServer = zsock_new (ZMQ_SUB))) {
        error ("Cannot allocate ROUTER backend");
        return false;
    }

    // Allocate the workers entity list
    if (!(self->readyWorkers = zlist_new ())) {
        error ("Cannot allocate ready workers list.");
        return false;
    }

    // Allocate the workers array
    if ((self->workers = calloc (self->info.workersCount, sizeof (WorkerState))) == NULL) {
        error ("Cannot allocate the workers array.");
        return false;
    }

    return true;
}

bool
RouterStartupInfo_init (
    RouterStartupInfo *self,
    uint16_t routerId,
    char *ip,
    int *ports,
    int portsCount,
    int workersCount,
    RedisStartupInfo *redisInfo,
    MySQLStartupInfo *sqlInfo
) {
    self->routerId = routerId;
    if (!(self->ip = strdup (ip))) {
        error ("Cannot allocate ip.");
        return false;
    }

    if (!(self->ports = calloc (portsCount, sizeof (int)))) {
        error ("Cannot allocate ports array.");
        return false;
    }

    memcpy (self->ports, ports, sizeof (int) * portsCount);
    self->portsCount = portsCount;
    self->workersCount = workersCount;

    if (!(RedisStartupInfo_init (&self->redisInfo, redisInfo->hostname, redisInfo->port))) {
        error ("Cannot initialize Redis Start up info.");
        return false;
    }

    if (!(MySQLStartupInfo_init (&self->sqlInfo, sqlInfo->hostname, sqlInfo->login, sqlInfo->password, sqlInfo->database))) {
        error ("Cannot initialize MySQL start up info.");
        return false;
    }

    return true;
}

static int
Router_subscribe (
    zloop_t *loop,
    zsock_t *publisher,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *header;
    Router *self = (Router *) _self;

    // Receive the message from the publisher socket
    if (!(msg = zmsg_recv (publisher))) {
        // Interrupt
        return 0;
    }

    // Get the header frame of the message
    if (!(header = zmsg_pop (msg))) {
        error ("Frame header cannot be retrieved.");
        return -1;
    }

    // Convert the header frame to a RouterHeader
    RouterHeader packetHeader = *((RouterHeader *) zframe_data (header));
    zframe_destroy (&header);

    switch (packetHeader)
    {
        case ROUTER_WORKER_MULTICAST: {
            // The worker send a 'multicast' message : It is addressed to a group of destination clients.
            // [1 frame data] + [1 frame identity] + [1 frame identity] + ...
            zframe_t *dataFrame = zmsg_pop (msg);
            size_t identityCount = zmsg_size (msg);
            for (size_t count = 0; count < identityCount; count++) {
                // TODO : Don't allocate a new zmsg_t for every submessage ?
                zmsg_t *subMsg = zmsg_new ();
                zmsg_add (subMsg, zmsg_pop (msg));
                zmsg_add (subMsg, zframe_dup (dataFrame));
                zmsg_send (&subMsg, self->frontend);
            }
            zframe_destroy (&dataFrame);
        } break;

        default:
            warning ("Server subscriber received an unknown header : %x", packetHeader);
        break;
    }

    // Cleanup
    zmsg_destroy (&msg);

    return 0;
}

static int
Router_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *_self
) {
    int result = 0;
    zmsg_t *msg = NULL;
    zframe_t *workerStateFrame = NULL;
    zframe_t *header = NULL;
    Router *self = (Router *) _self;

    // Receive the message from the backend router
    if (!(msg = zmsg_recv (backend))) {
        // Interrupt
        result = 0;
        goto cleanup;
    }

    // Retrieve the workerStateFrame who sent the message
    if (!(workerStateFrame = zmsg_unwrap (msg))) {
        error ("Worker identity cannot be retrieved.");
        result = -1;
        goto cleanup;
    }

    // Get the header frame of the message
    if (!(header = zmsg_pop (msg))) {
        error ("Frame data cannot be retrieved.");
        result = -1;
        goto cleanup;
    }

    // Convert the header frame to a RouterHeader
    RouterHeader packetHeader = *((RouterHeader *) zframe_data (header));

    switch (packetHeader)
    {
        case ROUTER_WORKER_ERROR:
            // The worker sent an 'error' signal.
            // TODO : logging ?
            error ("Router received an error from a worker.");
            result = 0;
            goto cleanup;
        break;

        case ROUTER_WORKER_READY: {
            // The worker sent a 'ready' signal. Register it.
            zframe_t *workerIdFrame = zmsg_next (msg);
            uint16_t workerId = *((uint16_t *) zframe_data (workerIdFrame));
            self->workers [workerId].identity.frame = zframe_dup (workerStateFrame);
            self->workers [workerId].identity.id = workerId;
            self->workersReadyCount++;

            if (self->workersReadyCount == self->info.workersCount) {
                // All the workers are ready. Start the monitor
                if (!(Router_initMonitor (self))) {
                    error ("Cannot initialize the monitor.");
                    result = -1;
                    goto cleanup;
                }

                // Everything is ready from here. Open the frontend to the outside world !
                if (!(Router_initFrontend (self))) {
                    error ("Cannot initialize the frontend.");
                    result = -1;
                    goto cleanup;
                }

                info ("Router ID=%d is listening to clients from now.", self->info.routerId);
            }
        }
        break;

        case ROUTER_WORKER_NORMAL: {
            // The worker send a 'normal' message to the client
            if (zmsg_size (msg) == 2) {
                // Simple message : [1 frame identity] + [1 frame data]
                if (zmsg_send (&msg, self->frontend) != 0) {
                    error ("Cannot send message to the frontend.");
                    result = -1;
                    goto cleanup;
                }
            } else {
                // Multiple messages : [1 frame identity] + [1 frame data] + [1 frame data] + ... + [1 frame data]
                // Send N messages [1 frame identity] + [1 frame data]
                zframe_t *identity = zmsg_pop (msg);
                size_t msgCount = zmsg_size (msg);
                for (int i = 0; i < msgCount; i++) {
                    zmsg_t *subMsg = zmsg_new ();
                    zmsg_add (subMsg, zframe_dup (identity));
                    zmsg_add (subMsg, zmsg_pop (msg));
                    zmsg_send (&subMsg, self->frontend);
                }
                zframe_destroy (&identity);
            }
        } break;

        default :
            warning ("Router received an unknown header : %x", packetHeader);
        break;
    }

    // Get the WorkerState corresponding to the frame of the worker
    WorkerState *workerState = NULL;
    for (int i = 0; i < self->info.workersCount; i++) {
        if (zframe_eq (workerStateFrame, self->workers[i].identity.frame)) {
            workerState = &self->workers[i];
        }
    }
    if (!workerState) {
        error ("Cannot find the Worker in the Worker list.");
        result = -1;
        goto cleanup;
    }

    // Remove the responsability of the client for the current worker
    zframe_destroy (&self->workers [workerState->identity.id].curClientId.frame);

    // The worker finished its job; add it at the end of the list (round robin load balancing)
    zlist_append (self->readyWorkers, workerState);

cleanup:
    zmsg_destroy (&msg);
    zframe_destroy (&workerStateFrame);
    zframe_destroy (&header);

    return result;
}

static bool
Router_informMonitor (
    Router *self,
    zframe_t *identityClient,
    uint64_t fdClient
) {
    // TODO : Don't check the FD everytime, keep a connected hashtable in the router ?

    // Build the message to the Router Monitor
    zmsg_t *msg;
    if ((msg = zmsg_new ()) == NULL
    ||  zmsg_addmem (msg, PACKET_HEADER (ROUTER_MONITOR_ADD_FD), sizeof (ROUTER_MONITOR_ADD_FD)) != 0
    ||  zmsg_addmem (msg, PACKET_HEADER (fdClient), sizeof (fdClient)) != 0
    ||  zmsg_append (msg, &identityClient) != 0
    ||  zmsg_send   (&msg, self->monitor)
    ) {
        error ("Cannot send a " STRINGIFY (ROUTER_MONITOR_ADD_FD) " packet to the router monitor.");
        return false;
    }

    return true;
}

static int
Router_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *_self
) {
    zmsg_t *msg;
    WorkerState *workerState = NULL;
    zframe_t *workerStateDest = NULL;
    Router *self = (Router *) _self;

    if (!(msg = zmsg_recv (frontend))) {
        // Interrupt
        return 0;
    }

    zframe_t *identityClient = zmsg_first (msg);
    zframe_t *data = zmsg_next (msg);

    // Retrieve the FD of the client
    zframe_t *identityClientDup = zframe_dup (identityClient);
    uint8_t *identity = zframe_data (identityClientDup);
    zmq_getsockopt (zsock_resolve (frontend), ZMQ_IDENTITY_FD, identity, (size_t[]) {5});
    uint64_t fdClient = *((uint64_t *) identity);
    zframe_destroy (&identityClientDup);

    // Don't process the packet if it is empty, or if an invalid fd is used
    if (zframe_size (data) == 0 || fdClient == -1) {
        zmsg_destroy (&msg);
        return 0;
    }

    // Inform the RouterMonitor that the client sent a request
    if (!(Router_informMonitor (self, zframe_dup (identityClient), fdClient))) {
        error ("Cannot inform the Router Monitor.");
        return 0;
    }

    // Check if the client is not currently processed by another Worker
    for (int i = 0; i < self->info.workersCount; i++) {
        if ((self->workers[i].curClientId.frame != NULL)
        &&  (zframe_eq (identityClient, self->workers[i].curClientId.frame))
        ) {
            // Already processed by this worker : Keep the responsibility to this worker so we
            // don't break the protocol by processing a packet before another one
            workerStateDest = zframe_dup (self->workers[i].identity.frame);
        }
    }

    // Retrieve a workerState if not already done (round robin)
    if (!workerStateDest) {
        uint16_t workerIdInCharge;

        if (!(workerState = (WorkerState *) zlist_pop (self->readyWorkers))) {
            // All Workers seem to be busy.
            warning ("All Workers seem to be busy. Transfer the request to the overload worker.");

            if (self->workersReadyCount == 0) {
                error ("No worker has been registered yet.");
                return 0;
            }

            workerStateDest = zframe_dup (self->workers[self->overloadWorker].identity.frame);
            workerIdInCharge = self->overloadWorker;

            // We don't want only one Worker takes the overload charge, do a round robin here too
            self->overloadWorker = (self->overloadWorker + 1) % self->workersReadyCount;
        } else {
            workerStateDest = zframe_dup (workerState->identity.frame);
            workerIdInCharge = workerState->identity.id;
        }

        // Replace the new identity
        if (self->workers[workerIdInCharge].curClientId.frame) {
            zframe_destroy (&self->workers[workerIdInCharge].curClientId.frame);
        }
        self->workers[workerIdInCharge].curClientId.frame = zframe_dup (identityClient);
    }

    // Wrap the worker's identity which receives the message
    zmsg_wrap (msg, workerStateDest);

    // Forward message to backend
    if (zmsg_send (&msg, self->backend) != 0) {
        error ("Frontend cannot send message to the backend");
        return -1;
    }

    return 0;
}

static bool
Router_initMonitor (
    Router *self
) {
    // ===================================
    //     Initialize Router Monitor
    // ===================================
    if (zsock_bind (self->monitor, ROUTER_MONITOR_SUBSCRIBER_ENDPOINT, self->info.routerId) == -1) {
        error ("Cannot bind to the Router Monitor endpoint");
        return false;
    }
    info ("Binded to the Router Monitor endpoint %s", zsys_sprintf (ROUTER_MONITOR_SUBSCRIBER_ENDPOINT, self->info.routerId));

    RouterMonitorStartupInfo *routerMonitorInfo;
    if (!(routerMonitorInfo = RouterMonitorStartupInfo_new (self->frontend, self->info.routerId, &self->info.redisInfo, &self->info.sqlInfo))) {
        error ("Cannot allocate a new Router Monitor Info.");
        return false;
    }

    zactor_t *monitorActor;
    if ((monitorActor = zactor_new (RouterMonitor_start, routerMonitorInfo)) == NULL) {
        error ("Cannot create a new thread for the Router Monitor.");
        return false;
    }

    // Wait for the READY signal from the monitor actor
    zmsg_t *msg;
    if ((!(msg = zmsg_recv (monitorActor)))
    || (memcmp (zframe_data (zmsg_first (msg)), PACKET_HEADER (ROUTER_MONITOR_READY), sizeof (ROUTER_MONITOR_READY)) != 0)
    ) {
        error ("Cannot received correctly an answer from the monitor actor.");
        return false;
    }

    zmsg_destroy (&msg);

    return true;
}

static bool
Router_initFrontend (
    Router *self
) {
    // ===================================
    //        Initialize frontend
    // ===================================
    zsock_set_router_raw (self->frontend, 1);

    // Bind the endpoints for the ROUTER frontend
    for (int i = 0; i < self->info.portsCount; i++) {
        if (zsock_bind (self->frontend, ROUTER_FRONTEND_ENDPOINT, self->info.ip, self->info.ports[i]) == -1) {
            error ("Failed to bind Server frontend to the endpoint : %s:%d.", self->info.ip, self->info.ports[i]);
            return false;
        }
        info ("Frontend listening on port %d.", self->info.ports[i]);
    }

    return true;
}

static bool
Router_initBackend (
    Router *self
) {
    // ===================================
    //       Initialize backend
    // ===================================
    // Create and connect a socket to the backend
    if (zsock_bind (self->backend, ROUTER_BACKEND_ENDPOINT, self->info.routerId) == -1) {
        error ("Failed to bind Server ROUTER backend.");
        return false;
    }
    info ("Backend listening on %s.", zsys_sprintf (ROUTER_BACKEND_ENDPOINT, self->info.routerId));

    return true;
}

bool
Router_initEventServerSubscriber (
    Router *self
) {
    // ===================================
    //       Initialize subscriber
    // ===================================
    if (zsock_connect (self->eventServer, ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId) != 0) {
        error ("Failed to connect to the eventServer subscriber endpoint %s.", zsys_sprintf (ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId));
        return false;
    }

    // Subscribe for all messages
    zsock_set_subscribe (self->eventServer, "");

    return true;
}

bool
Router_start (
    Router *self
) {
    zloop_t *reactor;

    // Initialize the backend
    if (!(Router_initBackend (self))) {
        error ("Cannot initialize the backend.");
        return false;
    }

    // Initialize the subscriber
    if (!(Router_initEventServerSubscriber (self))) {
        error ("Cannot initialize the subscriber.");
        return false;
    }

    // ====================================
    //   Prepare a reactor and fire it up
    // ====================================
    if (!(reactor = zloop_new ())) {
        error ("Cannot allocate a new reactor.");
        return false;
    }

    // Attach a callback to frontend and backend sockets
    if (zloop_reader (reactor, self->backend,  Router_backend,  self) == -1
    ||  zloop_reader (reactor, self->frontend, Router_frontend, self) == -1
    ) {
        error ("Cannot register the sockets with the reactor.");
        return false;
    }

    // Attach a callback to subscribers sockets
    if (zloop_reader (reactor, self->eventServer, Router_subscribe, self) == -1) {
        error ("Cannot register the subscriber to the reactor.");
        return false;
    }

    info ("Router is ready and running.");
    if (zloop_start (reactor) != 0) {
        error ("An error occurred in the reactor.");
        return false;
    }

    zloop_destroy (&reactor);

    return true;
}

int
Router_getId (
    Router *self
) {
    return self->info.routerId;
}

void
RouterStartupInfo_free (
    RouterStartupInfo *self
) {
    free (self->ip);
    free (self->ports);
}

void
Router_destroy (
    Router **_self
) {
    Router *self = *_self;

    if (self->frontend) {
        zsock_destroy (&self->frontend);
    }

    if (self->backend) {
        zsock_destroy (&self->backend);
    }

    zsock_destroy (&self->eventServer);

    if (self->workers) {
        free (self->workers);
    }

    if (self->readyWorkers) {
        zlist_destroy (&self->readyWorkers);
    }

    RouterStartupInfo_free (&self->info);

    free (self);
    *_self = NULL;
}
