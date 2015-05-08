/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GameSession.h
 * @brief GameSession fields definition
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Redis/Redis.h"

// ---------- Defines -------------
#define REDIS_SOCKET_SESSION_accountId_str "accountId"
#define REDIS_SOCKET_SESSION_routerId_str "routerId"
#define REDIS_SOCKET_SESSION_mapId_str "mapId"
#define REDIS_SOCKET_SESSION_authenticated_str "authenticated"

enum RedisSocketSessionFields {
	REDIS_SOCKET_SESSION_accountId,
	REDIS_SOCKET_SESSION_routerId,
	REDIS_SOCKET_SESSION_mapId,
	REDIS_SOCKET_SESSION_authenticated,
	REDIS_SOCKET_SESSION_COUNT
};

// ------ Structure declaration -------


// ------ Global variables declaration -------
extern const char *redisSocketSessionsStr [];


// ----------- Functions ------------
/**
 * @brief Get the SocketSession associated with the Socket ID
 * @param self An allocated Redis
 * @param serverId The zone ID requesting the socket session
 * @param socketIdKey A socket ID key
 * @param[out] socketSession The socket Session
 * @return
 */
bool
Redis_getSocketSession (
    Redis *self,
    int serverId,
    char *socketIdKey,
    SocketSession *socketSession
);


/**
 * @brief Save an entire GameSession to the Redis server.
 * @param self An allocated Redis instance
 * @param[out] socketSession An allocated socket session to refresh
 * @return true on success, false otherwise
 */
bool
Redis_updateSocketSession (
    Redis *self,
    SocketSession *socketSession
);
