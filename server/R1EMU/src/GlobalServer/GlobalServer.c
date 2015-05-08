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
#include "BarrackServer/BarrackServer.h"
#include "Common/Server/ServerFactory.h"


// ------ Structure declaration -------
/**
 * @brief GlobalServer detains the authority on all the zone servers
 * It communicates with the nodes that need to communicates with all the zone servers.
 */
struct GlobalServer
{
    /** Socket listening to the CLI */
    zsock_t *cliConnection;

    /** Socket talking to the zones */
    zsock_t *zonesConnection;

    // ----- Configuration -----
    // === Global Server ===
    /** Server IP for the global server. */
    char *ip;

    /** Frontend port of the global server. It shouldn't be opened on internet. */
    int cliPort;

    // === Zone Server ===
    /** Zone servers ports. They should be opened to the internet, as clients will connect to them */
    int *zoneServersPorts;

    /** Zone servers IP. */
    char **zoneServersIp;

    /** Count of zone servers */
    int zoneServersCount;

    /** Count of worker for each zone servers */
    int zoneWorkersCount;

    /** Port for communicating with the zones */
    int zonesPort;

    // === Barrack Server ===
    /** Barrack servers ports. They should be opened to the internet, as clients will connect to them */
    int *barrackServerPort;

    /** Number of ports */
    int barrackServerPortCount;

    /** Barrack servers IP. */
    char *barrackServerIp;

    /** Count of workers */
    int barrackWorkersCount;

    /** Configuration file path */
    char *confFilePath;

    // === Database ===
    MySQLStartupInfo sqlInfo;
    RedisStartupInfo redisInfo;
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
    char *portsArray;

    self->confFilePath = confFilePath;

    // ==================================
    //     Read the configuration file
    // ==================================

