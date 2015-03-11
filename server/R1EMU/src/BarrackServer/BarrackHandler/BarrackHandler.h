/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackHandler.h
 * @brief BarrackHandler is a module containing all the handlers for the Barrack system
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Packet/PacketType.h"
#include "Common/ClientSession/ClientSession.h"

// ---------- Defines -------------
#define BARRACK_HANDLER_MAX_ID      CZ_LOGOUT
#define BARRACK_HANDLER_ARRAY_SIZE  (BARRACK_HANDLER_MAX_ID + 1)


// ------ Structure declaration -------
/** Return state of the barrack handlers. */
typedef enum BarrackHandlerState {
    BARRACK_HANDLER_ERROR          = -1,
    BARRACK_HANDLER_OK             = 0,
    BARRACK_HANDLER_UPDATE_SESSION = 1,
}   BarrackHandlerState;

/** @return BarrackHandlerState (see above) */
typedef BarrackHandlerState (*BarrackHandlerFunction) (
    /** Session of the current player */
    ClientSession *session,
    /** Packet sent by the client */
    unsigned char *packet,
    /** Size of the packet sent by the client */
    size_t packetSize,
    /** Reply buffer. If you need to send multiple replies, add multiple frames to this zmsg_t */
    zmsg_t *reply
);


typedef struct _BarrackHandlers {
    /** Function handler */
    BarrackHandlerFunction handler;
    /** The packet name  */
    char *packetName;
}   BarrackHandlers;



// ----------- Functions ------------
// Handlers
/** Global table of handlers for the Barrack. It contains only the packets allowed. */
extern const BarrackHandlers barrackHandlers [BARRACK_HANDLER_ARRAY_SIZE];
