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
#include "Common/Commander/Commander.h"

// ---------- Defines -------------
typedef enum PacketTypeZoneNormal {

    ZC_NORMAL_UNKNOWN_1 = 0x92,
    ZC_NORMAL_UNKNOWN_2 = 0xC4,
    ZC_NORMAL_UNKNOWN_3 = 0xD2,
    ZC_NORMAL_UNKNOWN_4 = 0xDF,
    ZC_NORMAL_UNKNOWN_5 = 0x151,
    ZC_NORMAL_UNKNOWN_6 = 0xC9,
    ZC_NORMAL_UNKNOWN_7 = 0xE1,
    ZC_NORMAL_UNKNOWN_8 = 0x54,
    ZC_NORMAL_UNKNOWN_9 = 0x124

}   PacketTypeZoneNormal;

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


/**
 * @brief Inform that a skill is ready
 * @param targetPcId Actor of the skill
 * @param skillId Skill ID
 * @param pos1
 * @param pos2
 */
void
ZoneBuilder_skillReady (
    uint32_t targetPcId,
    uint32_t skillId,
    PositionXYZ *pos1,
    PositionXYZ *pos2,
    zmsg_t *replyMsg
);


/**
 * @brief Play an animation
 */
void
ZoneBuilder_playAni (
    zmsg_t *replyMsg
);


/**
 * @brief Cast a given skill at a given position from a given PC ID
 */
void
ZoneBuilder_skillCast (
    uint32_t targetPcId,
    uint32_t skillId,
    PositionXYZ *position1,
    PositionXYZ *position2,
    zmsg_t *replyMsg
);


/**
 * @brief Play the cast animation of a given skill at a given position from a given PC ID
 */
void
ZoneBuilder_playSkillCastAni (
    uint32_t targetPcId,
    PositionXYZ *position,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk8 (
    uint32_t targetPcId,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk9 (
    uint32_t targetPcId,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_partyList (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_partyInfo (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_jobPts (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk6 (
    char *commanderName,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk7 (
    uint64_t accountId,
    uint32_t targetPcId,
    char *familyName,
    char *commanderName,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_faction (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_enterMonster (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_buffList (
    uint32_t targetPcId,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_enterPc (
    CommanderInfo *commander,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_skillAdd (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_loginTime (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_stamina (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_objectProperty (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_addonMsg (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk5 (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk4 (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk3 (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk2 (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_normalUnk1 (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_quickSlotList (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_itemEquipList (
    zmsg_t *replyMsg
);


/**
 * @brief Send information about Jobs
 */
void
ZoneBuilder_startInfo (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_abilityList (
    uint32_t targetPcId,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_skillList (
    uint32_t targetPcId,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_guestPageMap (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_myPageMap (
    zmsg_t *replyMsg
);


/**
 * @brief Send information about the UI
 */
void
ZoneBuilder_uiInfoList (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_npcStateList (
    zmsg_t *replyMsg
);


/**
 * @brief Alert the client that its own commander arrived in the game at a given position
 */
void
ZoneBuilder_MyPCEnter (
    PositionXYZ *position,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_chatMacroList (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_helpList (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_achievePointList (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_skillmapList (
    zmsg_t *replyMsg
);


/**
 * @brief Send information about item inventory list
 */
void
ZoneBuilder_itemInventoryList (
    zmsg_t *replyMsg
);


/**
 * @brief Send a target commander movement speed
 */
void
ZoneBuilder_moveSpeed (
    uint32_t targetPcId,
    float movementSpeed,
    zmsg_t *replyMsg
);


/**
 * @brief Connect to the zone server
 */
void
ZoneBuilder_connect (
    uint8_t gameMode,
    uint8_t accountPrivileges,
    uint32_t targetPcId,
    CommanderInfo *commander,
    zmsg_t *replyMsg
);


/**
 * Makes a target PC jump at a given height
 */
void
ZoneBuilder_jump (
    uint32_t targetPcId,
    float height,
    zmsg_t *replyMsg
);


/**
 * @brief @unknown Contains information about quest position ?
 */
void
ZoneBuilder_sessionObjects (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_optionList (
    zmsg_t *replyMsg
);


/**
 * @brief @unknown
 */
void
ZoneBuilder_cooldownList (
    zmsg_t *replyMsg
);


/**
 * @brief Set the position of a commander
 */
void
ZoneBuilder_setPos (
    uint32_t targetPcId,
    PositionXYZ *position,
    zmsg_t *replyMsg
);


/**
 * @brief Move a given commander to a given position
 */
void
ZoneBuilder_moveDir (
    PositionXYZ *position,
    float dirX, float dirZ,
    uint32_t targetPcId,
    float timestamp,
    zmsg_t *replyMsg
);

/**
 * @brief After the loading screen, start the game for the client
 */
void
ZoneBuilder_startGame (
    float timeMultiplier,
    float serverAppTimeOffset,
    float globalAppTimeOffset,
    double serverDateTime,
    zmsg_t *replyMsg
);
