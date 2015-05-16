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
 * @brief ZoneHandler is a module containing all the handlers for the Zone system
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Packet/PacketType.h"
#include "Common/Server/Worker.h"

// ---------- Defines -------------
typedef enum PacketTypeZoneNormal {

    ZC_NORMAL_UNKNOWN_1 = 0x92,
    ZC_NORMAL_UNKNOWN_2 = 0xC4,
    ZC_NORMAL_UNKNOWN_3 = 0xD2,
    ZC_NORMAL_UNKNOWN_4 = 0xDF,
    ZC_NORMAL_UNKNOWN_5 = 0x151,
    ZC_NORMAL_UNKNOWN_6 = 0xC9,
    ZC_NORMAL_UNKNOWN_7 = 0xE1,

}   PacketTypeZoneNormal;

// ------ Structure declaration -------

// ----------- Functions ------------
// Handlers
/** Global table of handlers for the Barrack. It contains only the packets allowed. */
extern const PacketHandler zoneHandlers [PACKET_TYPE_COUNT];
