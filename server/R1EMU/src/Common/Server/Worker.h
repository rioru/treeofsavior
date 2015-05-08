/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Worker.h
 * @brief Worker is the component in the network architecture that processes the packet received from the Router.
 *
 * Here are the general steps that a worker always do :
 * - It receives a request from the Router
 * - It extracts the entity ID from the packet
 * - It searches for the session associated with the entity ID in the Redis server
 * - If it exists, return the session associated with the entity ID
 * - If it doesn't, create the session
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/MySQL/MySQL.h"
#include "Common/Redis/Redis.h"
#include "Common/Session/Session.h"
#include "Common/Packet/PacketHandler.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/**
 * @brief Worker is the minimal structure that a worker needs to work
 *        Everything else goes through the messaging system
 */
struct Worker
{
    // === Network ===
    /** The worker ID */
    uint16_t workerId;

    /** The Server ID having authority on this worker */
    uint16_t serverId;

    /** The frontend port of the Router */
    int frontendPort;

    /** The publisher socket to send asynchronous messages to the Router */
    zsock_t *publisher;

    /** IP of the global server */
    char *globalServerIp;

    /** Private port with the global server */
    int globalServerPort;

    // === Database ===
    /** The MySQL session */
    MySQL *sqlConn;

    /** The Redis session */
    Redis *redis;

    // === Handlers ===
    /** Array of packet handlers */
    PacketHandler *packetHandlers;

    /** Packet handlers entries count */
    int packetHandlersCount;
};

typedef struct Worker Worker;

// ----------- Functions ------------

/**
 * @brief Allocate a new Worker structure.
 * @param workerId The worker ID.
 * @param serverId The Server ID
 * @param globalServerIp The IP of the global server
 * @param globalServerPort The private port exposed to the global server
 * @param sqlInfo The information about the SQL Database
 * @param redisInfo The information about the Redis Database
 * @param packetHandlers Array of packet handlers
 * @param packetHandlersCount Packet handlers entries count
 * @return A pointer to an allocated Worker.
 */
Worker *
Worker_new (
    uint16_t workerId,
    uint16_t serverId,
    char *globalServerIp,
    int globalServerPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo,
    PacketHandler *packetHandlers,
    int packetHandlersCount
);


/**
 * @brief Initialize an allocated Worker structure.
 * @param self An allocated Worker to initialize.
 * @param workerId The worker ID.
 * @param serverId The Server ID
 * @param globalServerIp The IP of the global server
 * @param globalServerPort The private port exposed to the global server
 * @param sqlInfo The information about the SQL Database
 * @param redisInfo The information about the Redis Database
 * @param packetHandlers Array of packet handlers
 * @param packetHandlersCount Packet handlers entries count
 * @return true on success, false otherwise.
 */
bool
Worker_init (
    Worker *self,
    uint16_t workerId,
    uint16_t serverId,
    char *globalServerIp,
    int globalServerPort,
    MySQLInfo *sqlInfo,
    RedisInfo *redisInfo,
    PacketHandler *packetHandlers,
    int packetHandlersCount
);


/**
 * @brief Worker routine for the zone server.
 * @param arg A Worker
 * @return Always NULL
 */
void *
Worker_worker (
    void *arg
);


/**
 * @brief Return a list of clients into an area
 * @param self An allocated Worker
 * @param session The session of the client
 * @param x, y, z The position of the middle of the circle
 * @param range Radius of the circle
 * @return a zlist_t of identity keys
 */
zlist_t *
Worker_getClientsWithinDistance (
    Worker *self,
    Session *session,
    float x, float y, float z,
    float range
);


/**
 * @brief Send the same packet to multiple clients
 * @param self An allocated Worker
 * @param clients A list of clients socket identity
 * @param packet The packet to send
 * @param packetLen the length of the packet
 * @return true on success, false otherwise
 */
bool
Worker_sendToClients (
    Worker *self,
    zlist_t *clients,
    unsigned char *packet,
    size_t packetLen
);

/**
 * @brief Free an allocated Worker structure and nullify the content of the pointer.
 * @param self A pointer to an allocated Worker.
 */
void
Worker_destroy (
    Worker **self
);

