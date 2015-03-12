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
#include "GlobalServer.h"
#include "GlobalWorker/GlobalWorker.h"
#include "ZoneServer/ZoneServer.h"


// ------ Structure declaration -------
/**
 * @brief GlobalServer detains the authority on all the zone servers
 * It communicates with the nodes that need to communicates with all the zone servers.
 */
struct GlobalServer
{
    /** Barrack server frontend socket. Listens to ports exposed to the clients */
    zsock_t *frontend;

    /** Barrack server backend socket. Listens to "barrackWorkers" endpoint */
    zsock_t *backend;

    // ----- Configuration -----
    /** Frontend port of the global server. It shouldn't be opened on internet. */
    int frontendPort;

    /** Zone servers ports. They should be opened to the internet, as clients will connect to them */
    int *zoneServersPorts;

    /** Count of zone servers */
    int zoneServersCount;

    /** Count of worker for each zone servers */
    int zoneWorkersCount;

    /** Port of the session server */
    int sessionServerFrontendPort;
};


// ------ Static declaration ------

/**
 * @brief Frontend ROUTER handler of the Global Server
 * @param loop The reactor handler
 * @param frontend The frontend socket
 * @param self The globalServer
 * @return 0 on success, -1 on error
 */
static int
GlobalServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *self
);

/**
 * @brief Backend ROUTER handler of the Global Server
 * @param loop The reactor handler
 * @param backend The backend socket
 * @param self The globalServer
 * @return 0 on success, -1 on error
 */
static int
GlobalServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *self
);


// ------ Extern function implementation ------

GlobalServer *
GlobalServer_new (
    char *confFilePath
) {
    GlobalServer *self;

    if ((self = calloc (1, sizeof (GlobalServer))) == NULL) {
        return NULL;
    }

    if (!GlobalServer_init (self, confFilePath)) {
        GlobalServer_destroy (&self);
        error ("GlobalServer failed to initialize.");
        return NULL;
    }

    return self;
}


