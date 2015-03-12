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

/** Enumeration of all the packets headers that the session server handles */
// We want to differentiate the recv header from the send header, but we want to keep a list
// with uniques header id. So let's declare all the ids here, and distribute them afterward
typedef enum SessionServerHeader {
    _SESSION_SERVER_WORKER_READY,            // Ready
    _SESSION_SERVER_PING,                    // Ping
    _SESSION_SERVER_PONG,                    // Pong
    _SESSION_SERVER_REQUEST_SESSION,         // Request a session
    _SESSION_SERVER_UPDATE_SESSION,          // Update a session
    _SESSION_SERVER_UPDATE_SESSION_OK,       // Session update success
    _SESSION_SERVER_UPDATE_SESSION_FAILED,   // Session update fail
}   SessionServerHeader;

// Macro helper for the distribution
#define DECL_SESSION_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the session server accepts */
typedef enum SessionServerRecvHeader {
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_PING),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_REQUEST_SESSION),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_UPDATE_SESSION),
}   SessionServerRecvHeader;

/** Enumeration of all the packets header that the session server sends */
typedef enum SessionServerSendHeader {
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_WORKER_READY),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_PONG),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_UPDATE_SESSION_OK),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_UPDATE_SESSION_FAILED),
}   SessionServerSendHeader;

#undef DECL_SESSION_SERVER_HEADER


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
