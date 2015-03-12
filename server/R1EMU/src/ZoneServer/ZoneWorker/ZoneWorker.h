/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ZoneWorker.h
 * @brief ZoneWorker is the worker for the zone server
 *
 * - It receives a request from any entity
 * - It extracts the entity ID from the packet
 * - It searches for the key ID in its hashtable
 * - If it exists, return the data associated with the key
 * - If it doesn't, create the zone
 * - Returns the zone afterwards
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/**
 * @brief ZoneWorker is the minimal structure that a worker needs to work
 *        Everything else goes through the messaging system
 */
struct ZoneWorker
{
    /** The worker ID */
    int workerId;

    /** The zoneServer ID having autorithy on this worker */
    int zoneServerId;

    /** The front end port of the zone server */
    int frontendPort;

    /**< The worker socket connected to the backend. */
    zsock_t *worker;

    /**< The session server socket connected to the frontend. */
    zsock_t *sessionServer;

    /** Port of the session server */
    int sessionServerFrontendPort;
};

typedef struct ZoneWorker ZoneWorker;

// ----------- Functions ------------

/**
 * @brief Allocate a new ZoneWorker structure.
 * @param workerId The worker ID.
 * @param zoneServerId The zoneServer ID having autorithy on this worker
 * @param frontendPort The port of the zone server frontend
 * @param sessionServerFrontendPort The port of the session server frontend
 * @return A pointer to an allocated ZoneWorker.
 */
ZoneWorker *
ZoneWorker_new (
    int workerId,
    int zoneServerId,
    int frontendPort,
    int sessionServerFrontendPort
);


/**
 * @brief Initialize an allocated ZoneWorker structure.
 * @param self An allocated ZoneWorker to initialize.
 * @param workerId The worker ID.
 * @param zoneServerId The zoneServer ID having autorithy on this worker
 * @param frontendPort The port of the zone server frontend
 * @param sessionServerFrontendPort The port of the session server frontend
 * @return true on success, false otherwise.
 */
bool
ZoneWorker_init (
    ZoneWorker *self,
    int workerId,
    int zoneServerId,
    int frontendPort,
    int sessionServerFrontendPort
);


/**
 * @brief Worker routine for the zone server.
 * @param arg A ZoneWorker
 * @return Always NULL
 */
void *
ZoneWorker_worker (
    void *arg
);


/**
 * @brief Free an allocated ZoneWorker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ZoneWorker.
 */
void
ZoneWorker_destroy (
    ZoneWorker **self
);
