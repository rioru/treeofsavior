/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SessionServer.h
 * @brief This module stores all the session of the clients
 *
 * SessionServer detains the hashtable of all the sessions in a given system.
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "SessionServer.h"
#include "SessionWorker/SessionWorker.h"
#include "SessionServer/ClientSession/ClientSession.h"


// ------ Structure declaration -------
/**
 * @brief SessionServer is the representation of the session server system
 *
 * It stores all the session of the clients in the barrack phase
 *
 */
struct SessionServer
{
    /** Session server frontend socket. Listens to ports exposed to the clients */
    zsock_t *frontend;

    /** Session server backend socket. Listens to "sessionWorkers" endpoint */
    zsock_t *backend;

    /** List of workers entities */
    zlist_t *readyWorkers;

    /** Hashtable of the sessions */
    zhash_t *sessions;

    // ----- Configuration -----
    /** Public ports exposed to the clients */
    int frontendPort;

    /** Number of workers allocated for the backend */
    int workersCount;
};


// ------ Static declaration ------

/**
 * @brief Frontend ROUTER handler of the Session Server
 * @param loop The reactor handler
 * @param frontend The frontend socket
 * @param self The sessionServer
 * @return 0 on success, -1 on error
 */
static int
SessionServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *self
);

/**
 * @brief Backend ROUTER handler of the Session Server
 * @param loop The reactor handler
 * @param backend The backend socket
 * @param self The sessionServer
 * @return 0 on success, -1 on error
 */
static int
SessionServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *self
);


// ------ Extern declaration ------

SessionServer *
SessionServer_new (
    char *confFilePath
) {
    SessionServer *self;

    if ((self = calloc (1, sizeof (SessionServer))) == NULL) {
        return NULL;
    }

    if (!SessionServer_init (self, confFilePath)) {
        SessionServer_destroy (&self);
        error ("SessionServer failed to initialize.");
        return NULL;
    }

    return self;
}


bool
SessionServer_init (
    SessionServer *self,
    char *confFilePath
) {
    zconfig_t *conf;

    // Only 1 worker for the session server.
    // It may change later, but for the moment there is no way to share
    // the hashtable containing all the sessions between multiple threads
    self->workersCount = 1;

    // ==================================
    //     Read the configuration file
    // ==================================

    // Open the configuration file
    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the session configuration file (%s).", confFilePath);
        return false;
    }

    // Read the port
    if (!(self->frontendPort = atoi (zconfig_resolve (conf, "sessionServer/port", STRINGIFY (SESSION_SERVER_PORT_DEFAULT))))
    ) {
        warning ("Cannot read correctly the session server port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", SESSION_SERVER_PORT_DEFAULT);
    }

    // Close the configuration file
    zconfig_destroy (&conf);

    // ==========================
    //   Allocate ZMQ objects
    // ==========================

    // The frontend listens to a 0MQ socket.
    if (!(self->frontend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate Session Server ROUTER frontend");
        return false;
    }

    // Backend listens to a 0MQ socket.
    if (!(self->backend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate Session Server ROUTER backend");
        return false;
    }

    // Allocate the workers entity list
    if (!(self->readyWorkers = zlist_new ())) {
        error ("Cannot allocate ready workers list.");
        return false;
    }

    // Allocate the sessions hashtable
    if (!(self->sessions = zhash_new ())) {
        error ("Cannot allocate a new hashtable for sessions.");
        return false;
    }

    return true;
}


static int
SessionServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    zframe_t *packet;
    SessionServer *self = (SessionServer *) _self;

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
    if (memcmp (zframe_data (packet), SESSION_SERVER_WORKER_READY, sizeof(SESSION_SERVER_WORKER_READY)) == 0) {
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
            zframe_destroy (&identity);
        }
    }

	return 0;
}


static int
SessionServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *workerIdentity;
    SessionServer *self = (SessionServer *) _self;

    // Receive the message from the frontend router
    if (!(msg = zmsg_recv (frontend))) {
        // Interrupt
        return 0;
    }

    // Retrieve a workerIdentity (round robin)
    if (!(workerIdentity = (zframe_t *) zlist_pop (self->readyWorkers))) {
        // All Session Workers seem to be busy.
        warning ("All Session Workers seem to be busy. Waiting for one to be ready.");
        // Naïve solution : Wait for one to be ready.
        // We could create a new worker on demand, so the Session scales itself depending on the charge
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
SessionServer_start (
    SessionServer *self
) {
    zloop_t *reactor;
    SessionWorker * sessionWorker;

    // ===================================
    //       Initialize backend
    // ===================================

    if (zsock_bind (self->backend, SESSION_SERVER_BACKEND_ENDPOINT) == -1) {
        error ("Failed to bind Session Server ROUTER backend.");
        return false;
    }

    // Initialize workers - Start N worker threads.
    for (int workerId = 0; workerId < self->workersCount; workerId++) {
        if (!(sessionWorker = SessionWorker_new (workerId, self->sessions))
        ||  zthread_new (SessionWorker_worker, sessionWorker) != 0
        ) {
            error ("Cannot allocate a new sessionWorker");
            return false;
        }
    }

    // ===================================
    //        Initialize frontend
    // ===================================

    // Bind the endpoint for the ROUTER frontend
    if (zsock_bind (self->frontend, SESSION_SERVER_FRONTEND_ENDPOINT, self->frontendPort) == -1) {
        error ("Failed to bind Session Server ROUTER frontend to the port %d.", self->frontendPort);
        return false;
    }

    // ====================================
    //   Prepare a reactor and fire it up
    // ====================================

    if (!(reactor = zloop_new ())) {
        error ("Cannot allocate a new reactor.");
        return false;
    }

	if (zloop_reader (reactor, self->backend,  SessionServer_backend,  self) == -1
    ||  zloop_reader (reactor, self->frontend, SessionServer_frontend, self) == -1
    ) {
        error ("Cannot register the sockets with the reactor.");
        return false;
    }

    dbg ("SessionServer is ready and running.");
    if (zloop_start (reactor) != 0) {
        error ("An error occured in the reactor.");
        return false;
    }

    zloop_destroy (&reactor);

    return true;
}


void
SessionServer_destroy (
    SessionServer **_self
) {
    SessionServer *self = *_self;

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
