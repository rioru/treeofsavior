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
#include "ServerFactory.h"
#include "ZoneServer/ZoneHandler/ZoneHandler.h"
#include "BarrackServer/BarrackHandler/BarrackHandler.h"
#include "BarrackServer/BarrackServer.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

Server *
ServerFactory_createServer (
    int routerId,
    char *routerIp,
    int portsCount,
    int *ports,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    char *sqlHostname,
    char *sqlUsername,
    char *sqlPassword,
    char *sqlDatabase,
    char *redisHostname,
    int redisPort
) {
    Server *server;

    ServerStartupInfo serverInfo;
    if (!(ServerFactory_initServerInfo (&serverInfo,
        routerId, routerIp,
        portsCount, ports,
        workersCount,
        globalServerIp, globalServerPort,
        sqlHostname, sqlUsername, sqlPassword, sqlDatabase,
        redisHostname, redisPort
    ))) {
        error ("Cannot build a ServerStartupInfo.");
        return NULL;
    }

    // Allocate a new server
    if (!(server = Server_new (&serverInfo))) {
        error ("Cannot allocate a new Server.");
        return NULL;
    }

    return server;
}

bool
ServerFactory_initServerInfo (
    ServerStartupInfo *serverInfo,
    int routerId,
    char *routerIp,
    int portsCount,
    int *ports,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    char *sqlHostname,
    char *sqlUsername,
    char *sqlPassword,
    char *sqlDatabase,
    char *redisHostname,
    int redisPort
) {
    // Initialize Router start up information
    RouterStartupInfo routerInfo;
    if (!(RouterStartupInfo_init (&routerInfo, routerId, routerIp, ports, portsCount, workersCount))) {
        error ("Cannot initialize correctly the Router start up information.");
        return false;
    }

    // Initialize MySQL start up information
    MySQLStartupInfo sqlInfo;
    if (!(MySQLStartupInfo_init (&sqlInfo, sqlHostname, sqlUsername, sqlPassword, sqlDatabase))) {
        error ("Cannot initialize correctly the MySQL start up information.");
        return false;
    }

    // Initialize Redis start up information
    RedisStartupInfo redisInfo;
    if (!(RedisStartupInfo_init (&redisInfo, redisHostname, redisPort))) {
        error ("Cannot initialize correctly the Redis start up information.");
        return false;
    }

    // Initialize Worker start up information
    WorkerStartupInfo *workersInfo;
    if (!(workersInfo = malloc (sizeof (WorkerStartupInfo) * workersCount))) {
        error ("Cannot allocate an array of %d WorkerStartUpInfo.", workersCount);
        return false;
    }

    // Use the correct packetHandlers based on the ID
    const PacketHandler *packetHandlers;
    int packetHandlersCount;
    if (routerId == BARRACK_SERVER_ROUTER_ID) {
        packetHandlers = barrackHandlers;
        packetHandlersCount = PACKET_TYPE_COUNT;
    } else {
        packetHandlers = zoneHandlers;
        packetHandlersCount = PACKET_TYPE_COUNT;
    }

    for (uint16_t workerId = 0; workerId < workersCount; workerId++) {
        if (!(WorkerStartupInfo_init (
            &workersInfo[workerId],
            workerId, routerId,
            globalServerIp, globalServerPort,
            &sqlInfo, &redisInfo,
            packetHandlers, packetHandlersCount)
        )) {
            error ("Cannot initialize a Worker.");
            return false;
        }
    }

    // Initialize Server start up information
    if (!(ServerStartupInfo_init (serverInfo, &routerInfo, workersInfo, workersCount))) {
        error ("Cannot initialize correctly the Server start up information.");
        return false;
    }

    return true;
}
