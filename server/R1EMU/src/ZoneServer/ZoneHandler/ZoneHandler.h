/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ZoneHandler.h
 * @brief ZoneHandler is a module containing all the handlers for the Barrack system
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Packet/PacketType.h"
#include "Common/ClientSession/ClientSession.h"

// ---------- Defines -------------
#define ZONE_HANDLER_MAX_ID      CZ_HACKSHIELD_BUFFER
#define ZONE_HANDLER_ARRAY_SIZE  (ZONE_HANDLER_MAX_ID + 1)


// ------ Structure declaration -------
/** Return state of the barrack handlers. */
typedef enum ZoneHandlerState {
    ZONE_HANDLER_ERROR          = -1,
    ZONE_HANDLER_OK             = 0,
    ZONE_HANDLER_UPDATE_SESSION = 1,
}   ZoneHandlerState;

/** @return ZoneHandlerState (see above) */
typedef ZoneHandlerState (*ZoneHandlerFunction) (
    /** Session of the current player */
    ClientSession *session,
    /** Packet sent by the client */
    unsigned char *packet,
    /** Size of the packet sent by the client */
    size_t packetSize,
    /** Reply buffer. If you need to send multiple replies, add multiple frames to this zmsg_t */
    zmsg_t *reply
);


typedef struct _ZoneHandlers {
    /** Function handler */
    ZoneHandlerFunction handler;
    /** The packet name  */
    char *packetName;
}   ZoneHandlers;



// ----------- Functions ------------
// Handlers
/** Global table of handlers for the Barrack. It contains only the packets allowed. */
extern const ZoneHandlers zoneHandlers [ZONE_HANDLER_ARRAY_SIZE];
