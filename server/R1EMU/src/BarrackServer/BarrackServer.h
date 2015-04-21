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
#define BARRACK_SERVER_FRONTEND_ENDPOINT        "tcp://%s:%d"
#define BARRACK_SERVER_BACKEND_ENDPOINT         "inproc://barrackServerWorkersBackend"
#define BARRACK_SERVER_ZONE_ID                  0
#define BARRACK_SERVER_MAP_ID                   -1

// Configuration default values
#define BARRACK_SERVER_PORTS_DEFAULT            (char []) {"2000 2001"}
#define BARRACK_SERVER_WORKERS_COUNT_DEFAULT    3
#define BARRACK_SERVER_FRONTEND_IP_DEFAULT      (char []) {"127.0.0.1"}

/** Enumeration of all the packets headers that the barrack server handles */
// We want to differentiate the recv header from the send header, but we want to keep a list
// with uniques header id. So let's declare all the ids here, and distribute them afterward
typedef enum BarrackServerHeader {
    _BARRACK_SERVER_WORKER_READY,            // Ready
    _BARRACK_SERVER_WORKER_NORMAL,           // Normal message
    _BARRACK_SERVER_WORKER_ERROR,            // Error message received from a worker
    _BARRACK_SERVER_PING,                    // Ping
    _BARRACK_SERVER_PONG,                    // Pong
}   BarrackServerHeader;

// Macro helper for the distribution
#define DECL_BARRACK_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the barrack server accepts */
typedef enum BarrackServerRecvHeader {
    DECL_BARRACK_SERVER_HEADER (BARRACK_SERVER_PING),
    DECL_BARRACK_SERVER_HEADER (BARRACK_SERVER_WORKER_READY),
    DECL_BARRACK_SERVER_HEADER (BARRACK_SERVER_WORKER_ERROR),
    DECL_BARRACK_SERVER_HEADER (BARRACK_SERVER_WORKER_NORMAL),
}   BarrackServerRecvHeader;

/** Enumeration of all the packets header that the barrack server sends */
typedef enum BarrackServerSendHeader {
    DECL_BARRACK_SERVER_HEADER (BARRACK_SERVER_PONG),
}   BarrackServerSendHeader;

#undef DECL_BARRACK_SERVER_HEADER


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
    char *confFilePath
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
