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
// Game session
#define REDIS_GAME_SESSION_socketId_str                  "socketId"
#define REDIS_GAME_SESSION_pcId_str                      "pcId"
#define REDIS_GAME_SESSION_mapId_str                     "mapId"
#define REDIS_GAME_SESSION_charactersBarrackCount_str    "charactersBarrackCount"
#define REDIS_GAME_SESSION_accountLogin_str              "accountLogin"

// Commander
#define REDIS_GAME_SESSION_commander_commanderName_str   "commanderName"
#define REDIS_GAME_SESSION_commander_familyName_str      "familyName"
#define REDIS_GAME_SESSION_commander_unk1_str            "unk1"
#define REDIS_GAME_SESSION_commander_unk2_str            "unk2"
#define REDIS_GAME_SESSION_commander_unk3_str            "unk3"
#define REDIS_GAME_SESSION_commander_accountId_str       "accountId"
#define REDIS_GAME_SESSION_commander_classId_str         "classId"
#define REDIS_GAME_SESSION_commander_unk4_str            "unk4"
#define REDIS_GAME_SESSION_commander_jobId_str           "jobId"
#define REDIS_GAME_SESSION_commander_gender_str          "gender"
#define REDIS_GAME_SESSION_commander_unk5_str            "unk5"
#define REDIS_GAME_SESSION_commander_level_str           "level"
#define REDIS_GAME_SESSION_commander_hairType_str        "hairType"
#define REDIS_GAME_SESSION_commander_unk6_str            "unk6"

// CommanderEquipment
#define REDIS_GAME_SESSION_equipment_head_top_str        "head_top"
#define REDIS_GAME_SESSION_equipment_head_middle_str     "head_middle"
#define REDIS_GAME_SESSION_equipment_itemUnk1_str        "itemUnk1"
#define REDIS_GAME_SESSION_equipment_body_armor_str      "body_armor"
#define REDIS_GAME_SESSION_equipment_gloves_str          "gloves"
#define REDIS_GAME_SESSION_equipment_boots_str           "boots"
#define REDIS_GAME_SESSION_equipment_itemUnk2_str        "itemUnk2"
#define REDIS_GAME_SESSION_equipment_bracelet_str        "bracelet"
#define REDIS_GAME_SESSION_equipment_weapon_str          "weapon"
#define REDIS_GAME_SESSION_equipment_shield_str          "shield"
#define REDIS_GAME_SESSION_equipment_costume_str         "costume"
#define REDIS_GAME_SESSION_equipment_itemUnk3_str        "itemUnk3"
#define REDIS_GAME_SESSION_equipment_itemUnk4_str        "itemUnk4"
#define REDIS_GAME_SESSION_equipment_itemUnk5_str        "itemUnk5"
#define REDIS_GAME_SESSION_equipment_leg_armor_str       "leg_armor"
#define REDIS_GAME_SESSION_equipment_itemUnk6_str        "itemUnk6"
#define REDIS_GAME_SESSION_equipment_itemUnk7_str        "itemUnk7"
#define REDIS_GAME_SESSION_equipment_ring_left_str       "ring_left"
#define REDIS_GAME_SESSION_equipment_ring_right_str      "ring_right"
#define REDIS_GAME_SESSION_equipment_necklace_str        "necklace"

// CommanderInfo
#define REDIS_GAME_SESSION_info_posX_str            "posX"
#define REDIS_GAME_SESSION_info_posY_str            "posY"
#define REDIS_GAME_SESSION_info_posZ_str            "posZ"
#define REDIS_GAME_SESSION_info_currentXP_str       "currentXP"
#define REDIS_GAME_SESSION_info_maxXP_str           "maxXP"
#define REDIS_GAME_SESSION_info_commanderId_str     "commanderId"
#define REDIS_GAME_SESSION_info_zoneServerId_str        "zoneServerId"
#define REDIS_GAME_SESSION_info_zoneServerId2_str       "zoneServerId2"
#define REDIS_GAME_SESSION_info_currentHP_str       "currentHP"
#define REDIS_GAME_SESSION_info_maxHP_str           "maxHP"
#define REDIS_GAME_SESSION_info_currentSP_str       "currentSP"
#define REDIS_GAME_SESSION_info_maxSP_str           "maxSP"
#define REDIS_GAME_SESSION_info_currentStamina_str  "currentStamina"
#define REDIS_GAME_SESSION_info_maxStamina_str      "maxStamina"
#define REDIS_GAME_SESSION_info_unk6_str            "unk6"
#define REDIS_GAME_SESSION_info_unk7_str            "unk7"

