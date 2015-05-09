/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file main.c
 * @brief Entry point of the Zone Server
 *
 * Entry point of the Zone Server
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "ZoneServer/ZoneServer.h"
#include "BarrackServer/BarrackServer.h"
#include "Common/Server/ServerFactory.h"

int main (int argc, char **argv)
{
    ZoneServer *zoneServer;
    BarrackServer *barrackServer;

    int curArg = 1;

    // === Read the command line arguments ===
    int routerId = atoi (argv[curArg++]);

    // Set a custom output for linux for each servers,
    #ifndef WIN32
    dbg_set_output (fopen (zsys_sprintf ("ZoneServer%d_output.txt", routerId), "w+"));
    #else
    if (routerId == BARRACK_SERVER_ROUTER_ID) {
        SetConsoleTitle (zsys_sprintf ("Barrack Server (RouterID = %d)", BARRACK_SERVER_ROUTER_ID));
    } else {
        SetConsoleTitle (zsys_sprintf ("Zone Server (RouterID = %d)", routerId));
    }
    #endif

    char *routerIp = argv[curArg++];
    int portsCount = atoi (argv[curArg++]);
    int *ports;
    if (!(ports = malloc (sizeof (int) * portsCount))) {
        error ("Cannot allocate the port array.");
        return -1;
    }
    for (int i = 0; i < portsCount; i++) {
        ports[i] = atoi (argv[curArg++]);
    }
    int workersCount = atoi (argv[curArg++]);
    char *globalServerIp = argv[curArg++];
    int globalServerPort = atoi (argv[curArg++]);
    char *sqlHostname = argv[curArg++];
    char *sqlUsername = argv[curArg++];
    char *sqlPassword = argv[curArg++];
    char *sqlDatabase = argv[curArg++];
    char *redisHostname = argv[curArg++];
    int redisPort = atoi(argv[curArg++]);

    special ("Parameters received : ");
    special ("routerId = %d", routerId);
    special ("routerIp = %s", routerIp);
    special ("portsCount = %d", portsCount);
    special ("workersCount = %d", workersCount);
    special ("globalServerIp = %s", globalServerIp);
    special ("globalServerPort = %d", globalServerPort);
    special ("sqlHostname = %s", sqlHostname);
    special ("sqlUsername = %s", sqlUsername);
    special ("sqlPassword = %s", sqlPassword);
    special ("sqlDatabase = %s", sqlDatabase);
    special ("redisHostname = %s", redisHostname);
    special ("redisPort = %d", redisPort);


    // === Build the Server ===
    Server *server;
    if (!(server = ServerFactory_createServer (
        routerId, routerIp,
        portsCount, ports,
        workersCount,
        globalServerIp, globalServerPort,
        sqlHostname, sqlUsername, sqlPassword, sqlDatabase,
        redisHostname, redisPort
    ))) {
        error ("Cannot create a Server.");
        return -1;
    }

    // Initialize the Server
    if (routerId == BARRACK_SERVER_ROUTER_ID)
    {
        special ("======================");
        special ("=== Barrack server ===");
        special ("======================");
        // Initialize the Barrack Server
        if ((barrackServer = BarrackServer_new (server))) {

            // Start the Barrack Server
            if (!BarrackServer_start (barrackServer)) {
                error ("Cannot start the BarrackServer properly.");
            }

            // Unload the Barrack Server properly
            BarrackServer_destroy (&barrackServer);
        }
        else {
            error ("Cannot initialize the BarrackServer properly.");
        }
    }

    else
    {
        special ("=====================");
        special ("=== Zone server %d ===", routerId);
        special ("=====================");
        // Initialize the Zone Server
        if ((zoneServer = ZoneServer_new (server))) {

            // Start the Zone Server
            if (!ZoneServer_start (zoneServer)) {
                error ("Cannot start the Zone Server properly.");
            }

            // Unload the Zone Server properly
            ZoneServer_destroy (&zoneServer);
        }
        else {
            error ("Cannot initialize the Zone Server properly.");
        }
    }

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

    // Close the custom debug file if necessary
    dbg_close ();

    info ("Press any key to exit...");
    fgetc (stdin);

    return 0;
}
