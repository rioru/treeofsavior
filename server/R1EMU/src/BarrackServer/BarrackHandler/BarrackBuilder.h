/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file BarrackBuilder.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Commander/Commander.h"
#include "Common/Session/GameSession.h"


// ---------- Defines -------------
typedef enum PacketTypeBarrackNormal {

    BC_NORMAL_COMMANDER_INFO = 0x0,
    BC_NORMAL_COMMANDER_MOVE_OK = 0x2,
    BC_NORMAL_UNKNOWN_1 = 0x4,
    BC_NORMAL_ZONE_TRAFFIC = 0xB,
    BC_NORMAL_PET_INFORMATION = 0x8

}   PacketTypeBarrackNormal;

// ------ Structure declaration -------

// ----------- Functions ------------

/**
 * @brief Send back the information of the account after a successful log in
 */
void
BarrackBuilder_loginOk (
    uint64_t accountId,
    uint8_t *accountLogin,
    GameSessionPrivileges accountPrivileges,
    zmsg_t *replyMsg
);

/**
 * @brief Packet sent when the request for the player to enter in game (AKA connect to a given zone server) is accepted
 */
void
BarrackBuilder_startGameOk (
    uint32_t zoneServerId,
    uint8_t *zoneServerIp,
    uint32_t zoneServerPort,
    uint16_t mapId,
    uint8_t commanderListId,
    uint64_t spriteId,
    uint8_t isSingleMap,
    zmsg_t *replyMsg
);


/**
 * @brief The request for the player to move in the barrack is accepted
 */
void
BarrackBuilder_commanderMoveOk (
    uint64_t accountId,
    uint16_t commanderListId,
    PositionXZ *position,
    zmsg_t *replyMsg
);

/**
 * @brief Register new servers
 */
void
BarrackBuilder_serverEntry (
    uint32_t ipClientNet,
    uint32_t ipVirtualClientNet,
    uint16_t channelPort1,
    uint16_t channelPort2,
    zmsg_t *replyMsg
);


/**
 * @brief Build the list of commanders in the barrack
 */
void
BarrackBuilder_commanderList (
    uint64_t accountId,
    zmsg_t *replyMsg
);


/**
 * @brief Build the list of zone servers
 */
void
BarrackBuilder_zoneTraffics (
    zmsg_t *reply
);


/**
 * @brief Change the name of the barrack
 */
void
BarrackBuilder_barrackNameChange (
    uint8_t *barrackName,
    zmsg_t *replyMsg
);


/**
 * @brief Destroy commanders
 */
void
BarrackBuilder_commanderDestroy (
    uint8_t commanderDestroyMask,
    zmsg_t *replyMsg
);

/**
 * @brief Create a new commander
 */
void
BarrackBuilder_commanderCreate (
    CommanderCreateInfo *commander,
    zmsg_t *replyMsg
);
