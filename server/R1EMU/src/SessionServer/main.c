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
 * @brief Entry point of the Session Server
 *
 * Entry point of the Session Server
 *
 * @license <license placeholder>
 */

#include "SessionServer/SessionServer.h"

int main (int argc, char **argv)
{
    SessionServer *sessionServer;
    char *confFilePath;

    if (argc > 2) {
        confFilePath = argv[1];
    } else {
        confFilePath = DEFAULT_SERVER_CONF_PATH;
    }

    // Initialize seed
    R1EMU_seed_random ();

    // Initialize the Session Server
    if ((sessionServer = SessionServer_new (confFilePath))) {

        // Start the Session Server
        if (!SessionServer_start (sessionServer)) {
            error ("Cannot start the Session Server properly.");
        }

        // Unload the Session Server properly
        SessionServer_destroy (&sessionServer);
    }
    else {
        error ("Cannot initialize the Session Server properly.");
    }

    // Shutdown the CZMQ layer properly
    zsys_shutdown ();

	return 0;
}
