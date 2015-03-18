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

int main (int argc, char **argv)
{
    ZoneServer *ZoneServer;

    if (argc < 5) {
        error ("Wrong command line.");
        info ("Usage : ZoneServer <zoneServerId> <frontendPort> <workersCount> <privateGlobalPort>");
        return 0;
    }

    int zoneServerId = atoi (argv[1]);
    char *serverIp = argv[2];
    int frontendPort = atoi (argv[3]);
    int workersCount = atoi (argv[4]);
    int privateGlobalPort = atoi (argv[5]);

    // Initialize the Zone Server
    if ((ZoneServer = ZoneServer_new (zoneServerId, serverIp, frontendPort, workersCount, privateGlobalPort))) {

        // Start the Zone Server
        if (!ZoneServer_start (ZoneServer)) {
            error ("Cannot start the Zone Server properly.");
        }

        // Unload the Zone Server properly
        ZoneServer_destroy (&ZoneServer);
    }
    else {
        error ("Cannot initialize the Zone Server properly.");
    }

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

    return 0;
}
