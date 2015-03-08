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
 * @brief Entry point for clients on the login screen
 *
 * This module accepts login requests from clients and manage the connection through the barrack phase.
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "SessionServer/SessionServer.h"

// ---------- Defines -------------
#define BARRACK_SERVER_FRONTEND_ENDPOINT        "tcp://127.0.0.1:%d"
#define BARRACK_SERVER_BACKEND_ENDPOINT         "inproc://barrackServerWorkersBackend"

// Configuration default values
#define BARRACK_SERVER_PORT1_DEFAULT            2000
#define BARRACK_SERVER_PORT2_DEFAULT            2001
#define BARRACK_SERVER_WORKERS_COUNT_DEFAULT    3

// Signals
#define BARRACK_SERVER_WORKER_READY             "\001" // Ready Signal
#define BARRACK_SERVER_PING                     "\002" // PING Signal
#define BARRACK_SERVER_PONG                     "\003" // PONG Signal
#define BARRACK_SERVER_INVALID_PACKET           "\004" // Invalid packet Signal

// ------ Structure declaration -------

// Barrack Server is opaque
typedef struct BarrackServer BarrackServer;


// ----------- Functions ------------

/**
 * @brief Allocate a new BarrackServer structure.
 * @param confFilePath The path to the configuration file associated with the Barrack Server
 * @return A pointer to an allocated BarrackServer.
 */
BarrackServer *
BarrackServer_new (
    char * confFilePath
);


/**
 * @brief Initialize an allocated BarrackServer structure.
 * @param self An allocated BarrackServer to initialize.
 * @param confFilePath The path to the configuration file associated with the Barrack Server
 * @return true on success, false otherwise
 */
bool
BarrackServer_init (
    BarrackServer *self,
    const char *confFilePath
);



/**
 * @brief Free an allocated BarrackServer structure and nullify the content of the pointer
 * @param self A pointer to an allocated BarrackServer
 */
void
BarrackServer_destroy (
    BarrackServer **self
);


/**
 * @brief Start the Barrack Server main loop.
 * @param self An allocated BarrackServer
 * @return true on success, false otherwise
 */
bool
BarrackServer_start (
    BarrackServer *self
);
