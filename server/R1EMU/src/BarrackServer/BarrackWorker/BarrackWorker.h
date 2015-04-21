/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackWorker.h
 * @brief BarrackWorker is the worker for the barrack server
 *
 * - It reads the packet sent by the player
 * - It retrieves session from the session server if necessary (most of the case)
 * - It processes the request and send back the answer to the barrack server
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/MySQL/MySQL.h"
#include "Common/Redis/Redis.h"

// ---------- Defines -------------
/** This size is defined in the client, do not touch */
#define BARRACK_WORKER_BUFFER_SIZE      8192


// ------ Structure declaration -------

/**
 * @brief BarrackWorker is the minimal structure that a worker needs to work
 *        Everything else goes through the messaging system
 */
typedef struct BarrackWorker
{
    /** ID of the barrack worker */
    int workerId;

    /** The worker socket connected to the backend. */
    zsock_t *worker;

    /** Seed for the random generator */
    uint32_t seed;

    // Database stuff
    /** The MySQL session */
    MySQL *sqlConn;

    /** The Redis session */
    Redis *redis;

}   BarrackWorker;

// ----------- Functions ------------

/**
 * @brief Allocate a new BarrackWorker structure.
 * @param workerId The worker ID.
 * @param sqlInfo The information about the SQL database connection to etablish
 * @param redisInfo The information about the Redis database connection to etablish
 * @return A pointer to an allocated BarrackWorker.
 */
BarrackWorker *
BarrackWorker_new (
    int workerId,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
);


/**
 * @brief Initialize an allocated BarrackWorker structure.
 * @param self An allocated BarrackWorker to initialize.
 * @param workerId The worker ID.
 * @param sqlInfo The information about the SQL database connection to etablish
 * @param redisInfo The information about the Redis database connection to etablish
 * @return true on success, false otherwise.
 */
bool
BarrackWorker_init (
    BarrackWorker *self,
    int workerId,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
);


/**
 * @brief Worker routine for the barrack server.
 * @param args A BarrackWorker
 * @return Always NULL
 */
void *
BarrackWorker_worker (
    void *args
);


/**
 * @brief Free an allocated BarrackWorker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated BarrackWorker.
 */
void
BarrackWorker_destroy (
    BarrackWorker **self
);

