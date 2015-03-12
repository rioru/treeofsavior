/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GlobalWorker.h
 * @brief GlobalWorker is the worker for the global server
 *
 * - It receives a request from any entity
 * - It extracts the entity ID from the packet
 * - It searches for the key ID in its hashtable
 * - If it exists, return the data associated with the key
 * - If it doesn't, create the global
 * - Returns the global afterwards
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/**
 * @brief GlobalWorker is the minimal structure that a worker needs to work
 *        Everything else goes through the messaging system
 */
struct GlobalWorker
{
    /** The worker ID */
    int workerId;

    /**< The worker socket connected to the backend. */
    zsock_t *worker;
};

typedef struct GlobalWorker GlobalWorker;

// ----------- Functions ------------

/**
 * @brief Allocate a new GlobalWorker structure.
 * @param worker The worker ID.
 * @return A pointer to an allocated GlobalWorker.
 */
GlobalWorker *
GlobalWorker_new (
    int workerId
);


/**
 * @brief Initialize an allocated GlobalWorker structure.
 * @param self An allocated GlobalWorker to initialize.
 * @param worker The worker ID.
 * @return true on success, false otherwise.
 */
bool
GlobalWorker_init (
    GlobalWorker *self,
    int workerId
);


/**
 * @brief Worker routine for the global server.
 * @param arg A GlobalWorker
 * @return Always NULL
 */
void *
GlobalWorker_worker (
    void *arg
);


/**
 * @brief Free an allocated GlobalWorker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GlobalWorker.
 */
void
GlobalWorker_destroy (
    GlobalWorker **self
);

