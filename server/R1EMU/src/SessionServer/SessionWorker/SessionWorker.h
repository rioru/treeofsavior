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

// ---------- Defines -------------


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

    /** The hashtable containing all the sessions.
     *  BE CAREFUL : It is shared between all the session workers of the same server.
     */
    zhash_t *sessions;
};

typedef struct SessionWorker SessionWorker;

// ----------- Functions ------------

/**
 * @brief Allocate a new SessionWorker structure.
 * @param workerId The worker ID.
 * @param serverId The server ID.
 * @param sessions The hashtable containing all the sessions
 * @return A pointer to an allocated SessionWorker.
 */
SessionWorker *
SessionWorker_new (
    int workerId,
    int serverId,
    zhash_t *sessions
);


/**
 * @brief Initialize an allocated SessionWorker structure.
 * @param self An allocated SessionWorker to initialize.
 * @param workerId The worker ID.
 * @param serverId The server ID.
 * @param sessions The hashtable containing all the sessions
 * @return true on success, false otherwise.
 */
bool
SessionWorker_init (
    SessionWorker *self,
    int workerId,
    int serverId,
    zhash_t *sessions
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

