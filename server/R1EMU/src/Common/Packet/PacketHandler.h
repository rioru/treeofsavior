/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file PacketHandler.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Session/SocketSession.h"
#include "Common/Session/GameSession.h"

// ---------- Defines -------------


// ------ Structure declaration -------

// ------ Structure declaration -------
/** Return state of the packet handlers. */
typedef enum {
    PACKET_HANDLER_ERROR          = -1,
    PACKET_HANDLER_OK             = 0,
    PACKET_HANDLER_UPDATE_SESSION = 1,
    PACKET_HANDLER_DELETE_SESSION = 2
}   PacketHandlerState;

/** @return PacketHandlerState (see above) */
typedef PacketHandlerState (*PacketHandlerFunction) (
    /** Session of the current player */
    GameSession *session,
    /** Packet sent by the client */
    unsigned char *packet,
    /** Size of the packet sent by the client */
    size_t packetSize,
    /** Reply buffer. If you need to send multiple replies, add multiple frames to this zmsg_t */
    zmsg_t *reply,
    /** Generic pointer to any data */
    void *arg
);


typedef struct {
    /** Function handler */
    PacketHandlerFunction handler;
    /** The packet name  */
    char *packetName;

}   PacketHandler;


// ----------- Functions ------------

/**
 * @brief Build a reply based on the packet handler
 * @param[in] handlers The packet handlers
 * @param[in] handlersCount The number of handlers in the handlers array
 * @param[in] session The game session associated with the packet
 * @param[in] packet The packet sent by the client
 * @param[in] packetSize The size of the packet
 * @param[out] reply The message for the reply. Each frame contains a reply to send in different packets.
 * @param[in] arg A generic pointer to any data
 * @return PacketHandlerState
 */
PacketHandlerState
PacketHandler_buildReply (
    const PacketHandler *handlers,
    size_t handlersCount,
    GameSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
);
