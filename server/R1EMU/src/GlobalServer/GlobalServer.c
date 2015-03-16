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
#include "ZoneServer/ZoneServer.h"


// ------ Structure declaration -------
typedef struct {
    int sessionPort;
} ServerInformation;

/**
 * @brief GlobalServer detains the authority on all the zone servers
 * It communicates with the nodes that need to communicates with all the zone servers.
 */
struct GlobalServer
{
    /** Table of barrack and zone servers information. */
    ServerInformation *serversInformation;

    /** Socket listening to the CLI */
    zsock_t *cliConnection;

    /** Socket talking to the zones */
    zsock_t *zonesConnection;

    // ----- Configuration -----
    /** Frontend port of the global server. It shouldn't be opened on internet. */
    int cliPort;

    /** Zone servers ports. They should be opened to the internet, as clients will connect to them */
    int *zoneServersPorts;

    /** Count of zone servers */
    int zoneServersCount;

    /** Count of worker for each zone servers */
    int zoneWorkersCount;

    /** Port for communicating with the zones */
    int zonesPort;
};


// ------ Static declaration ------


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
    char *publicPortsArray;

    // ==================================
    //     Read the configuration file
    // ==================================

    // Open the configuration file
    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the global configuration file (%s).", confFilePath);
        return false;
    }

    // Read the frontend port
    if (!(self->cliPort = atoi (zconfig_resolve (conf, "globalServer/CLIport", NULL)))
    ) {
        warning ("Cannot read correctly the CLI port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", GLOBAL_SERVER_CLI_PORT_DEFAULT);
        self->cliPort = GLOBAL_SERVER_CLI_PORT_DEFAULT;
    }

    // Read the zones port
    if (!(self->zonesPort = atoi (zconfig_resolve (conf, "globalServer/zonesPort", NULL)))
    ) {
        warning ("Cannot read correctly the zones port port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", GLOBAL_SERVER_ZONES_PORT_DEFAULT);
        self->zonesPort = GLOBAL_SERVER_ZONES_PORT_DEFAULT;
    }

    // ==== Read the zone ports array ====
    if (!(publicPortsArray = zconfig_resolve (conf, "zoneServer/publicPortsArray", NULL))) {
        warning ("Public Zone ports cannot be read for Global Server. Defaults ports have been used : %s", ZONE_SERVER_PORTS_DEFAULT);
        publicPortsArray = ZONE_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    char *port = strtok (publicPortsArray, " ");
    while (port != NULL) {
        self->zoneServersCount++;
        port = strtok (NULL, " ");
    }

    // Fill the server ports array
    self->zoneServersPorts = calloc (self->zoneServersCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->zoneServersCount; portIndex++) {
        self->zoneServersPorts[portIndex] = strtol (publicPortsArray, &publicPortsArray, 10);
        publicPortsArray++;
    }

    // Read the number of barrack server workers
    if (!(self->zoneWorkersCount = atoi (zconfig_resolve (conf, "zoneServer/workers_count", NULL)))) {
        warning ("Cannot read correctly the zone workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", ZONE_SERVER_WORKERS_COUNT_DEFAULT);
        self->zoneWorkersCount = ZONE_SERVER_WORKERS_COUNT_DEFAULT;
    }

    // Close the configuration file
    zconfig_destroy (&conf);

    // ================================
    //    Allocate server information
    // ================================
    // + 1 because it counts the barrack server
    if (!(self->serversInformation = calloc (self->zoneServersCount + 1, sizeof (ServerInformation)))) {
        error ("Cannot allocate servers information array.");
        return false;
    }

    // ==========================
    //   Allocate ZMQ objects
    // ==========================
    if (!(self->cliConnection = zsock_new (ZMQ_RAW_ROUTER))) {
        error ("Cannot allocate a new CLI zsock.");
        return false;
    }
    if (!(self->zonesConnection = zsock_new (ZMQ_REQ))) {
        error ("Cannot allocate a new zones zsock.");
        return false;
    }

    return true;
}

int
GlobalServer_handleZonesRequest (
    GlobalServer *self,
    zsock_t *zone
) {
    zmsg_t *msg;

    if (!(msg = zmsg_recv (zone))) {
        dbg ("Global Server stops working.");
        return -2;
    }

    return 0;
}

int
GlobalServer_handleCliRequest (
    GlobalServer *self,
    zsock_t *zone
) {
    zmsg_t *msg;

    if (!(msg = zmsg_recv (zone))) {
        dbg ("Global Server stops working.");
        return -2;
    }

    return 0;
}


bool
GlobalServer_start (
    GlobalServer *self
) {
    zpoller_t *poller;
    bool isRunning = true;

    // ===================================
    //     Initialize CLI connection
    // ===================================
    // CLI should communicates through BSD sockets
    zsock_set_router_raw (self->cliConnection, true);

    if (zsock_bind (self->cliConnection, GLOBAL_SERVER_CLI_ENDPOINT, self->cliPort) == -1) {
        error ("Failed to bind CLI port.");
        return false;
    }
    info ("CLI connection binded on port %s.", zsys_sprintf (GLOBAL_SERVER_CLI_ENDPOINT, self->cliPort));


    // ===================================
    //     Initialize Zones connection
    // ===================================
    if (zsock_bind (self->zonesConnection, GLOBAL_SERVER_ZONES_ENDPOINT, self->zonesPort) == -1) {
        error ("Failed to bind zones port.");
        return false;
    }
    info ("Zones connection binded on port %s.", zsys_sprintf (GLOBAL_SERVER_ZONES_ENDPOINT, self->zonesPort));


    // ===================================
    //     Initialize N zone servers
    // ===================================

    for (int zoneServerId = 0; zoneServerId < self->zoneServersCount; zoneServerId++) {
        ZoneServer *zoneServer;

        if (!(zoneServer = ZoneServer_new (zoneServerId + 1, self->zoneServersPorts[zoneServerId], self->zoneWorkersCount, self->zonesPort))) {
            error ("Cannot create a new ZoneServer");
            continue;
        }

        if (!(ZoneServer_launchZoneServer (zoneServer))) {
            error ("Can't launch a new ZoneServer");
            continue;
        }

        ZoneServer_destroy (&zoneServer);
    }

    // Define a poller with the zones and the CLI sockets
    if (!(poller = zpoller_new (self->cliConnection, self->zonesConnection, NULL))) {
        error ("Global server cannot create a poller.");
        return NULL;
    }

    // Listens to requests
    info ("GlobalServer is ready and running.");

    while (isRunning) {
        zsock_t *actor = zpoller_wait (poller, -1);
        typedef int (*GlobalServerRequestHandler) (GlobalServer *self, zsock_t *actor);
        GlobalServerRequestHandler handler;

        // Get the correct handler based on the actor
        if (actor == self->zonesConnection) {
            handler = GlobalServer_handleZonesRequest;
        } else if (actor == self->cliConnection) {
            handler = GlobalServer_handleCliRequest;
        }
        else {
            error ("Unknown actor talked to the Global Server.");
            continue;
        }

        switch (handler (self, actor)) {
            case -1: // ERROR
                error ("Global Server encountered an error when handling a request.");
            case -2: // Connection stopped
                isRunning = false;
            break;

            case 0: // OK
            break;
        }
    }

    return true;
}


void
GlobalServer_destroy (
    GlobalServer **_self
) {
    GlobalServer *self = *_self;

    if (self->serversInformation) {
        free (self->serversInformation);
    }

    free (self);
    *_self = NULL;
}
