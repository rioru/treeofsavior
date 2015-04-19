/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GameSession.h
 * @brief GameSession fields definition
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Redis/Redis.h"

// ---------- Defines -------------
#define REDIS_GAME_SESSION_zoneId_str "zoneId"
#define REDIS_GAME_SESSION_familyName_str "familyName"
#define REDIS_GAME_SESSION_currentCommanderName_str "currentCommanderName"
#define REDIS_GAME_SESSION_charactersBarrackCount_str "charactersBarrackCount"
#define REDIS_GAME_SESSION_accountId_str "accountId"
#define REDIS_GAME_SESSION_currentCommanderId_str "currentCommanderId"
#define REDIS_GAME_SESSION_currentPcId_str "currentPcId"
#define REDIS_GAME_SESSION_commander_classId_str "classId"
#define REDIS_GAME_SESSION_commander_jobId_str "jobId"
#define REDIS_GAME_SESSION_commander_gender_str "gender"
#define REDIS_GAME_SESSION_commander_level_str "level"
#define REDIS_GAME_SESSION_commander_head_top_str "head_top"
#define REDIS_GAME_SESSION_commander_head_middle_str "head_middle"
#define REDIS_GAME_SESSION_commander_body_armor_str "body_armor"
#define REDIS_GAME_SESSION_commander_gloves_str "gloves"
#define REDIS_GAME_SESSION_commander_boots_str "boots"
#define REDIS_GAME_SESSION_commander_bracelet_str "bracelet"
#define REDIS_GAME_SESSION_commander_weapon_str "weapon"
#define REDIS_GAME_SESSION_commander_shield_str "shield"
#define REDIS_GAME_SESSION_commander_costume_str "costume"
#define REDIS_GAME_SESSION_commander_leg_armor_str "leg_armor"
#define REDIS_GAME_SESSION_commander_ring_left_str "ring_left"
#define REDIS_GAME_SESSION_commander_ring_right_str "ring_right"
#define REDIS_GAME_SESSION_commander_necklace_str "necklace"
#define REDIS_GAME_SESSION_commander_hairType_str "hairType"
#define REDIS_GAME_SESSION_commander_pcId_str "pcId"
#define REDIS_GAME_SESSION_commander_listPosition_str "listPosition"
#define REDIS_GAME_SESSION_commander_charPosition_str "charPosition"
#define REDIS_GAME_SESSION_commander_mapId_str "mapId"
#define REDIS_GAME_SESSION_commander_currentXP_str "currentXP"
#define REDIS_GAME_SESSION_commander_maxXP_str "maxXP"
#define REDIS_GAME_SESSION_commander_pose_str "pose"
#define REDIS_GAME_SESSION_commander_spriteRelated_str "spriteRelated"
#define REDIS_GAME_SESSION_commander_currentHP_str "currentHP"
#define REDIS_GAME_SESSION_commander_maxHP_str "maxHP"
#define REDIS_GAME_SESSION_commander_currentSP_str "currentSP"
#define REDIS_GAME_SESSION_commander_maxSP_str "maxSP"

enum RedisGameSessionFields {
	REDIS_GAME_SESSION_zoneId,
	REDIS_GAME_SESSION_familyName,
	REDIS_GAME_SESSION_currentCommanderName,
	REDIS_GAME_SESSION_charactersBarrackCount,
	REDIS_GAME_SESSION_accountId,
	REDIS_GAME_SESSION_currentCommanderId,
	REDIS_GAME_SESSION_currentPcId,
	REDIS_GAME_SESSION_commander_classId,
	REDIS_GAME_SESSION_commander_jobId,
	REDIS_GAME_SESSION_commander_gender,
	REDIS_GAME_SESSION_commander_level,
	REDIS_GAME_SESSION_commander_head_top,
	REDIS_GAME_SESSION_commander_head_middle,
	REDIS_GAME_SESSION_commander_body_armor,
	REDIS_GAME_SESSION_commander_gloves,
	REDIS_GAME_SESSION_commander_boots,
	REDIS_GAME_SESSION_commander_bracelet,
	REDIS_GAME_SESSION_commander_weapon,
	REDIS_GAME_SESSION_commander_shield,
	REDIS_GAME_SESSION_commander_costume,
	REDIS_GAME_SESSION_commander_leg_armor,
	REDIS_GAME_SESSION_commander_ring_left,
	REDIS_GAME_SESSION_commander_ring_right,
	REDIS_GAME_SESSION_commander_necklace,
	REDIS_GAME_SESSION_commander_hairType,
	REDIS_GAME_SESSION_commander_pcId,
	REDIS_GAME_SESSION_commander_listPosition,
	REDIS_GAME_SESSION_commander_charPosition,
	REDIS_GAME_SESSION_commander_mapId,
	REDIS_GAME_SESSION_commander_currentXP,
	REDIS_GAME_SESSION_commander_maxXP,
	REDIS_GAME_SESSION_commander_pose,
	REDIS_GAME_SESSION_commander_spriteRelated,
	REDIS_GAME_SESSION_commander_currentHP,
	REDIS_GAME_SESSION_commander_maxHP,
	REDIS_GAME_SESSION_commander_currentSP,
	REDIS_GAME_SESSION_commander_maxSP,
	REDIS_GAME_SESSION_COUNT
};

// ------ Structure declaration -------


// ------ Global variables declaration -------
extern const char *redisGameSessionsStr [];


// ----------- Functions ------------

/**
 * @brief Get the GameSession associated with the SocketSession
 * @param self An allocated Redis
 * @param socketSession A socket session
 * @param[out] session The game Session
 * @return
 */
bool
Redis_getGameSession (
    Redis *self,
    SocketSession *socketSession,
    GameSession *session
);


/**
 * @brief Save an entire GameSession to the Redis server.
 * @param self An allocated Redis instance
 * @param session An allocated session to refresh
 * @return true on success, false otherwise
 */
bool
Redis_updateGameSession (
    Redis *self,
    GameSession *session
);