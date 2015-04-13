/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SessionWorker.h
 * @brief SessionWorker is the worker for the session server
 *
 * - It receives a request from any entity
 * - It extracts the entity ID from the packet
 * - It searches for the key ID in its hashtable
 * - If it exists, return the data associated with the key
 * - If it doesn't, create the session
 * - Returns the session afterwards
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Session/SessionTable.h"
#include "Common/MySQL/MySQL.h"
#include "Common/Redis/Redis.h"

// ---------- Defines -------------

// Configuration default values
static const char SESSION_WORKER_SQL_HOSTNAME_DEFAULT[] = "localhost";
static const char SESSION_WORKER_SQL_LOGIN_DEFAULT[]    = "r1emu";
static const char SESSION_WORKER_SQL_PASSWORD_DEFAULT[] = "r1emu";
static const char SESSION_WORKER_SQL_DATABASE_DEFAULT[] = "r1emu";

#define SESSION_WORKER_REDIS_HOSTNAME_DEFAULT   "localhost"
#define SESSION_WORKER_REDIS_PORT_DEFAULT   "6379"

// ------ Structure declaration -------
/**
 * @brief SessionWorker is the minimal structure that a worker needs to work
 *        Everything else goes through the messaging system
 */
struct SessionWorker
{
    /** The worker ID */
    int workerId;

    /** The server ID using the session server */
    int serverId;

    /**< The worker socket connected to the backend. */
    zsock_t *worker;

    /** The MySQL session */
    SQL *sqlConn;

    /** The Redis session */
    Redis *redis;

    /** The table containing all the sessions.
     *  BE CAREFUL : It is shared between all the session workers of the same server.
     */
    SessionTable *sessionsTable;
};

typedef struct SessionWorker SessionWorker;

// ----------- Functions ------------

/**
 * @brief Allocate a new SessionWorker structure.
 * @param workerId The worker ID.
 * @param serverId The server ID.
 * @param sessionsTable The table containing all the sessions
 * @param confFilePath The configuration file path
 * @return A pointer to an allocated SessionWorker.
 */
SessionWorker *
SessionWorker_new (
    int workerId,
    int serverId,
    SessionTable *sessionsTable,
    char *confFilePath
);


/**
 * @brief Initialize an allocated SessionWorker structure.
 * @param self An allocated SessionWorker to initialize.
 * @param workerId The worker ID.
 * @param serverId The server ID.
 * @param sessionsTable The table containing all the sessions
 * @param confFilePath The configuration file path
 * @return true on success, false otherwise.
 */
bool
SessionWorker_init (
    SessionWorker *self,
    int workerId,
    int serverId,
    SessionTable *sessionsTable,
    char *confFilePath
);

/**
 * @brief Worker routine for the session server.
 * @param arg A SessionWorker
 * @return Always NULL
 */
void *
SessionWorker_worker (
    void *arg
);


/**
 * @brief Free an allocated SessionWorker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SessionWorker.
 */
void
SessionWorker_destroy (
    SessionWorker **self
);

/**
 * @brief Save the current session to the database.
 * @param self A pointer to an allocated SessionWorker.
 */
void
SessionWorker_flushSession (
    SessionWorker *self,
    ClientSession *session
);
