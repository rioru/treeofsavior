/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file EventServer.h
 * @brief EventServer broadcast the game events to the concerned clients
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Session/Session.h"
#include "Common/Server/Router.h"

// ---------- Defines -------------
#define EVENT_SERVER_EXECUTABLE_NAME             "EventServer"
#define EVENT_SERVER_SUBSCRIBER_ENDPOINT         "inproc://eventServerWorkersSubscriber-%d-%d"

/** Enumeration of all the packets headers that the EventServer handles */
// We want to differentiate the headers being received from the the ones being send, but we also want to keep a list
// with uniques header IDs. So, let's declare all the IDs here, and distribute them afterward
typedef enum EventServerHeader {
    _EVENT_SERVER_PING,                    // Ping message
    _EVENT_SERVER_PONG,                    // Pong message
    _EVENT_SERVER_EVENT,                    // Pong message
}   EventServerHeader;

// Macro helper for the distribution
#define DECL_EVENT_SERVER_HEADER(x) \
    x = _##x

/** Enumeration of all the packets header that the EventServer accepts */
typedef enum EventServerRecvHeader {
    DECL_EVENT_SERVER_HEADER (EVENT_SERVER_PING),
    DECL_EVENT_SERVER_HEADER (EVENT_SERVER_EVENT),
}   EventServerRecvHeader;

/** Enumeration of all the packets header that the EventServer sends */
typedef enum EventServerSendHeader {
    DECL_EVENT_SERVER_HEADER (EVENT_SERVER_PONG),
}   EventServerSendHeader;

#undef DECL_EVENT_SERVER_HEADER

// ------ Structure declaration -------
typedef struct {
    uint16_t routerId;
    uint16_t workersCount;

    RedisStartupInfo redisInfo;

} EventServerStartupInfo;

// EventServer is opaque
typedef struct EventServer EventServer;

// ----------- Functions ------------

/**
 * @brief Allocate a new EventServer structure.
 * @param info An allocated EventServerStartupInfo containing the information for starting up the event server.
 * @return A pointer to an allocated EventServer.
 */
EventServer *
EventServer_new (
    EventServerStartupInfo *info
);


/**
 * @brief Initialize an allocated EventServer structure.
 * @param self An allocated EventServer to initialize.
 * @param info An allocated EventServerStartupInfo containing the information for starting up the event server.
 * @return true on success, false otherwise.
 */
bool
EventServer_init (
    EventServer *self,
    EventServerStartupInfo *info
);


/**
 * @brief Free an allocated EventServer structure and nullify the content of the pointer.
 * @param self A pointer to an allocated EventServer.
 */
void
EventServer_destroy (
    EventServer **self
);


/**
 * @brief Initialize an allocated EventServerStartupInfo structure.
 * @param self An allocated EventServerStartupInfo to initialize.
 * @param routerId The routerID
 * @param workersCount The workers count
 * @return true on success, false otherwise.
 */
bool
EventServerStartupInfo_init (
    EventServerStartupInfo *self,
    uint16_t routerId,
    uint16_t workersCount,
    char *redisHostname,
    int redisPort
);


/**
 * @brief Return a list of clients into an area
 * @param self An allocated EventServer
 * @param session The session of the client
 * @param center The position of the center of the circle
 * @param range Radius of the circle
 * @param selfInclude if false, don't include current client in the results. Do it otherwise.
 * @return a zlist_t of identity keys
 */
zlist_t *
EventServer_getClientsWithinRange (
    EventServer *self,
    Session *session,
    PositionXZ *center,
    float range,
    bool selfInclude
);


/**
 * @brief Send the same packet to multiple clients
 * @param self An allocated EventServer
 * @param clients A list of clients socket identity
 * @param packet The packet to send
 * @param packetLen the length of the packet
 * @return true on success, false otherwise
 */
bool
EventServer_sendToClients (
    EventServer *self,
    zlist_t *clients,
    uint8_t *packet,
    size_t packetLen
);


/**
 * @brief Start the Zone Server main loop.
 * @param self An allocated EventServer
 * @return true on success, false otherwise
 */
bool
EventServer_start (
    EventServer *self
);
