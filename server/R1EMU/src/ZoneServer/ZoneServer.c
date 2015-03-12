/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ZoneServer.h
 * @brief This module stores all the zone of the clients
 *
 * ZoneServer detains the hashtable of all the zones in a given system.
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "ZoneServer.h"
#include "ZoneWorker/ZoneWorker.h"


// ------ Structure declaration -------
/**
 * @brief ZoneServer is the representation of the zone server system
 */
struct ZoneServer
{
    /** Zone server ID */
    int zoneServerId;

    /** Zone server frontend socket. Listens to ports exposed to the clients */
    zsock_t *frontend;

    /** Zone server backend socket. Listens to "zoneWorkers" endpoint */
    zsock_t *backend;

    /** List of workers entities */
    zlist_t *readyWorkers;

    /** Identity frames of the workers. */
    zframe_t **workers;

    /** Count the number of workers registered in the array */
    int workersRegistredCount;

    /** Index of the worker in the worker array that is going to take the charge if there is an overload */
    int overloadWorker;

    // ----- Configuration -----
    /** Public ports exposed to the clients */
    int frontendPort;

    /** Number of workers allocated for the backend */
    int workersCount;

    /** Port of the session server */
    int sessionServerFrontendPort;
};


// ------ Static declaration ------

/**
 * @brief Frontend ROUTER handler of the Zone Server
 * @param loop The reactor handler
 * @param frontend The frontend socket
 * @param self The zoneServer
 * @return 0 on success, -1 on error
 */
static int
ZoneServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *self
);

/**
 * @brief Backend ROUTER handler of the Zone Server
 * @param loop The reactor handler
 * @param backend The backend socket
 * @param self The zoneServer
 * @return 0 on success, -1 on error
 */
static int
ZoneServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *self
);


// ------ Extern function implementation ------

ZoneServer *
ZoneServer_new (
    int zoneServerId,
    int frontendPort,
    int workersCount,
    int sessionServerFrontendPort
) {
    ZoneServer *self;

    if ((self = calloc (1, sizeof (ZoneServer))) == NULL) {
        return NULL;
    }

    if (!ZoneServer_init (self, zoneServerId, frontendPort, workersCount, sessionServerFrontendPort)) {
        ZoneServer_destroy (&self);
        error ("ZoneServer failed to initialize.");
        return NULL;
    }

    return self;
}


bool
ZoneServer_init (
    ZoneServer *self,
    int zoneServerId,
    int frontendPort,
    int workersCount,
    int sessionServerFrontendPort
) {
    self->zoneServerId = zoneServerId;
    self->workersCount = workersCount;
    self->frontendPort = frontendPort;
    self->sessionServerFrontendPort = sessionServerFrontendPort;

    // ==========================
    //   Allocate ZMQ objects
    // ==========================

    // The frontend listens to a RAW socket, not a 0MQ socket.
    if (!(self->frontend = zsock_new (ZMQ_RAW_ROUTER))) {
        error ("Cannot allocate Zone Server ROUTER frontend");
        return false;
    }

    // Backend listens to a 0MQ socket.
    if (!(self->backend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate Zone Server ROUTER backend");
        return false;
    }

    // Allocate the workers entity list
    if (!(self->readyWorkers = zlist_new ())) {
        error ("Cannot allocate ready workers list.");
        return false;
    }

    // ==========================================================
    //   The configuration file must be read entirely from here
    // ==========================================================

    // Allocate the workers array
    if ((self->workers = malloc (sizeof (zframe_t *) * self->workersCount)) == NULL) {
        error ("Cannot allocate the workers array.");
        return false;
    }

    // The workers will be registered in the backend once they are ready
    self->workersRegistredCount = 0;

    // By default, the first worker is going to take the charge
    self->overloadWorker = 0;

    return true;
}


static int
ZoneServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    zframe_t *packet;
    ZoneServer *self = (ZoneServer *) _self;

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

    // Register the identity of the workers if it hasn't been registered yet
    if (self->workersRegistredCount < self->workersCount) {
        // Check if it hasn't been registered already
        bool isAlreadyRegistred = false;
        for (int i = 0; i < self->workersRegistredCount; i++) {
            if (zframe_eq (self->workers[i], workerIdentity)) {
                isAlreadyRegistred = true;
                break;
            }
        }
        if (!isAlreadyRegistred) {
            self->workers [self->workersRegistredCount] = zframe_dup (workerIdentity);
            self->workersRegistredCount++;
        }
    }

    // The worker finished its work; add it at the end of the list (round robin load balancing)
    zlist_append (self->readyWorkers, workerIdentity);

    // Get the content of the message
    if (!(packet = zmsg_first (msg))) {
        error ("Frame data cannot be retrieved.");
        return -1;
    }

    // Forward the message to the frontend if it's not a READY signal
    if (memcmp (zframe_data (packet), PACKET_HEADER (ZONE_SERVER_WORKER_READY), sizeof(ZONE_SERVER_WORKER_READY)) == 0) {
        zmsg_destroy (&msg);
    }
    else {
        if (zmsg_send (&msg, self->frontend) != 0) {
            error ("Cannot send message to the frontend.");
            return -1;
        }
    }

	return 0;
}


