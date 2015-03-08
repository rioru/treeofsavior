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
 * @brief Entry point of the Barrack Server
 *
 * Entry point of the Barrack Server
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "BarrackServer/BarrackServer.h"

int main (int argc, char **argv)
{
    BarrackServer *barrackServer;
    char *confFilePath;

    if (argc > 2) {
        confFilePath = argv[1];
    } else {
        confFilePath = DEFAULT_SERVER_CONF_PATH;
    }

    // Initialize seed
    R1EMU_seed_random ();

    // Initialize the Barrack Server
    if ((barrackServer = BarrackServer_new (confFilePath))) {

        // Start the Barrack Server
        if (!BarrackServer_start (barrackServer)) {
            error ("Cannot start the Barrack Server properly.");
        }

        // Unload the Barrack Server properly
        BarrackServer_destroy (&barrackServer);
    }
    else {
        error ("Cannot initialize the Barrack Server properly.");
    }

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

	return 0;
}
