/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Router.h
 *
 * @brief Router is the routing component of the network.
 * It accepts packets on a specific port, and routes them to the workers.
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/MySQL/MySQL.h"
#include "Common/Redis/Redis.h"

// ---------- Defines -------------
#define ROUTER_FRONTEND_ENDPOINT           "tcp://%s:%d"
#define ROUTER_BACKEND_ENDPOINT            "inproc://routerWorkersBackend-%d"
#define ROUTER_SUBSCRIBER_ENDPOINT         "inproc://routerWorkersSubscriber-%d-%d"

#define ROUTER_GLOBAL_ENDPOINT             "tcp://%s:%d"

/** Enumeration of all the packets headers that the zone server handles */
// We want to differentiate the headers being received from the the ones being send, but we also want to keep a list
// with uniques header IDs. So, let's declare all the IDs here, and distribute them afterward
typedef enum RouterHeader {
    _ROUTER_WORKER_READY,            // Ready message
    _ROUTER_WORKER_NORMAL,           // Normal message
    _ROUTER_WORKER_MULTICAST,        // Multicast message
    _ROUTER_WORKER_ERROR,            // Error message received from a worker
    _ROUTER_PING,                    // Ping message
    _ROUTER_PONG,                    // Pong message
}   RouterHeader;

// Macro helper for the distribution
#define DECL_ROUTER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the zone server accepts */
typedef enum RouterRecvHeader {
    DECL_ROUTER_HEADER (ROUTER_WORKER_READY),
    DECL_ROUTER_HEADER (ROUTER_WORKER_NORMAL),
    DECL_ROUTER_HEADER (ROUTER_WORKER_MULTICAST),
    DECL_ROUTER_HEADER (ROUTER_WORKER_ERROR),
    DECL_ROUTER_HEADER (ROUTER_PING),
}   RouterRecvHeader;

/** Enumeration of all the packets header that the zone server sends */
typedef enum RouterSendHeader {
    DECL_ROUTER_HEADER (ROUTER_PONG),
}   RouterSendHeader;

#undef DECL_ROUTER_HEADER

// ------ Structure declaration -------
typedef struct Router Router;

// ----------- Functions ------------

/**
 * @brief Allocate a new Router structure.
 * @param serverId The Server ID
 * @param ip The IP of the router
 * @param ports The ports binded by the Router
 * @param portsCount Ports count
 * @param workersCount Number of workers linked to the Router
 * @param globalServerIp The IP of the global server
 * @param globalServerPort The private port exposed to the global server
 * @param sqlInfo The information about the SQL Database
 * @param redisInfo The information about the Redis Database
 * @return A pointer to an allocated Router.
 */
Router *
Router_new (
    uint16_t serverId,
    char *ip,
    int *ports,
    int portsCount,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
);


/**
 * @brief Initialize an allocated Router structure.
 * @param self An allocated Router to initialize.
 * @param serverId The Server ID
 * @param ip The IP of the router
 * @param ports The ports binded by the Router
 * @param portsCount Ports count
 * @param workersCount Number of workers linked to the Router
 * @param globalServerIp The IP of the global server
 * @param globalServerPort The private port exposed to the global server
 * @param sqlInfo The information about the SQL Database
 * @param redisInfo The information about the Redis Database
 * @return true on success, false otherwise.
 */
bool
Router_init (
    Router *self,
    uint16_t serverId,
    char *ip,
    int *ports,
    int portsCount,
    int workersCount,
    char *globalServerIp,
    int globalServerPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo
);


/**
 * @brief Free an allocated Router structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Router.
 */
void
Router_destroy (
    Router **self
);

