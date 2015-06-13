/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ZoneBuilder.h
 * @brief ZoneBuilders build packets generically based on given arguments.
 *        They are called from ZoneHandlers.
 *        They don't communicate with other entities on the network.
 *        They never fail.
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Server/Worker.h"

// ---------- Defines -------------


// ------ Structure declaration -------

// ----------- Functions ------------
/**
 * @brief Sit a target PC
 * @param targetPcId The PC ID of the target entity who sit
 */
void ZoneBuilder_restSit (
    uint32_t targetPcId,
    zmsg_t *replyMsg
);
