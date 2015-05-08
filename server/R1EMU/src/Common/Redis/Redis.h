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
#include "Common/Session/SocketSession.h"
#include "Common/Session/GameSession.h"

// ---------- Defines -------------
#define REDIS_EMPTY_STRING (unsigned char *) "__EMPTY_STRING__"

#define REDIS_HOSTNAME_DEFAULT   (char []) {"localhost"}
#define REDIS_PORT_DEFAULT       6379

// ------ Structure declaration -------
typedef struct Redis Redis;

typedef struct RedisStartupInfo
{
    char *hostname;
    int port;

}   RedisStartupInfo;

// ----------- Functions ------------

/**
 * @brief Allocate a new Redis structure.
 * @param redisInfo The information about the Redis database connection to etablish
 * @return A pointer to an allocated Redis.
 */
Redis *
Redis_new (
    RedisStartupInfo *info
);


/**
 * @brief Initialize an allocated Redis structure.
 * @param self An allocated Redis to initialize.
 * @param redisInfo The information about the Redis database connection to etablish
 * @return true on success, false otherwise.
 */
bool
Redis_init (
    Redis *self,
    RedisStartupInfo *info
);


/**
 * @brief Initialize an allocated RedisStartupInfo structure.
 * @param self An allocated RedisStartupInfo to initialize.
 * @param hostname The IP address of the Redis server
 * @param port The port of the Redis server
 * @return true on success, false otherwise.
 */
bool
RedisStartupInfo_init (
    RedisStartupInfo *self,
    char *ip,
    int port
);


/**
 * @brief : Connect to the Redis database
 * @param self An allocated Redis instance
 * @return true on success, false otherwise
 */
bool
Redis_connect (
    Redis *self
);



/**
 * @brief Send a command to the redis server AND display it in the console.
 * @param self An allocated Redis instance
 * @param format the format of the command
 * @param ... The values of the command
 * @return A redisReply
 */
redisReply *
Redis_commandDbg (
    Redis *self,
    char * format,
    ...
);


/**
 * @brief Send data to the Redis Server.
 * @param self An allocated Redis instance
 * @param socketSession An allocated socket session
 * @param ... <Key, value> couples. *Must* finishes with Key = NULL.
 * @return true on success, false otherwise
 */
bool
Redis_set (
    Redis *self,
    SocketSession *socketSession,
    ...
);

/**
 * @brief Tells if any element of the redisReply array is NULL
 * @param elements Array of redisReply
 * @param nbElements Number of elements
 * @return The position if any element is NULL, -1 otherwise
 */
size_t
Redis_anyElementIsNull (
    redisReply **elements,
    size_t nbElements
);


/**
 * @brief Dump the elements in the console
 * @param elements Array of redisReply
 * @param nbElements Number of elements
 * @param elementsName The name of elements
 */
void
Redis_printElements (
    redisReply **elements,
    size_t nbElements,
    const char **elementsName
);


/**
 * @brief Free an allocated Redis structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Redis.
 */
void
Redis_destroy (
    Redis **self
);