    // ====== Read the global server configuration ======
    // Open the configuration file
    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the global configuration file (%s).", confFilePath);
        return false;
    }

    // Read the CLI serverIP
    if (!(self->ip = strdup (zconfig_resolve (conf, "globalServer/serverIP", NULL)))
    ) {
        warning ("Cannot read correctly the CLI serverIP in the configuration file (%s). ", confFilePath);
        warning ("The default serverIP = %s has been used.", GLOBAL_SERVER_CLI_IP_DEFAULT);
        self->ip = GLOBAL_SERVER_CLI_IP_DEFAULT;
    }

    // Read the CLI port
    if (!(self->cliPort = atoi (zconfig_resolve (conf, "globalServer/port", NULL)))
    ) {
        warning ("Cannot read correctly the CLI port in the configuration file (%s). ", confFilePath);
        warning ("The default port = %d has been used.", GLOBAL_SERVER_CLI_PORT_DEFAULT);
        self->cliPort = GLOBAL_SERVER_CLI_PORT_DEFAULT;
    }

    // ====== Read the zones server configuration ======
    // Read the zone ports array
    if (!(portsArray = zconfig_resolve (conf, "zoneServer/portsArray", NULL))) {
        warning ("Public Zone ports cannot be read for Global Server. Defaults ports have been used : %s", ZONE_SERVER_PORTS_DEFAULT);
        portsArray = ZONE_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    char *port = strtok (portsArray, " ");
    while (port != NULL) {
        self->zoneServersCount++;
        port = strtok (NULL, " ");
    }

    if (self->zoneServersCount == 0) {
        error ("Cannot read correctly the zone ports array.");
        return false;
    }

    // Fill the server ports array
    self->zoneServersPorts = calloc (self->zoneServersCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->zoneServersCount; portIndex++) {
        self->zoneServersPorts[portIndex] = strtol (portsArray, &portsArray, 10);
        portsArray++;
    }

    // Read the number of zone workers
    if (!(self->zoneWorkersCount = atoi (zconfig_resolve (conf, "zoneServer/workersCount", NULL)))) {
        warning ("Cannot read correctly the zone workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", ZONE_SERVER_WORKERS_COUNT_DEFAULT);
        self->zoneWorkersCount = ZONE_SERVER_WORKERS_COUNT_DEFAULT;
    }

    char *zoneServersIp;
    // Read the zone server interfaces IP
    if (!(zoneServersIp = zconfig_resolve (conf, "zoneServer/serversIP", NULL))) {
        error ("Cannot read correctly the zone servers interface IP in the configuration file (%s). ", confFilePath);
        return false;
    }

    int nbZoneServersIp = 0;
    char *routerIp = strtok (zoneServersIp, " ");
    while (routerIp != NULL) {
        routerIp = strtok (NULL, " ");
        nbZoneServersIp++;
    }

    if (nbZoneServersIp != self->zoneServersCount) {
        error ("Number of zone ports different from number of zone interfaces IP.");
        return false;
    }

    // Fill the zone server IPs array
    self->zoneServersIp = calloc (self->zoneServersCount, sizeof (char *));
    for (int ipIndex = 0; ipIndex < self->zoneServersCount; ipIndex++) {
        self->zoneServersIp[ipIndex] = strdup (zoneServersIp);
        zoneServersIp += strlen (zoneServersIp) + 1;
    }

    // ====== Read the barrack server configuration ======
    // Read the ports array
    if (!(portsArray = zconfig_resolve (conf, "barrackServer/portsArray", NULL))) {
        warning ("Ports cannot be read for Barrack Server. Defaults ports have been used : %s", BARRACK_SERVER_PORTS_DEFAULT);
        portsArray = BARRACK_SERVER_PORTS_DEFAULT;
    }

    // Tokenize the ports array
    port = strtok (portsArray, " ");
    while (port != NULL) {
        self->barrackServerPortCount++;
        port = strtok (NULL, " ");
    }

    // Fill the server ports array
    self->barrackServerPort = calloc (self->barrackServerPortCount, sizeof (int));
    for (int portIndex = 0; portIndex < self->barrackServerPortCount; portIndex++) {
        self->barrackServerPort[portIndex] = strtol (portsArray, &portsArray, 10);
        portsArray++;
    }

    // Read the number of barrack server workers
    if (!(self->barrackWorkersCount = atoi (zconfig_resolve (conf, "barrackServer/workersCount", NULL)))) {
        warning ("Cannot read correctly the barrack workers count in the configuration file (%s). ", confFilePath);
        warning ("The default worker count = %d has been used.", BARRACK_SERVER_WORKERS_COUNT_DEFAULT);
        self->barrackWorkersCount = BARRACK_SERVER_WORKERS_COUNT_DEFAULT;
    }

    // Read the server interface IP
    if (!(self->barrackServerIp = strdup (zconfig_resolve (conf, "barrackServer/serverIP", NULL)))) {
        warning ("Cannot read correctly the barrack interface IP in the configuration file (%s). ", confFilePath);
        warning ("The default IP = %s has been used.", BARRACK_SERVER_FRONTEND_IP_DEFAULT);
        self->barrackServerIp = BARRACK_SERVER_FRONTEND_IP_DEFAULT;
    }

    // ===================================
    //       Read MySQL configuration
    // ===================================
    if (!(self->sqlInfo.hostname = strdup (zconfig_resolve (conf, "database/mysql_host", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL host in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_HOSTNAME_DEFAULT);
        self->sqlInfo.hostname = MYSQL_HOSTNAME_DEFAULT;
    }
    if (!(self->sqlInfo.login = strdup (zconfig_resolve (conf, "database/mysql_user", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL user in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_LOGIN_DEFAULT);
        self->sqlInfo.login = MYSQL_LOGIN_DEFAULT;
    }
    if (!(self->sqlInfo.password = strdup (zconfig_resolve (conf, "database/mysql_password", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL password in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_PASSWORD_DEFAULT);
        self->sqlInfo.password = MYSQL_PASSWORD_DEFAULT;
    }
    if (!(self->sqlInfo.database = strdup (zconfig_resolve (conf, "database/mysql_database", NULL)))
    ) {
        warning ("Cannot read correctly the MySQL database in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", MYSQL_DATABASE_DEFAULT);
        self->sqlInfo.database = MYSQL_DATABASE_DEFAULT;
    }

    // ===================================
    //       Read Redis configuration
    // ===================================
    if (!(self->redisInfo.hostname = strdup (zconfig_resolve (conf, "redisServer/redis_host", NULL)))
    ) {
        warning ("Cannot read correctly the Redis host in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", REDIS_HOSTNAME_DEFAULT);
        self->redisInfo.hostname = REDIS_HOSTNAME_DEFAULT;
    }
    if (!(self->redisInfo.port = atoi (zconfig_resolve (conf, "redisServer/redis_port", NULL)))
    ) {
        warning ("Cannot read correctly the Redis port in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %d has been used.", REDIS_PORT_DEFAULT);
        self->redisInfo.port = REDIS_PORT_DEFAULT;
    }

    // Close the configuration file
    zconfig_destroy (&conf);

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
    ServerStartupInfo serverInfo;

    // ===================================
    //     Initialize CLI connection
    // ===================================
    // CLI should communicates through BSD sockets
    zsock_set_router_raw (self->cliConnection, true);

    if (zsock_bind (self->cliConnection, GLOBAL_SERVER_CLI_ENDPOINT, self->ip, self->cliPort) == -1) {
        error ("Failed to bind CLI port.");
        return false;
    }
    info ("CLI connection binded on port %s.", zsys_sprintf (GLOBAL_SERVER_CLI_ENDPOINT, self->ip, self->cliPort));


    // ===================================
    //     Initialize Zones connection
    // ===================================
    if ((self->zonesPort = zsock_bind (self->zonesConnection, GLOBAL_SERVER_ZONES_ENDPOINT, self->ip)) == -1) {
        error ("Failed to bind zones port.");
        return false;
    }
    info ("Zones connection binded on port %s.", zsys_sprintf (GLOBAL_SERVER_CLI_ENDPOINT, self->ip, self->zonesPort));

    // ===================================
    //     Initialize 1 Barrack Server
    // ===================================
    if (!(ServerFactory_initServerInfo (&serverInfo,
        BARRACK_SERVER_ROUTER_ID,
        self->barrackServerIp,
        self->barrackServerPortCount, self->barrackServerPort,
        self->zoneWorkersCount,
        self->ip, self->cliPort,
        self->sqlInfo.hostname, self->sqlInfo.login, self->sqlInfo.password, self->sqlInfo.database,
        self->redisInfo.hostname, self->redisInfo.port)
    )) {
        error ("[Barrack] Cannot create a new ServerInfo.");
        return false;
    }

    if (!(Server_createProcess (&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
        error ("[Barrack] Can't launch a new Server process.");
        return false;
    }

    // ===================================
    //     Initialize N Zone Server
    // ===================================
    for (uint16_t routerId = 0; routerId < self->zoneServersCount; routerId++)
    {
        ServerFactory_initServerInfo (&serverInfo,
            routerId + 1, // The ID = 0 is reserved for the Barrack Server, start at 1.
            self->zoneServersIp[routerId],
            1, &self->zoneServersPorts[routerId], // Only 1 port for each Zone server
            self->zoneWorkersCount,
            self->ip, self->cliPort,
            self->sqlInfo.hostname, self->sqlInfo.login, self->sqlInfo.password, self->sqlInfo.database,
            self->redisInfo.hostname, self->redisInfo.port
        );

        if (!(Server_createProcess (&serverInfo, ZONE_SERVER_EXECUTABLE_NAME))) {
            error ("[routerId=%d] Can't launch a new Server process.", routerId);
            return false;
        }
    }

    // Define a poller with the zones and the CLI sockets
    if (!(poller = zpoller_new (self->cliConnection, self->zonesConnection, NULL))) {
        error ("Global server cannot create a poller.");
        return false;
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
            warning ("An unknown actor talked to the Global Server. Maybe SIGINT signal ?");
            break;
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

    free (self);
    *_self = NULL;
}