static int
ZoneServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    ZoneServer *self = (ZoneServer *) _self;

    // Receive the message from the frontend router
    if (!(msg = zmsg_recv (frontend))) {
        // Interrupt
        return 0;
    }

    dbg ("\n === ZONE RECV ===");
    zmsg_print (msg);

    // Retrieve a workerIdentity (round robin)
    if (!(workerIdentity = (zframe_t *) zlist_pop (self->readyWorkers))) {
        // All Zone Workers seem to be busy.
        warning ("All Zone Workers seem to be busy. Transfer the request to the overload worker.");
        // Transfer the request to the overload worker
        zframe_destroy (&workerIdentity);

        if (self->workersRegistredCount == 0) {
            error ("No worker has been registered yet.");
            return 0;
        }

        workerIdentity = zframe_dup (self->workers[self->overloadWorker]);
        self->overloadWorker = (self->overloadWorker + 1) % self->workersRegistredCount;
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

void *
ZoneServer_start (
    void *args
) {
    zloop_t *reactor;
    ZoneWorker * zoneWorker;
    ZoneServer *self = (ZoneServer *) args;

    // ===================================
    //       Initialize backend
    // ===================================

    if (zsock_bind (self->backend, ZONE_SERVER_BACKEND_ENDPOINT, self->frontendPort) == -1) {
        error ("Failed to bind Zone Server ROUTER backend.");
        return NULL;
    }
    info ("[%d] Backend listening on %s.", self->zoneServerId, zsys_sprintf (ZONE_SERVER_BACKEND_ENDPOINT, self->frontendPort));

    // Initialize workers - Start N worker threads.
    for (int workerId = 0; workerId < self->workersCount; workerId++) {
        if ((zoneWorker = ZoneWorker_new (workerId, self->zoneServerId, self->frontendPort, self->sessionServerFrontendPort))) {
            if (zthread_new (ZoneWorker_worker, zoneWorker) != 0) {
                error ("Cannot create Zone Server worker thread ID %d.", workerId);
                return NULL;
            }
        } else {
            error ("Cannot allocate a new zoneWorker");
            return NULL;
        }
    }

    // ===================================
    //        Initialize frontend
    // ===================================

    // Fill the client specifications :
    // - It connects to zone server port given in BC_START_GAMEOK
    // - It communicates with the zone server through a RAW socket.
    // Set the ROUTER as a RAW socket.
    zsock_set_router_raw (self->frontend, true);

    // Bind the endpoint for the ROUTER frontend
    if (zsock_bind (self->frontend, ZONE_SERVER_FRONTEND_ENDPOINT, self->frontendPort) == -1) {
        error ("Failed to bind Zone Server ROUTER frontend to the port %d.", self->frontendPort);
        return NULL;
    }
    info ("[%d] Frontend listening on port %d.", self->zoneServerId, self->frontendPort);

    // ====================================
    //   Prepare a reactor and fire it up
    // ====================================

    if (!(reactor = zloop_new ())) {
        error ("Cannot allocate a new reactor.");
        return NULL;
    }

	if (zloop_reader (reactor, self->backend,  ZoneServer_backend,  self) == -1
    ||  zloop_reader (reactor, self->frontend, ZoneServer_frontend, self) == -1
    ) {
        error ("Cannot register the sockets with the reactor.");
        return NULL;
    }

    info ("[%d] ZoneServer is ready and running.", self->zoneServerId);
    if (zloop_start (reactor) != 0) {
        error ("An error occurred in the reactor.");
        return NULL;
    }

    zloop_destroy (&reactor);

    return NULL;
}


void
ZoneServer_destroy (
    ZoneServer **_self
) {
    ZoneServer *self = *_self;

    if (self->frontend) {
        zsock_destroy (&self->frontend);
    }

    if (self->backend) {
        zsock_destroy (&self->backend);
    }

    if (self->workers) {
        free (self->workers);
    }

    if (self->readyWorkers) {
        zlist_destroy (&self->readyWorkers);
    }

    free (self);
    *_self = NULL;
}
