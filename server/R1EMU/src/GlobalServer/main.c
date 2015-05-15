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
 * @brief Entry point of the Global Server
 *
 * Entry point of the Global Server
 *
 * @license <license placeholder>
 */

#include "Common/Server/ServerFactory.h"
#include "GlobalServer/GlobalServer.h"

int main (int argc, char **argv)
{
    GlobalServer *globalServer;

    // Force the initialization of the CZMQ layer here.
    if (!(zsys_init ())) {
        error ("Cannot init CZMQ.");
        goto main_cleanup;
    }

    // Get the configuration file
    char *confFilePath;
    if (argc >= 2) {
        confFilePath = argv[1];
    } else {
        confFilePath = DEFAULT_SERVER_CONF_PATH;
    }

    GlobalServerStartupInfo info;
    if (!(GlobalServerStartupInfo_init (&info, confFilePath))) {
        error ("Cannot initialize GlobalServer init information. (%s)", confFilePath);
        goto main_cleanup;
    }

    // Initialize the Server
    special ("======================");
    special ("=== Global server ===");
    special ("======================");
    // Initialize the Global Server
    if ((globalServer = GlobalServer_new (&info))) {

        // Flush the Redis server
        if (!(GlobalServer_flushRedis (globalServer))) {
            error ("Cannot flush the Redis server properly.");
        }

        // Start the Global Server
        else if (!GlobalServer_start (globalServer)) {
            error ("Cannot start the GlobalServer properly.");
        }
    }
    else {
        error ("Cannot initialize the GlobalServer properly.");
    }

main_cleanup:

    // Unload the Global Server properly
    GlobalServer_destroy (&globalServer);

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

    info ("Press any key to exit...");
    getc (stdout);

    return 0;
}
