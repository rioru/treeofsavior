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

#include "GlobalServer/GlobalServer.h"

int main (int argc, char **argv)
{
    GlobalServer *globalServer;
    char *confFilePath;

    if (argc > 2) {
        confFilePath = argv[1];
    } else {
        confFilePath = DEFAULT_SERVER_CONF_PATH;
    }

    // Initialize seed
    R1EMU_seed_random ();

    // Initialize the Global Server
    if ((globalServer = GlobalServer_new (confFilePath))) {

        // Start the Global Server
        if (!GlobalServer_start (globalServer)) {
            error ("Cannot start the Global Server properly.");
        }

        // Unload the Global Server properly
        GlobalServer_destroy (&globalServer);
    }
    else {
        error ("Cannot initialize the Global Server properly.");
    }

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

	return 0;
}
