/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SessionServer.h
 * @brief SessionServer keeps in memory a table of all the client sessions
 *
 * SessionServer detains the hashtable of all the sessions.
 * It communicates with all the nodes that need session.
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------
#define SESSION_SERVER_FRONTEND_ENDPOINT    "tcp://127.0.0.1:%d"
#define SESSION_SERVER_BACKEND_ENDPOINT     "inproc://sessionServerWorkersBackend"

// Configuration default values
#define SESSION_SERVER_PORT_DEFAULT         2002

// Signals
#define SESSION_SERVER_WORKER_READY             "\001" // READY Signal
#define SESSION_SERVER_PING                     "\002" // PING Signal
#define SESSION_SERVER_PONG                     "\003" // PONG Signal
#define SESSION_SERVER_INVALID_PACKET           "\004" // Invalid packet Signal


// ------ Structure declaration -------

// SessionServer is opaque
typedef struct SessionServer SessionServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new SessionServer structure.
 * @param confFilePath The path to the configuration file associated with the Session Server
 * @return A pointer to an allocated SessionServer.
 */
SessionServer *
SessionServer_new (
    char *confFilePath
);


/**
 * @brief Initialize an allocated SessionServer structure.
 * @param self An allocated SessionServer to initialize.
 * @param confFilePath The path to the configuration file associated with the Session Server
 * @return true on success, false otherwise.
 */
bool
SessionServer_init (
    SessionServer *self,
    char *confFilePath
);


/**
 * @brief Free an allocated SessionServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SessionServer.
 */
void
SessionServer_destroy (
    SessionServer **self
);


/**
 * @brief Start the Session Server main loop.
 * @param self An allocated SessionServer
 * @return true on success, false otherwise
 */
bool
SessionServer_start (
    SessionServer *self
);
