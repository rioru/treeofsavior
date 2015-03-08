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

    // ----- Configuration -----
    /** Public ports exposed to the clients */
    int publicPorts[2];

    /** Number of workers allocated for the backend */
    int workersCount;

    /** Port of the session server */
    int sessionServerFrontendPort;
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


// ------ Extern declaration ------

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
    const char *confFilePath
) {
    zconfig_t *conf;

    // ==================================
    //     Read the configuration file
    // ==================================

    // Open the configuration file
    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the barrack configuration file (%s).", confFilePath);
        return false;
    }

    // Read the ports
    if (!(self->publicPorts[0] = atoi (zconfig_resolve (conf, "barrackServer/port1", STRINGIFY (BARRACK_SERVER_PORT1_DEFAULT))))
    ||  !(self->publicPorts[1] = atoi (zconfig_resolve (conf, "barrackServer/port2", STRINGIFY (BARRACK_SERVER_PORT2_DEFAULT))))
    ) {
        warning ("Cannot read correctly the barrack server ports in the configuration file (%s). ", confFilePath);

        if (!self->publicPorts[0]) {
            warning ("The default port = %d has been used.", BARRACK_SERVER_PORT1_DEFAULT);
            self->publicPorts[0] = BARRACK_SERVER_PORT1_DEFAULT;
        }

        if (!self->publicPorts[1]) {
            warning ("The default port = %d has been used.", BARRACK_SERVER_PORT2_DEFAULT);
            self->publicPorts[1] = BARRACK_SERVER_PORT2_DEFAULT;
        }
    }

    // Read the Session Server frontend port
    if (!(self->sessionServerFrontendPort = atoi (zconfig_resolve (conf, "sessionServer/port", STRINGIFY (SESSION_SERVER_PORT_DEFAULT))))
    ) {
        warning ("Cannot read correctly the session server port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", SESSION_SERVER_PORT_DEFAULT);
    }

    // Read the number of barrack server workers
    if (!(self->workersCount = atoi (zconfig_resolve (conf, "barrackServer/workers_count", STRINGIFY (BARRACK_SERVER_WORKERS_COUNT_DEFAULT))))) {
        warning ("Cannot read correctly the barrack workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", BARRACK_SERVER_WORKERS_COUNT_DEFAULT);
    }

    // Close the configuration file
    zconfig_destroy (&conf);

    // ==========================
    //   Allocate ZMQ objects
    // ==========================

    // The frontend listens to a RAW socket, not a 0MQ socket.
    if (!(self->frontend = zsock_new (ZMQ_RAW_ROUTER))) {
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
    zframe_t *packet;
    BarrackServer *self = (BarrackServer *) _self;

    // Receive the message from the backend router
    if (!(msg = zmsg_recv (backend))) {
        // Interrupt
        return 0;
    }

    // Retrieve the workerIdentity who sent the message
    if (!(workerIdentity = zmsg_unwrap (msg))) {
        error ("Worker identity cannot be retrieved. Message dropped.");
        return -1;
    }

    // The worker finished its work; add it at the end of the list (round robin load balancing)
    zlist_append (self->readyWorkers, workerIdentity);

    // Get the content of the message
    if (!(packet = zmsg_first (msg))) {
        error ("Frame data cannot be retrieved. Message dropped.");
        return -1;
    }

    // Forward the message to the frontend if it's not a READY signal
    if (memcmp (zframe_data (packet), BARRACK_SERVER_WORKER_READY, sizeof(BARRACK_SERVER_WORKER_READY)) == 0) {
        zmsg_destroy (&msg);
    }
    else {
        zmsg_first (msg);
        zframe_t * secondFrame = zmsg_next (msg);

        if (zmsg_size (msg) == 2
        || (zmsg_size (msg) == 3 && zframe_size (secondFrame) == 0)) {
            // Simple message : [1 frame identity] + [1 frame data]
            // Or             : [1 frame identity] + [1 empty frame] + [1 frame data]
            if (zmsg_send (&msg, self->frontend) != 0) {
                error ("Cannot send message to the frontend. Message dropped");
                return -1;
            }
        } else {
            // Multiple messages : [1 frame identity] + [1 frame data] + [1 frame data] + ... + [1 frame data]
            // Send N messages [1 frame identity] + [1 frame data]
            zframe_t *identity = zmsg_pop (msg);
            int msgCount = zmsg_size (msg);
            for (int i = 0; i < msgCount; i++) {
                zmsg_t *subMsg = zmsg_new ();
                zmsg_add (subMsg, zframe_dup (identity));
                zmsg_add (subMsg, zmsg_pop (msg));
                zmsg_send (&subMsg, self->frontend);
            }
        }
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
        warning ("All Barrack Workers seem to be busy. Waiting for one to be ready.");
        // Naïve solution : Wait for one to be ready.
        // We could create a new worker on demand, so the Barrack scales itself depending on the charge
        // But let's keep it simple
        while (!(workerIdentity = (zframe_t *) zlist_pop (self->readyWorkers))) {
            zclock_sleep (1);
        }
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
    zsock_t *sessionServerFrontend;
    BarrackWorker * barrackWorker;
    zframe_t * pingPongFrame;

    // =============================================
    //    Wait for the session server to be alive
    // =============================================

    // Create and connect a socket to the session server frontend
    if (!(sessionServerFrontend = zsock_new (ZMQ_REQ))
    ||  zsock_connect (sessionServerFrontend, SESSION_SERVER_FRONTEND_ENDPOINT, self->sessionServerFrontendPort) == -1
    ) {
        error ("Barrack Server cannot connect to the Session Server.");
        return false;
    }

    // Test the connectivity : PING - PONG protocol
    if (!(pingPongFrame = zframe_new (SESSION_SERVER_PING, sizeof (SESSION_SERVER_PING)))
    ||  zframe_send (&pingPongFrame, sessionServerFrontend, 0) != 0
    ) {
        error ("Barrack Server cannot send a PING request to the Session Server.");
		return false;
    }

    if (!(pingPongFrame = zframe_recv (sessionServerFrontend))
    || memcmp (zframe_data (pingPongFrame), SESSION_SERVER_PONG, sizeof(SESSION_SERVER_PONG)) != 0
    ) {
        error ("Session Server didn't answer PONG correctly.");
		return false;
    }
    zframe_destroy (&pingPongFrame);

    // ===================================
    //       Initialize backend
    // ===================================

    // Create and connect a socket to the backend
    if (zsock_bind (self->backend, BARRACK_SERVER_BACKEND_ENDPOINT) == -1) {
        error ("Failed to bind Barrack Server ROUTER backend.");
        return false;
    }

    // Initialize workers - Start N worker threads.
    for (int workerId = 0; workerId < self->workersCount; workerId++) {
        if ((barrackWorker = BarrackWorker_new (workerId, self->sessionServerFrontendPort))) {
            if (zthread_new (BarrackWorker_worker, barrackWorker) != 0) {
                warning ("Cannot create Barrack Server worker thread ID %d.", workerId);
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
    // - It communicates with the server through a RAW socket.

    // Set the ROUTER as a RAW socket.
    zsock_set_router_raw (self->frontend, true);

    // Bind the 2 endpoints for the ROUTER frontend
    for (int i = 0; i < 2; i++) {
        if (zsock_bind (self->frontend, BARRACK_SERVER_FRONTEND_ENDPOINT, self->publicPorts[i]) == -1) {
            error ("Failed to bind Barrack Server ROUTER frontend to the port %d.", self->publicPorts[i]);
            return false;
        }
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

    dbg ("BarrackServer is ready and running.");
    if (zloop_start (reactor) != 0) {
        error ("An error occured in the reactor.");
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

    zlist_destroy (&self->readyWorkers);

    free (self);
    *_self = NULL;
}