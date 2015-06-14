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
#include "Common/Session/Session.h"

// ---------- Defines -------------
#define REDIS_GAME_SESSION_socketId_str                  "socketId"
#define REDIS_GAME_SESSION_routerId_str                  "routerId"
#define REDIS_GAME_SESSION_charactersBarrackCount_str    "charactersBarrackCount"
#define REDIS_GAME_SESSION_accountLogin_str              "accountLogin"
#define REDIS_GAME_SESSION_commander_commanderName_str   "commanderName"
#define REDIS_GAME_SESSION_commander_familyName_str      "familyName"
#define REDIS_GAME_SESSION_commander_accountId_str       "accountId"
#define REDIS_GAME_SESSION_commander_classId_str         "classId"
#define REDIS_GAME_SESSION_commander_jobId_str           "jobId"
#define REDIS_GAME_SESSION_commander_gender_str          "gender"
#define REDIS_GAME_SESSION_commander_level_str           "level"
#define REDIS_GAME_SESSION_commander_head_top_str        "head_top"
#define REDIS_GAME_SESSION_commander_head_middle_str     "head_middle"
#define REDIS_GAME_SESSION_commander_itemUnk1_str        "itemUnk1"
#define REDIS_GAME_SESSION_commander_body_armor_str      "body_armor"
#define REDIS_GAME_SESSION_commander_gloves_str          "gloves"
#define REDIS_GAME_SESSION_commander_boots_str           "boots"
#define REDIS_GAME_SESSION_commander_itemUnk2_str        "itemUnk2"
#define REDIS_GAME_SESSION_commander_bracelet_str        "bracelet"
#define REDIS_GAME_SESSION_commander_weapon_str          "weapon"
#define REDIS_GAME_SESSION_commander_shield_str          "shield"
#define REDIS_GAME_SESSION_commander_costume_str         "costume"
#define REDIS_GAME_SESSION_commander_itemUnk3_str        "itemUnk3"
#define REDIS_GAME_SESSION_commander_itemUnk4_str        "itemUnk4"
#define REDIS_GAME_SESSION_commander_itemUnk5_str        "itemUnk5"
#define REDIS_GAME_SESSION_commander_leg_armor_str       "leg_armor"
#define REDIS_GAME_SESSION_commander_itemUnk6_str        "itemUnk6"
#define REDIS_GAME_SESSION_commander_itemUnk7_str        "itemUnk7"
#define REDIS_GAME_SESSION_commander_ring_left_str       "ring_left"
#define REDIS_GAME_SESSION_commander_ring_right_str      "ring_right"
#define REDIS_GAME_SESSION_commander_necklace_str        "necklace"
#define REDIS_GAME_SESSION_commander_hairType_str        "hairType"
#define REDIS_GAME_SESSION_commander_pcId_str            "pcId"
#define REDIS_GAME_SESSION_commander_unk13_str           "unk13"
#define REDIS_GAME_SESSION_commander_charPosition_str    "charPosition"
#define REDIS_GAME_SESSION_commander_mapId_str           "mapId"
#define REDIS_GAME_SESSION_commander_currentXP_str       "currentXP"
#define REDIS_GAME_SESSION_commander_maxXP_str           "maxXP"
#define REDIS_GAME_SESSION_commander_spriteId_str        "pose"
#define REDIS_GAME_SESSION_commander_spriteIdRelated_str "spriteIdRelated"
#define REDIS_GAME_SESSION_commander_currentHP_str       "currentHP"
#define REDIS_GAME_SESSION_commander_maxHP_str           "maxHP"
#define REDIS_GAME_SESSION_commander_currentSP_str       "currentSP"
#define REDIS_GAME_SESSION_commander_maxSP_str           "maxSP"
#define REDIS_GAME_SESSION_commander_cPosX_str           "cPosX"
#define REDIS_GAME_SESSION_commander_cPosZ_str           "cPosZ"

