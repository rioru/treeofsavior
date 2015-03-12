/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GlobalServer.h
 * @brief GlobalServer keeps a table of all the zone servers and communicate with them.
 *
 * GlobalServer detains the authority on all the zone servers. It can start or shut them down.
 * It communicates with the nodes that need to communicates with all the zone servers.
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------
#define GLOBAL_SERVER_FRONTEND_ENDPOINT    "tcp://127.0.0.1:%d"
#define GLOBAL_SERVER_BACKEND_ENDPOINT     "inproc://globalServerWorkersBackend"

// Configuration default values
#define GLOBAL_SERVER_PORT_DEFAULT         2003

/** Enumeration of all the packets headers that the global server handles */
// We want to differentiate the recv header from the send header, but we want to keep a list
// with uniques header id. So let's declare all the ids here, and distribute them afterward
typedef enum GlobalServerHeader {
    _GLOBAL_SERVER_WORKER_READY,            // Ready
    _GLOBAL_SERVER_PING,                    // Ping
    _GLOBAL_SERVER_PONG,                    // Pong
}   GlobalServerHeader;

// Macro helper for the distribution
#define DECL_GLOBAL_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the global server accepts */
typedef enum GlobalServerRecvHeader {
    DECL_GLOBAL_SERVER_HEADER (GLOBAL_SERVER_PING),
}   GlobalServerRecvHeader;

/** Enumeration of all the packets header that the global server sends */
typedef enum GlobalServerSendHeader {
    DECL_GLOBAL_SERVER_HEADER (GLOBAL_SERVER_WORKER_READY),
    DECL_GLOBAL_SERVER_HEADER (GLOBAL_SERVER_PONG),
}   GlobalServerSendHeader;

#undef DECL_GLOBAL_SERVER_HEADER


// ------ Structure declaration -------
// GlobalServer is opaque
typedef struct GlobalServer GlobalServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new GlobalServer structure.
 * @param confFilePath The path to the configuration file associated with the Global Server
 * @return A pointer to an allocated GlobalServer.
 */
GlobalServer *
GlobalServer_new (
    char *confFilePath
);


/**
 * @brief Initialize an allocated GlobalServer structure.
 * @param self An allocated GlobalServer to initialize.
 * @param confFilePath The path to the configuration file associated with the Global Server
 * @return true on success, false otherwise.
 */
bool
GlobalServer_init (
    GlobalServer *self,
    char *confFilePath
);


/**
 * @brief Free an allocated GlobalServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GlobalServer.
 */
void
GlobalServer_destroy (
    GlobalServer **self
);


/**
 * @brief Start the Global Server main loop.
 * @param self An allocated GlobalServer
 * @return true on success, false otherwise
 */
bool
GlobalServer_start (
    GlobalServer *self
);