bool
GlobalServer_init (
    GlobalServer *self,
    char *confFilePath
) {
    zconfig_t *conf;
    char *portsArray;

    // ==================================
    //     Read the configuration file
    // ==================================

    // Open the configuration file
    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the global configuration file (%s).", confFilePath);
        return false;
    }

    // Read the frontend port
    if (!(self->frontendPort = atoi (zconfig_resolve (conf, "globalServer/port", NULL)))
    ) {
        warning ("Cannot read correctly the global server port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", GLOBAL_SERVER_PORT_DEFAULT);
        self->frontendPort = GLOBAL_SERVER_PORT_DEFAULT;
    }

    // Read the zone ports array
    if (!(portsArray = zconfig_resolve (conf, "zoneServer/portsArray", NULL))) {
        warning ("Ports cannot be read for Barrack Server. Defaults ports have been used : %s", ZONE_SERVER_PORTS_DEFAULT);
        portsArray = ZONE_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    char *port = strtok (portsArray, " ");
    while (port != NULL) {
        self->zoneServersCount++;
        port = strtok (NULL, " ");
    }

    // Fill the server ports array
    self->zoneServersPorts = calloc (self->zoneServersCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->zoneServersCount; portIndex++) {
        self->zoneServersPorts[portIndex] = strtol (portsArray, &portsArray, 10);
        portsArray++;
    }

    // Read the Session Server frontend port
    if (!(self->sessionServerFrontendPort = atoi (zconfig_resolve (conf, "sessionServer/port", NULL)))
    ) {
        warning ("Cannot read correctly the session server port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", SESSION_SERVER_PORT_DEFAULT);
        self->sessionServerFrontendPort = SESSION_SERVER_PORT_DEFAULT;
    }

    // Read the number of barrack server workers
    if (!(self->zoneWorkersCount = atoi (zconfig_resolve (conf, "zoneServer/workers_count", NULL)))) {
        warning ("Cannot read correctly the zone workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", ZONE_SERVER_WORKERS_COUNT_DEFAULT);
        self->zoneWorkersCount = ZONE_SERVER_WORKERS_COUNT_DEFAULT;
    }

    // Close the configuration file
    zconfig_destroy (&conf);

    // ==========================
    //   Allocate ZMQ objects
    // ==========================

    // The frontend listens to a 0MQ socket.
    if (!(self->frontend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate Global Server ROUTER frontend");
        return false;
    }

    // Backend listens to a 0MQ socket.
    if (!(self->backend = zsock_new (ZMQ_ROUTER))) {
        error ("Cannot allocate Global Server ROUTER backend");
        return false;
    }

    return true;
}


static int
GlobalServer_backend (
    zloop_t *loop,
    zsock_t *backend,
    void *_self
) {
    zmsg_t *msg;
    zframe_t *identity;
    zframe_t *packet;
    GlobalServer *self = (GlobalServer *) _self;

    // Receive the message from the backend router
    if (!(msg = zmsg_recv (backend))) {
        // Interrupt
        return 0;
    }

    // Retrieve the identity who sent the message
    if (!(identity = zmsg_unwrap (msg))) {
        error ("Worker identity cannot be retrieved.");
        return -1;
    }

    // Get the content of the message
    if (!(packet = zmsg_first (msg))) {
        error ("Frame data cannot be retrieved.");
        return -1;
    }

    // Forward the message to the frontend if it's not a READY signal
    if (memcmp (zframe_data (packet), PACKET_HEADER (GLOBAL_SERVER_WORKER_READY), sizeof(GLOBAL_SERVER_WORKER_READY)) == 0) {
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
GlobalServer_frontend (
    zloop_t *loop,
    zsock_t *frontend,
    void *_self
) {
    zmsg_t *msg;
    // GlobalServer *self = (GlobalServer *) _self;

    // Receive the message from the frontend router
    if (!(msg = zmsg_recv (frontend))) {
        // Interrupt
        return 0;
    }

    // Forward message to the targeted zone server
    /*
    if (zmsg_send (&msg, self->backend) != 0) {
        error ("Frontend cannot send message to the backend");
        return -1;
    }
    */

    return 0;
}


bool
GlobalServer_start (
    GlobalServer *self
) {
    zloop_t *reactor;

    // ===================================
    //       Initialize backend
    // ===================================

    if (zsock_bind (self->backend, GLOBAL_SERVER_BACKEND_ENDPOINT) == -1) {
        error ("Failed to bind Global Server ROUTER backend.");
        return false;
    }
    info ("Backend listening on %s.", GLOBAL_SERVER_BACKEND_ENDPOINT);

    // Initialize N zone servers.
    for (int zoneServerId = 0; zoneServerId < self->zoneServersCount; zoneServerId++) {
        ZoneServer *zoneServer = ZoneServer_new (
            zoneServerId, self->zoneServersPorts[zoneServerId], self->zoneWorkersCount, self->sessionServerFrontendPort
        );
        zthread_new (ZoneServer_start, zoneServer);
    }

    // ====================================
    //   Prepare a reactor and fire it up
    // ====================================

    if (!(reactor = zloop_new ())) {
        error ("Cannot allocate a new reactor.");
        return false;
    }

	if (zloop_reader (reactor, self->backend,  GlobalServer_backend,  self) == -1
    ||  zloop_reader (reactor, self->frontend, GlobalServer_frontend, self) == -1
    ) {
        error ("Cannot register the sockets with the reactor.");
        return false;
    }

    info ("GlobalServer is ready and running.");
    if (zloop_start (reactor) != 0) {
        error ("An error occurred in the reactor.");
        return false;
    }

    zloop_destroy (&reactor);

    return true;
}


void
GlobalServer_destroy (
    GlobalServer **_self
) {
    GlobalServer *self = *_self;

    if (self->frontend) {
        zsock_destroy (&self->frontend);
    }

    if (self->backend) {
        zsock_destroy (&self->backend);
    }

    free (self);
    *_self = NULL;
}