enum RedisGameSessionFields {
	REDIS_GAME_SESSION_socketId,
	REDIS_GAME_SESSION_routerId,
	REDIS_GAME_SESSION_charactersBarrackCount,
	REDIS_GAME_SESSION_accountLogin,
	REDIS_GAME_SESSION_commander_commanderName,
	REDIS_GAME_SESSION_commander_familyName,
	REDIS_GAME_SESSION_commander_accountId,
	REDIS_GAME_SESSION_commander_classId,
	REDIS_GAME_SESSION_commander_jobId,
	REDIS_GAME_SESSION_commander_gender,
	REDIS_GAME_SESSION_commander_level,
	REDIS_GAME_SESSION_commander_head_top,
	REDIS_GAME_SESSION_commander_head_middle,
	REDIS_GAME_SESSION_commander_itemUnk1,
	REDIS_GAME_SESSION_commander_body_armor,
	REDIS_GAME_SESSION_commander_gloves,
	REDIS_GAME_SESSION_commander_boots,
	REDIS_GAME_SESSION_commander_itemUnk2,
	REDIS_GAME_SESSION_commander_bracelet,
	REDIS_GAME_SESSION_commander_weapon,
	REDIS_GAME_SESSION_commander_shield,
	REDIS_GAME_SESSION_commander_costume,
	REDIS_GAME_SESSION_commander_itemUnk3,
	REDIS_GAME_SESSION_commander_itemUnk4,
	REDIS_GAME_SESSION_commander_itemUnk5,
	REDIS_GAME_SESSION_commander_leg_armor,
	REDIS_GAME_SESSION_commander_itemUnk6,
	REDIS_GAME_SESSION_commander_itemUnk7,
	REDIS_GAME_SESSION_commander_ring_left,
	REDIS_GAME_SESSION_commander_ring_right,
	REDIS_GAME_SESSION_commander_necklace,
	REDIS_GAME_SESSION_commander_hairType,
	REDIS_GAME_SESSION_commander_pcId,
	REDIS_GAME_SESSION_commander_unk13,
	REDIS_GAME_SESSION_commander_charPosition,
	REDIS_GAME_SESSION_commander_mapId,
	REDIS_GAME_SESSION_commander_currentXP,
	REDIS_GAME_SESSION_commander_maxXP,
	REDIS_GAME_SESSION_commander_spriteId,
	REDIS_GAME_SESSION_commander_spriteIdRelated,
	REDIS_GAME_SESSION_commander_currentHP,
	REDIS_GAME_SESSION_commander_maxHP,
	REDIS_GAME_SESSION_commander_currentSP,
	REDIS_GAME_SESSION_commander_maxSP,
	REDIS_GAME_SESSION_commander_cPosX,
	REDIS_GAME_SESSION_commander_cPosZ,
	REDIS_GAME_SESSION_COUNT
};

// ------ Structure declaration -------
typedef struct {
    uint16_t routerId;
    uint16_t mapId;
    uint64_t accountId;
} RedisGameSessionKey;


// ------ Global variables declaration -------
extern const char *redisGameSessionsStr [];


// ----------- Functions ------------

/**
 * @brief Get the GameSession associated with the SocketSession
 * @param self An allocated Redis
 * @param key The GameSession key
 * @param[out] gameSession The output gameSession
 * @return
 */
bool
Redis_getGameSession (
    Redis *self,
    RedisGameSessionKey *key,
    GameSession *gameSession
);


/**
 * @brief Save an entire GameSession to the Redis server.
 * @param self An allocated Redis instance
 * @param key The GameSession key
 * @param socketId The socketId linked with the Game Session
 * @param gameSession The Game Session to save
 * @return true on success, false otherwise
 */
bool
Redis_updateGameSession (
    Redis *self,
    RedisGameSessionKey *key,
    unsigned char *socketId,
    GameSession *gameSession
);


/**
 * @brief Flush a GameSession
 * @param self An allocated Redis instance
 * @param key The GameSession key
 * @return true on success, false otherwise
 */
bool
Redis_flushGameSession (
    Redis *self,
    RedisGameSessionKey *key
);

/**
 * @brief Move a game session from a server to another
 * @param self An initialized Redis instance
 * @param from The source server
 * @param to The destination server
 */
bool
Redis_moveGameSession (
    Redis *self,
    RedisGameSessionKey *from,
    RedisGameSessionKey *to
);

/**
 * @brief Return a list of clients into the area of a given position
 * @param self An allocated Worker
 * @param serverId The serverId concerned
 * @param mapId The mapId of the position
 * @param The 2D position of the center of the circle
 * @param range Radius of the circle
 * @return a zlist_t of identity keys
 */
zlist_t *
Redis_getClientsWithinDistance (
    Redis *self,
    uint16_t serverId, uint16_t mapId,
    Position2D *position,
    float range
);