enum RedisGameSessionFields {
    REDIS_GAME_SESSION_socketId,
    REDIS_GAME_SESSION_pcId,
    REDIS_GAME_SESSION_mapId,
    REDIS_GAME_SESSION_charactersBarrackCount,
    REDIS_GAME_SESSION_accountLogin,

    REDIS_GAME_SESSION_commander_commanderName,
    REDIS_GAME_SESSION_commander_familyName,
    REDIS_GAME_SESSION_commander_unk1,
    REDIS_GAME_SESSION_commander_unk2,
    REDIS_GAME_SESSION_commander_unk3,
    REDIS_GAME_SESSION_commander_accountId,
    REDIS_GAME_SESSION_commander_classId,
    REDIS_GAME_SESSION_commander_unk4,
    REDIS_GAME_SESSION_commander_jobId,
    REDIS_GAME_SESSION_commander_gender,
    REDIS_GAME_SESSION_commander_unk5,
    REDIS_GAME_SESSION_commander_level,
    REDIS_GAME_SESSION_commander_hairType,
    REDIS_GAME_SESSION_commander_unk6,

    REDIS_GAME_SESSION_equipment_head_top,
    REDIS_GAME_SESSION_equipment_head_middle,
    REDIS_GAME_SESSION_equipment_itemUnk1,
    REDIS_GAME_SESSION_equipment_body_armor,
    REDIS_GAME_SESSION_equipment_gloves,
    REDIS_GAME_SESSION_equipment_boots,
    REDIS_GAME_SESSION_equipment_itemUnk2,
    REDIS_GAME_SESSION_equipment_bracelet,
    REDIS_GAME_SESSION_equipment_weapon,
    REDIS_GAME_SESSION_equipment_shield,
    REDIS_GAME_SESSION_equipment_costume,
    REDIS_GAME_SESSION_equipment_itemUnk3,
    REDIS_GAME_SESSION_equipment_itemUnk4,
    REDIS_GAME_SESSION_equipment_itemUnk5,
    REDIS_GAME_SESSION_equipment_leg_armor,
    REDIS_GAME_SESSION_equipment_itemUnk6,
    REDIS_GAME_SESSION_equipment_itemUnk7,
    REDIS_GAME_SESSION_equipment_ring_left,
    REDIS_GAME_SESSION_equipment_ring_right,
    REDIS_GAME_SESSION_equipment_necklace,

    REDIS_GAME_SESSION_info_posX,
    REDIS_GAME_SESSION_info_posY,
    REDIS_GAME_SESSION_info_posZ,
    REDIS_GAME_SESSION_info_currentXP,
    REDIS_GAME_SESSION_info_maxXP,
    REDIS_GAME_SESSION_info_commanderId,
    REDIS_GAME_SESSION_info_zoneServerId,
    REDIS_GAME_SESSION_info_zoneServerId2,
    REDIS_GAME_SESSION_info_currentHP,
    REDIS_GAME_SESSION_info_maxHP,
    REDIS_GAME_SESSION_info_currentSP,
    REDIS_GAME_SESSION_info_maxSP,
    REDIS_GAME_SESSION_info_currentStamina,
    REDIS_GAME_SESSION_info_maxStamina,
    REDIS_GAME_SESSION_info_unk6,
    REDIS_GAME_SESSION_info_unk7,

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
 * @return true on success, false otherwise
 */
bool
Redis_getGameSession (
    Redis *self,
    RedisGameSessionKey *key,
    GameSession *gameSession
);

/**
 * @brief Get the GameSession by socket Id
 * @param self An allocated Redis
 * @param routerId The router ID containing the target session
 * @param socketId The socket ID of the target session
 * @param[out] gameSession The output gameSession
 * @return true on success, false otherwise
 */
bool
Redis_getGameSessionBySocketId (
    Redis *self,
    uint16_t routerId,
    uint8_t *socketId,
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
    uint8_t *socketId,
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
 * @param socketIdNoInclude if not NULL, don't include this socketId to the result
 * @return a zlist_t of identity keys
 */
zlist_t *
Redis_getClientsWithinDistance (
    Redis *self,
    uint16_t serverId, uint16_t mapId,
    PositionXZ *position,
    float range,
    char *socketIdNoInclude
);
