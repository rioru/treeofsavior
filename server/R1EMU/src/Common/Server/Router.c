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


// ------ Structure declaration -------
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

    /** Subscribers connection to the workers.
     * It receives asynchronous messages to send to clients. */
    zsock_t **subscribers;

    /** List of workers entities. */
    zlist_t *readyWorkers;

    /** Identity frames of the workers. */
    zframe_t **workers;

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
    memcpy (&self->info, info, sizeof (self->info));

    // No Worker is ready at the startup
    self->workersReadyCount = 0;

    // By default, the workerID = 0 is going to take the charge when the Router is overloaded.
    self->overloadWorker = 0;


    // ==========================
    //   Allocate ZMQ objects
    // ==========================

    // The frontend listens to a BSD socket, not a ØMQ socket.
    if (!(self->frontend = zsock_new (ZMQ_STREAM))) {
        error ("Cannot allocate ROUTER frontend");
        return false;
    }

    // Backend listens to a 0MQ socket.
    if (!(self->backend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate ROUTER backend");
        return false;
    }

    // The Router can communicate asynchronously with the workers with a pub/sub socket.
    self->subscribers = calloc (1, sizeof (zsock_t *) * self->info.workersCount);
    for (int workerId = 0; workerId < self->info.workersCount; workerId++) {
        if (!(self->subscribers[workerId] = zsock_new (ZMQ_SUB))) {
            error ("Cannot allocate a Server SUBSCRIBER");
            return false;
        }
    }

    // Allocate the workers entity list
    if (!(self->readyWorkers = zlist_new ())) {
        error ("Cannot allocate ready workers list.");
        return false;
    }

    // Allocate the workers array
    if ((self->workers = malloc (sizeof (zframe_t *) * self->info.workersCount)) == NULL) {
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
    int workersCount
) {
    self->routerId = routerId;
    self->ip = strdup(ip);
    self->ports = ports;
    self->portsCount = portsCount;
    self->workersCount = workersCount;

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

    // Get the header of the message
    if (!(header = zmsg_pop (msg))) {
        error ("Frame header cannot be retrieved.");
        return -1;
    }

    RouterHeader packetHeader = *((RouterHeader *) zframe_data (header));
    zframe_destroy (&header);

    switch (packetHeader) {

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

    return 0;
}

static int
Router_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    zframe_t *header;
    Router *self = (Router *) _self;

    // Receive the message from the backend router
    if (!(msg = zmsg_recv (backend))) {
        // Interrupt
        return 0;
    }

    // Retrieve the workerIdentity who sent the message
    if (!(workerIdentity = zmsg_unwrap (msg))) {
        error ("Worker identity cannot be retrieved.");
        return -1;
    }

    // The worker finished its job; add it at the end of the list (round robin load balancing)
    zlist_append (self->readyWorkers, workerIdentity);

    // Get the header the message
    if (!(header = zmsg_pop (msg))) {
        error ("Frame data cannot be retrieved.");
        return -1;
    }

    RouterHeader packetHeader = *((RouterHeader *) zframe_data (header));
    zframe_destroy (&header);

    switch (packetHeader) {
        case ROUTER_WORKER_ERROR:
            // The worker sent an 'error' signal.
            // TODO : logging ?
            error ("Router received an error from a worker.");
            zmsg_destroy (&msg);
        break;

        case ROUTER_WORKER_READY:
            // The worker sent a 'ready' signal. Register it.
            self->workers [self->workersReadyCount++] = zframe_dup (workerIdentity);
            zmsg_destroy (&msg);

            if (self->workersReadyCount == self->info.workersCount) {
                // All the workers are ready. Open the frontend to the outside world !
                if (!(Router_initFrontend (self))) {
                    error ("Cannot initialize the frontend.");
                    return -1;
                }

                info ("Router ID=%d is ready and running.", self->info.routerId);
            }
        break;

        case ROUTER_WORKER_NORMAL: {
            // The worker send a 'normal' message to the client
            if (zmsg_size (msg) == 2) {
                // Simple message : [1 frame identity] + [1 frame data]
                if (zmsg_send (&msg, self->frontend) != 0) {
                    error ("Cannot send message to the frontend.");
                    return -1;
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

    return 0;
}


static int
Router_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    Router *self = (Router *) _self;

    // Receive the message from the frontend router
    if (!(msg = zmsg_recv (frontend))) {
        // Interrupt
        return 0;
    }

    // Retrieve a workerIdentity (round robin)
    if (!(workerIdentity = (zframe_t *) zlist_pop (self->readyWorkers))) {
        // All Workers seem to be busy.
        warning ("All Workers seem to be busy. Transfer the request to the overload worker.");
        // Transfer the request to the overload worker
        zframe_destroy (&workerIdentity);

        if (self->workersReadyCount == 0) {
            error ("No worker has been registered yet.");
            return 0;
        }

        workerIdentity = zframe_dup (self->workers[self->overloadWorker]);

        // We don't want only one Worker takes the overload charge, do a round robin here too
        self->overloadWorker = (self->overloadWorker + 1) % self->workersReadyCount;
    }

    // Wrap the worker's identity which receives the message
    zmsg_wrap (msg, workerIdentity);

    // Forward message to backend
    if (zmsg_send (&msg, self->backend) != 0) {
        error ("Frontend cannot send message to the backend");
        return -1;
    }

    return 0;
}

static bool
Router_initFrontend (
    Router *self
) {
    // ===================================
    //        Initialize frontend
    // ===================================

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
Router_initSubscribers (
    Router *self
) {
    // ===================================
    //       Initialize subscribers
    // ===================================
    for (int workerId = 0; workerId < self->info.workersCount; workerId++) {
        if (zsock_connect (self->subscribers[workerId], ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId) != 0) {
            error ("Failed to connect to the subscriber endpoint %d.", workerId);
            return false;
        }
        info ("Subscriber connected to %s", zsys_sprintf (ROUTER_SUBSCRIBER_ENDPOINT, self->info.routerId, workerId));
        // Subscribe to all messages, without any filter
        zsock_set_subscribe (self->subscribers[workerId], "");
    }

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

    // Initialize the subscribers
    if (!(Router_initSubscribers (self))) {
        error ("Cannot initialize the subscribers.");
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
    for (int workerId = 0; workerId < self->info.workersCount; workerId++) {
        if (zloop_reader (reactor, self->subscribers[workerId], Router_subscribe, self) == -1) {
            error ("Cannot register the subscribers with the reactor.");
            return false;
        }
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

    for (int i = 0; i < self->info.workersCount; i++) {
        zsock_destroy (&self->subscribers[i]);
    }

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
