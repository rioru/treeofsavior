/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Redis.h
 * @brief Redis set of functions for communicating with the Redis server.
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Commander/Commander.h"
#include "Common/Session/ClientSession.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef struct Redis Redis;

// ----------- Functions ------------

/**
 * @brief Allocate a new Redis structure.
 * @param ip The IP address of the Redis server
 * @param port The port of the Redis server
 * @return A pointer to an allocated Redis.
 */
Redis *
Redis_new (
    char *ip,
    int port
);


/**
 * @brief Initialize an allocated Redis structure.
 * @param self An allocated Redis to initialize.
 * @param ip The IP address of the Redis server
 * @param port The port of the Redis server
 * @return true on success, false otherwise.
 */
bool
Redis_init (
    Redis *self,
    char *ip,
    int port
);


/**
 * @brief Save a ClientSession to the Redis server.
 * @param self An allocated Redis instance
 * @param session An allocated session to save
 * @return true on success, false otherwise
 */
bool
Redis_setSession (
    Redis *self,
    ClientSession *session
);


/**
 * @brief Free an allocated Redis structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Redis.
 */
void
Redis_destroy (
    Redis **self
);

