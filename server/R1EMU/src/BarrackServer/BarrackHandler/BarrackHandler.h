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
#include "Packet/PacketType/PacketType.h"
#include "SessionServer/ClientSession/ClientSession.h"

// ---------- Defines -------------
#define BARRACK_HANDLER_MAX_ID      CB_DELETE_PET
#define BARRACK_HANDLER_ARRAY_SIZE  (BARRACK_HANDLER_MAX_ID + 1)


// ------ Structure declaration -------
typedef bool (*BarrackHandlerFunction) (
    ClientSession *session,
    unsigned char *packet,
    size_t dataSize,
    zmsg_t *reply
);

typedef struct _BarrackHandlers {
    BarrackHandlerFunction handler;
    char *packetName;
}   BarrackHandlers;



// ----------- Functions ------------
// Handlers
extern const BarrackHandlers barrackHandlers [BARRACK_HANDLER_ARRAY_SIZE];
