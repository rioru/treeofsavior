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
    Position3D *pos1,
    Position3D *pos2,
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
    Position3D *position1,
    Position3D *position2,
    zmsg_t *replyMsg
);

/**
 * @brief Play the cast animation of a given skill at a given position from a given PC ID
 */
void
ZoneBuilder_playSkillCastAni (
    uint32_t targetPcId,
    Position3D *position,
    zmsg_t *replyMsg
);
