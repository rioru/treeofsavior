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
#include "BarrackServer.h"
#include "BarrackWorker/BarrackWorker.h"


// ------ Structure declaration -------
/**
 * @brief BarrackServer is the representation of the barrack server system
 *
 * It accepts login requests from clients and manage the connection through the barrack phase.
 *
 */
struct BarrackServer
{
    /** Barrack server frontend socket. Listens to ports exposed to the clients */
    zsock_t *frontend;

    /** Barrack server backend socket. Listens to "barrackWorkers" endpoint */
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
    /** The IP of the server */
    char *serverIp;

    /** Count of public ports exposed to the clients */
    int publicPortsCount;

    /** Public ports exposed to the clients */
    int *publicPorts;

    /** Number of workers allocated for the backend */
    int workersCount;

    /** Configuration file path */
    char *confFilePath;
};


// ------ Static declaration ------

/**
 * @brief Frontend ROUTER handler of the Barrack Server
 * @param loop The reactor handler
 * @param frontend The frontend socket
 * @param self The barrackServer
 * @return 0 on success, -1 on error
 */
static int
BarrackServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *self
);

/**
 * @brief Backend ROUTER handler of the Barrack Server
 * @param loop The reactor handler
 * @param backend The backend socket
 * @param self The barrackServer
 * @return 0 on success, -1 on error
 */
static int
BarrackServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *self
);


// ------ Extern function implementation ------

BarrackServer *
BarrackServer_new (
    char *confFilePath
) {
    BarrackServer *self;

    if ((self = calloc (1, sizeof (BarrackServer))) == NULL) {
        return NULL;
    }

    if (!BarrackServer_init (self, confFilePath)) {
        BarrackServer_destroy (&self);
        error ("BarrackServer failed to initialize.");
        return NULL;
    }

    return self;
}


