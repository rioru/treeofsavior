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
#include "SessionServer/SessionServer.h"

// ---------- Defines -------------
#define ZONE_SERVER_FRONTEND_ENDPOINT    "tcp://127.0.0.1:%d"
#define ZONE_SERVER_BACKEND_ENDPOINT     "inproc://zoneServerWorkersBackend-%d"

// Configuration default values
#define ZONE_SERVER_PORTS_DEFAULT               (char []) {"2004 2005 2006"}
#define ZONE_SERVER_WORKERS_COUNT_DEFAULT       3

/** Enumeration of all the packets headers that the zone server handles */
// We want to differentiate the recv header from the send header, but we want to keep a list
// with uniques header id. So let's declare all the ids here, and distribute them afterward
typedef enum ZoneServerHeader {
    _ZONE_SERVER_WORKER_READY,            // Ready
    _ZONE_SERVER_PING,                    // Ping
    _ZONE_SERVER_PONG,                    // Pong
}   ZoneServerHeader;

// Macro helper for the distribution
#define DECL_ZONE_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the zone server accepts */
typedef enum ZoneServerRecvHeader {
    DECL_ZONE_SERVER_HEADER (ZONE_SERVER_PING),
}   ZoneServerRecvHeader;

/** Enumeration of all the packets header that the zone server sends */
typedef enum ZoneServerSendHeader {
    DECL_ZONE_SERVER_HEADER (ZONE_SERVER_WORKER_READY),
    DECL_ZONE_SERVER_HEADER (ZONE_SERVER_PONG),
}   ZoneServerSendHeader;

#undef DECL_ZONE_SERVER_HEADER


// ------ Structure declaration -------

// ZoneServer is opaque
typedef struct ZoneServer ZoneServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new ZoneServer structure.
 * @param zoneServerId The zone server ID
 * @param frontendPort The zone server port opened to the internet
 * @param workersCount Count of worker per zone server
 * @param sessionServerFrontendPort The port of the session server frontend
 * @return A pointer to an allocated ZoneServer.
 */
ZoneServer *
ZoneServer_new (
    int zoneServerId,
    int frontendPort,
    int workersCount,
    int sessionServerFrontendPort
);


/**
 * @brief Initialize an allocated ZoneServer structure.
 * @param self An allocated ZoneServer to initialize.
 * @param zoneServerId The zone server ID
 * @param frontendPort The zone server port opened to the internet
 * @param workersCount Count of worker per zone server
 * @param sessionServerFrontendPort The port of the session server frontend
 * @return true on success, false otherwise.
 */
bool
ZoneServer_init (
    ZoneServer *self,
    int zoneServerId,
    int frontendPort,
    int workersCount,
    int sessionServerFrontendPort
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
 * @param args An allocated ZoneServer
 * @return always NULL
 */
void *
ZoneServer_start (
    void *args
);