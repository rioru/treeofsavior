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
#define SESSION_SERVER_FRONTEND_ENDPOINT        "inproc://sessionServerWorkersFrontend-%d"
#define SESSION_SERVER_BACKEND_ENDPOINT         "inproc://sessionServerWorkersBackend-%d"

/** Enumeration of all the packets headers that the session server handles */
// We want to differentiate the recv header from the send header, but we want to keep a list
// with uniques header id. So let's declare all the ids here, and distribute them afterward
typedef enum SessionServerHeader {
    _SESSION_SERVER_WORKER_READY,            // Ready
    _SESSION_SERVER_PING,                    // Ping
    _SESSION_SERVER_PONG,                    // Pong
    _SESSION_SERVER_REQUEST_SESSION,         // Request a session with a socket ID
    _SESSION_SERVER_REQUEST_BARRACK_SESSION, // Request a session from the barrack server with an account ID
    _SESSION_SERVER_UPDATE_SESSION,          // Update a session
    _SESSION_SERVER_UPDATE_SESSION_OK,       // Session update success
    _SESSION_SERVER_UPDATE_SESSION_FAILED,   // Session update fail
    _SESSION_SERVER_DELETE_SESSION,          // Delete a session
    _SESSION_SERVER_DELETE_SESSION_OK,       // Session delete success
    _SESSION_SERVER_DELETE_SESSION_FAILED,   // Session delete fail
}   SessionServerHeader;

// Macro helper for the distribution
#define DECL_SESSION_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the session server accepts */
typedef enum SessionServerRecvHeader {
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_PING),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_REQUEST_SESSION),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_REQUEST_BARRACK_SESSION),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_UPDATE_SESSION),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_DELETE_SESSION),
}   SessionServerRecvHeader;

/** Enumeration of all the packets header that the session server sends */
typedef enum SessionServerSendHeader {
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_WORKER_READY),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_PONG),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_UPDATE_SESSION_OK),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_UPDATE_SESSION_FAILED),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_DELETE_SESSION_OK),
    DECL_SESSION_SERVER_HEADER (SESSION_SERVER_DELETE_SESSION_FAILED),
}   SessionServerSendHeader;

#undef DECL_SESSION_SERVER_HEADER


// ------ Structure declaration -------

// SessionServer is opaque
typedef struct SessionServer SessionServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new SessionServer structure.
 * @param serverId The server ID using the session server
 * @param confFilePath The configuration file
 * @return A pointer to an allocated SessionServer.
 */
SessionServer *
SessionServer_new (
    int serverId,
    char *confFilePath
);


/**
 * @brief Initialize an allocated SessionServer structure.
 * @param self An allocated SessionServer to initialize.
 * @param serverId The server ID using the session server
 * @param confFilePath The configuration file
 * @return true on success, false otherwise.
 */
bool
SessionServer_init (
    SessionServer *self,
    int serverId,
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
