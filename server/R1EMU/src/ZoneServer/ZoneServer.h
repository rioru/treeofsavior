/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ZoneServer.h
 * @brief ZoneServer controls and processes the client actions
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Server/Server.h"

// ---------- Defines -------------
#define ZONE_SERVER_EXECUTABLE_NAME             "ZoneServer"

// Configuration default values
#define ZONE_SERVER_PORTS_DEFAULT               (char []) {"2004"}
#define ZONE_SERVER_WORKERS_COUNT_DEFAULT       1


// ------ Structure declaration -------
// ZoneServer is opaque
typedef struct ZoneServer ZoneServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new ZoneServer structure.
 * @param zoneServerId The zone server ID
 * @param serverIp The IP of the zone server
 * @param frontendPort The zone server port opened to the internet
 * @param workersCount Count of worker per zone server
 * @param privateGlobalPort The private port exposed to the global server
 * @param confFilePath The configuration file path
 * @return A pointer to an allocated ZoneServer.
 */
ZoneServer *
ZoneServer_new (
    Server *server
);


/**
 * @brief Initialize an allocated ZoneServer structure.
 * @param self An allocated ZoneServer to initialize.
 * @param zoneServerId The zone server ID
 * @param serverIp The IP of the zone server
 * @param frontendPort The zone server port opened to the internet
 * @param workersCount Count of worker per zone server
 * @param confFilePath The configuration file path
 * @return true on success, false otherwise.
 */
bool
ZoneServer_init (
    ZoneServer *self,
    Server *server
);


/**
 * @brief Free an allocated ZoneServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ZoneServer.
 */
void
ZoneServer_destroy (
    ZoneServer **self
);

/**
 * @brief Start the Zone Server main loop.
 * @param self An allocated ZoneServer
 * @return always NULL
 */
bool
ZoneServer_start (
    ZoneServer *self
);