bool
BarrackServer_init (
    BarrackServer *self,
    char *confFilePath
) {
    zconfig_t *conf;
    char *portsArray;

    self->confFilePath = confFilePath;

    // ==================================
    //     Read the configuration file
    // ==================================

    // Open the configuration file
    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the barrack configuration file (%s).", confFilePath);
        return false;
    }

    // Read the ports array
    if (!(portsArray = zconfig_resolve (conf, "barrackServer/portsArray", NULL))) {
        warning ("Ports cannot be read for Barrack Server. Defaults ports have been used : %s", BARRACK_SERVER_PORTS_DEFAULT);
        portsArray = BARRACK_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    char *port = strtok (portsArray, " ");
    while (port != NULL) {
        self->publicPortsCount++;
        port = strtok (NULL, " ");
    }

    // Fill the server ports array
    self->publicPorts = calloc (self->publicPortsCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->publicPortsCount; portIndex++) {
        self->publicPorts[portIndex] = strtol (portsArray, &portsArray, 10);
        portsArray++;
    }

    // Read the number of barrack server workers
    if (!(self->workersCount = atoi (zconfig_resolve (conf, "barrackServer/workersCount", NULL)))) {
        warning ("Cannot read correctly the barrack workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", BARRACK_SERVER_WORKERS_COUNT_DEFAULT);
        self->workersCount = BARRACK_SERVER_WORKERS_COUNT_DEFAULT;
    }

    // Read the server interface IP
    if (!(self->serverIp = strdup (zconfig_resolve (conf, "barrackServer/serverIP", NULL)))) {
        warning ("Cannot read correctly the barrack interface IP in the configuration file (%s). ", confFilePath);
        warning ("The default IP = %s has been used.", BARRACK_SERVER_FRONTEND_IP_DEFAULT);
        self->serverIp = BARRACK_SERVER_FRONTEND_IP_DEFAULT;
    }

    // Close the configuration file
    zconfig_destroy (&conf);

    // ==========================
    //   Allocate ZMQ objects
    // ==========================

    // The frontend listens to a BSD socket, not a 0MQ socket.
    if (!(self->frontend = zsock_new (ZMQ_STREAM))) {
        error ("Cannot allocate Barrack Server ROUTER frontend");
        return false;
    }

    // Backend listens to a 0MQ socket.
    if (!(self->backend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate Barrack Server ROUTER backend");
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
BarrackServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    zframe_t *header;
    BarrackServer *self = (BarrackServer *) _self;

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

    // Get the header the message
    if (!(header = zmsg_pop (msg))) {
        error ("Frame data cannot be retrieved.");
        return -1;
    }

    BarrackServerHeader packetHeader = *((BarrackServerHeader *) zframe_data (header));
    zframe_destroy (&header);

    switch (packetHeader) {
        case BARRACK_SERVER_WORKER_ERROR:
            // The worker sent an 'error' signal.
            // TODO : logging ?
            zmsg_destroy (&msg);
        break;

        case BARRACK_SERVER_WORKER_READY:
            // The worker sent a 'ready' signal. Nothing much to do.
            zmsg_destroy (&msg);
        break;

        case BARRACK_SERVER_WORKER_NORMAL: {
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
            warning ("Zone Server received an unknown header : %x", packetHeader);
        break;
    }

    return 0;
}


static int
BarrackServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    BarrackServer *self = (BarrackServer *) _self;

    // Receive the message from the frontend router
    if (!(msg = zmsg_recv (frontend))) {
        // Interrupt
        return 0;
    }

    // Retrieve a workerIdentity (round robin)
    if (!(workerIdentity = (zframe_t *) zlist_pop (self->readyWorkers))) {
        // All Barrack Workers seem to be busy.
        warning ("All Barrack Workers seem to be busy. Transfer the request to the overload worker.");
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

bool
BarrackServer_start (
    BarrackServer *self
) {
    zloop_t *reactor;
    BarrackWorker * barrackWorker;

    // =============================================
    //  Launch the dedicated barrack session server
    // =============================================
    SessionServer *sessionServer = SessionServer_new (BARRACK_SERVER_ZONE_ID, self->confFilePath);
    if (zthread_new ((zthread_detached_fn *) SessionServer_start, sessionServer) == -1) {
        error ("Cannot launch session server thread.");
        return false;
    }

    // ===================================
    //       Initialize backend
    // ===================================
    // Create and connect a socket to the backend
    if (zsock_bind (self->backend, BARRACK_SERVER_BACKEND_ENDPOINT) == -1) {
        error ("Failed to bind Barrack Server ROUTER backend.");
        return false;
    }
    info ("Backend listening on %s.", BARRACK_SERVER_BACKEND_ENDPOINT);

    // Initialize workers - Start N worker threads.
    for (int workerId = 0; workerId < self->workersCount; workerId++) {
        if ((barrackWorker = BarrackWorker_new (workerId))) {
            if (zthread_new (BarrackWorker_worker, barrackWorker) != 0) {
                error ("Cannot create Barrack Server worker thread ID %d.", workerId);
                return false;
            }
        } else {
            error ("Cannot allocate a new barrackWorker");
            return false;
        }
    }

    // ===================================
    //        Initialize frontend
    // ===================================

    // Fill the client specifications :
    // - It connects to port 2000 and 2001 by default (configurable in serverlist_recent.xml)
    // - It communicates with the server through a BSD socket.

    // Bind the endpoints for the ROUTER frontend
    for (int i = 0; i < self->publicPortsCount; i++) {
        if (zsock_bind (self->frontend, BARRACK_SERVER_FRONTEND_ENDPOINT, self->serverIp, self->publicPorts[i]) == -1) {
            error ("Failed to bind Barrack Server frontend to the endpoint : %s:%d.", self->serverIp, self->publicPorts[i]);
            return false;
        }
        info ("Frontend listening on port %d.", self->publicPorts[i]);
    }

    // ====================================
    //   Prepare a reactor and fire it up
    // ====================================
    if (!(reactor = zloop_new ())) {
        error ("Cannot allocate a new reactor.");
        return false;
    }

    if (zloop_reader (reactor, self->backend,  BarrackServer_backend,  self) == -1
    ||  zloop_reader (reactor, self->frontend, BarrackServer_frontend, self) == -1
    ) {
        error ("Cannot register the sockets with the reactor.");
        return false;
    }

    info ("BarrackServer is ready and running.");
    if (zloop_start (reactor) != 0) {
        error ("An error occurred in the reactor.");
        return false;
    }

    zloop_destroy (&reactor);

    return true;
}


void
BarrackServer_destroy (
    BarrackServer **_self
) {
    BarrackServer *self = *_self;

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
