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
#include "Common/Server/Worker.h"

// ---------- Defines -------------
typedef enum PacketTypeBarrackNormal {

    BC_NORMAL_COMMANDER_MOVE_OK = 0x2,
    BC_NORMAL_UNKNOWN_1 = 0x4,
    BC_NORMAL_ZONE_TRAFFIC = 0xB,
    BC_NORMAL_PET_INFORMATION = 0x8

}   PacketTypeBarrackNormal;

// ----------- Functions ------------
// Handlers
/** Global table of handlers for the Barrack. It contains only the packets allowed. */
extern const PacketHandler barrackHandlers [PACKET_TYPE_COUNT];
