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

    /** The session server socket connected to the frontend. */
    zsock_t *sessionServer;

    /** Seed for the random generator */
    uint32_t seed;

}   BarrackWorker;

// ----------- Functions ------------

/**
 * @brief Allocate a new BarrackWorker structure.
 * @param worker The worker ID.
 * @return A pointer to an allocated BarrackWorker.
 */
BarrackWorker *
BarrackWorker_new (
    int workerId
);


/**
 * @brief Initialize an allocated BarrackWorker structure.
 * @param self An allocated BarrackWorker to initialize.
 * @param worker The worker ID.
 * @return true on success, false otherwise.
 */
bool
BarrackWorker_init (
    BarrackWorker *self,
    int workerId
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

