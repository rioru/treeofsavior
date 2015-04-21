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
#include "Common/MySQL/MySQL.h"
#include "Common/Redis/Redis.h"

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
    int serverId;

    /** The front end port of the zone server */
    int frontendPort;

    /**< The session server socket connected to the frontend. */
    zsock_t *sessionServer;

    /** Private port with the global server */
    int globalPort;

    // Database stuff
    /** The MySQL session */
    MySQL *sqlConn;

    /** The Redis session */
    Redis *redis;
};

typedef struct ZoneWorker ZoneWorker;

// ----------- Functions ------------

/**
 * @brief Allocate a new ZoneWorker structure.
 * @param workerId The worker ID.
 * @param zoneServerId The zoneServer ID having autorithy on this worker
 * @param frontendPort The port of the zone server frontend
 * @param globalPort The private port of the global server
 * @return A pointer to an allocated ZoneWorker.
 */
ZoneWorker *
ZoneWorker_new (
    int workerId,
    int zoneServerId,
    int frontendPort,
    int globalPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
);


/**
 * @brief Initialize an allocated ZoneWorker structure.
 * @param self An allocated ZoneWorker to initialize.
 * @param workerId The worker ID.
 * @param zoneServerId The zoneServer ID having autorithy on this worker
 * @param globalPort The private port of the global server
 * @return true on success, false otherwise.
 */
bool
ZoneWorker_init (
    ZoneWorker *self,
    int workerId,
    int serverId,
    int frontendPort,
    int globalPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
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
 * @brief Request to the sessionServer to get the list of clients around the current session
 * @param arg A ZoneWorker
 * @return Always NULL
 */
bool ZoneWorker_sendToClients (
    ZoneWorker *self,
    unsigned char *packet,
    size_t packetSize,
    zlist_t * clientsList
);

/**
 * @brief Request a session from the Barrack server
 * @param self An allocated ZoneWorker
 * @param clientIdentity The identity of the client
 * @param accountId The account ID of the session
 * @return a zframe_t on success, NULL otherwise
 */
zframe_t *
ZoneWorker_getBarrackSession (
    ZoneWorker *self,
    zframe_t *clientIdentity,
    uint64_t accountId
);


/**
 * @brief Free an allocated ZoneWorker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ZoneWorker.
 */
void
ZoneWorker_destroy (
    ZoneWorker **self
);

