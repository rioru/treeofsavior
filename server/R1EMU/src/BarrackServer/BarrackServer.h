/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackServer.h
 * @brief BarrackServer controls and processes the client actions
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Server/Server.h"

// ---------- Defines -------------
#define BARRACK_SERVER_EXECUTABLE_NAME             "BarrackServer"

// Configuration default values
#define BARRACK_SERVER_PORTS_DEFAULT               (char []) {"2000"}
#define BARRACK_SERVER_WORKERS_COUNT_DEFAULT       1
#define BARRACK_SERVER_MAP_ID                      -1           /** The Barrack map hasn't any mapId, let's define one */
#define BARRACK_SERVER_FRONTEND_IP_DEFAULT         (char []) {"127.0.0.1"}


// ------ Structure declaration -------
// BarrackServer is opaque
typedef struct BarrackServer BarrackServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new BarrackServer structure.
 * @param BarrackServerId The barrack server ID
 * @param serverIp The IP of the barrack server
 * @param frontendPort The barrack server port opened to the internet
 * @param workersCount Count of worker per barrack server
 * @param privateGlobalPort The private port exposed to the global server
 * @param confFilePath The configuration file path
 * @return A pointer to an allocated BarrackServer.
 */
BarrackServer *
BarrackServer_new (
    Server *server
);


/**
 * @brief Initialize an allocated BarrackServer structure.
 * @param self An allocated BarrackServer to initialize.
 * @param BarrackServerId The barrack server ID
 * @param serverIp The IP of the barrack server
 * @param frontendPort The barrack server port opened to the internet
 * @param workersCount Count of worker per barrack server
 * @param confFilePath The configuration file path
 * @return true on success, false otherwise.
 */
bool
BarrackServer_init (
    BarrackServer *self,
    Server *server
);


/**
 * @brief Free an allocated BarrackServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated BarrackServer.
 */
void
BarrackServer_destroy (
    BarrackServer **self
);

/**
 * @brief Start the barrack Server main loop.
 * @param self An allocated BarrackServer
 * @return always NULL
 */
bool
BarrackServer_start (
    BarrackServer *self
);
