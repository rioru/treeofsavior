/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */


// ---------- Includes ------------
#include "RedisGameSession.h"
#include "RedisSocketSession.h"
#include "Common/utils/math.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern variables implementation -------
const char *redisGameSessionsStr [] = {
	[REDIS_GAME_SESSION_socketId]                  = REDIS_GAME_SESSION_socketId_str,
	[REDIS_GAME_SESSION_routerId]                  = REDIS_GAME_SESSION_routerId_str,
	[REDIS_GAME_SESSION_charactersBarrackCount]    = REDIS_GAME_SESSION_charactersBarrackCount_str,
	[REDIS_GAME_SESSION_accountLogin]              = REDIS_GAME_SESSION_accountLogin_str,
	[REDIS_GAME_SESSION_commander_commanderName]   = REDIS_GAME_SESSION_commander_commanderName_str,
	[REDIS_GAME_SESSION_commander_familyName]      = REDIS_GAME_SESSION_commander_familyName_str,
	[REDIS_GAME_SESSION_commander_accountId]       = REDIS_GAME_SESSION_commander_accountId_str,
	[REDIS_GAME_SESSION_commander_classId]         = REDIS_GAME_SESSION_commander_classId_str,
	[REDIS_GAME_SESSION_commander_jobId]           = REDIS_GAME_SESSION_commander_jobId_str,
	[REDIS_GAME_SESSION_commander_gender]          = REDIS_GAME_SESSION_commander_gender_str,
	[REDIS_GAME_SESSION_commander_level]           = REDIS_GAME_SESSION_commander_level_str,
	[REDIS_GAME_SESSION_commander_head_top]        = REDIS_GAME_SESSION_commander_head_top_str,
	[REDIS_GAME_SESSION_commander_head_middle]     = REDIS_GAME_SESSION_commander_head_middle_str,
	[REDIS_GAME_SESSION_commander_itemUnk1]        = REDIS_GAME_SESSION_commander_itemUnk1_str,
	[REDIS_GAME_SESSION_commander_body_armor]      = REDIS_GAME_SESSION_commander_body_armor_str,
	[REDIS_GAME_SESSION_commander_gloves]          = REDIS_GAME_SESSION_commander_gloves_str,
	[REDIS_GAME_SESSION_commander_boots]           = REDIS_GAME_SESSION_commander_boots_str,
	[REDIS_GAME_SESSION_commander_bracelet]        = REDIS_GAME_SESSION_commander_bracelet_str,
	[REDIS_GAME_SESSION_commander_weapon]          = REDIS_GAME_SESSION_commander_weapon_str,
	[REDIS_GAME_SESSION_commander_shield]          = REDIS_GAME_SESSION_commander_shield_str,
	[REDIS_GAME_SESSION_commander_costume]         = REDIS_GAME_SESSION_commander_costume_str,
	[REDIS_GAME_SESSION_commander_itemUnk3]        = REDIS_GAME_SESSION_commander_itemUnk3_str,
	[REDIS_GAME_SESSION_commander_itemUnk4]        = REDIS_GAME_SESSION_commander_itemUnk4_str,
	[REDIS_GAME_SESSION_commander_itemUnk5]        = REDIS_GAME_SESSION_commander_itemUnk5_str,
	[REDIS_GAME_SESSION_commander_leg_armor]       = REDIS_GAME_SESSION_commander_leg_armor_str,
	[REDIS_GAME_SESSION_commander_itemUnk6]        = REDIS_GAME_SESSION_commander_itemUnk6_str,
	[REDIS_GAME_SESSION_commander_itemUnk7]        = REDIS_GAME_SESSION_commander_itemUnk7_str,
	[REDIS_GAME_SESSION_commander_ring_left]       = REDIS_GAME_SESSION_commander_ring_left_str,
	[REDIS_GAME_SESSION_commander_ring_right]      = REDIS_GAME_SESSION_commander_ring_right_str,
	[REDIS_GAME_SESSION_commander_necklace]        = REDIS_GAME_SESSION_commander_necklace_str,
	[REDIS_GAME_SESSION_commander_hairType]        = REDIS_GAME_SESSION_commander_hairType_str,
	[REDIS_GAME_SESSION_commander_pcId]            = REDIS_GAME_SESSION_commander_pcId_str,
	[REDIS_GAME_SESSION_commander_unk13]           = REDIS_GAME_SESSION_commander_unk13_str,
	[REDIS_GAME_SESSION_commander_charPosition]    = REDIS_GAME_SESSION_commander_charPosition_str,
	[REDIS_GAME_SESSION_commander_mapId]           = REDIS_GAME_SESSION_commander_mapId_str,
	[REDIS_GAME_SESSION_commander_currentXP]       = REDIS_GAME_SESSION_commander_currentXP_str,
	[REDIS_GAME_SESSION_commander_maxXP]           = REDIS_GAME_SESSION_commander_maxXP_str,
	[REDIS_GAME_SESSION_commander_spriteId]        = REDIS_GAME_SESSION_commander_spriteId_str,
	[REDIS_GAME_SESSION_commander_spriteIdRelated] = REDIS_GAME_SESSION_commander_spriteIdRelated_str,
	[REDIS_GAME_SESSION_commander_currentHP]       = REDIS_GAME_SESSION_commander_currentHP_str,
	[REDIS_GAME_SESSION_commander_maxHP]           = REDIS_GAME_SESSION_commander_maxHP_str,
	[REDIS_GAME_SESSION_commander_currentSP]       = REDIS_GAME_SESSION_commander_currentSP_str,
	[REDIS_GAME_SESSION_commander_maxSP]           = REDIS_GAME_SESSION_commander_maxSP_str,
	[REDIS_GAME_SESSION_commander_posX]           = REDIS_GAME_SESSION_commander_posX_str,
	[REDIS_GAME_SESSION_commander_posZ]           = REDIS_GAME_SESSION_commander_posZ_str
};


// ------ Extern functions implementation -------

bool
Redis_getGameSession (
    Redis *self,
    RedisGameSessionKey *key,
    GameSession *gameSession
) {
    bool result = true;
    redisReply *reply = NULL;

    reply = Redis_commandDbg (self,
        "HMGET zone%x:map%x:acc%llx"
        " " REDIS_GAME_SESSION_socketId_str
        " " REDIS_GAME_SESSION_routerId_str
        " " REDIS_GAME_SESSION_charactersBarrackCount_str
        " " REDIS_GAME_SESSION_accountLogin_str
        " " REDIS_GAME_SESSION_commander_commanderName_str
        " " REDIS_GAME_SESSION_commander_familyName_str
        // [UNKNOWN] " commander.unk1 "
        " " REDIS_GAME_SESSION_commander_accountId_str
        " " REDIS_GAME_SESSION_commander_classId_str
        // [UNKNOWN] " commander.unk2 "
        " " REDIS_GAME_SESSION_commander_jobId_str
        " " REDIS_GAME_SESSION_commander_gender_str
        // [UNKNOWN] "commander.unk3 "
        " " REDIS_GAME_SESSION_commander_level_str
        " " REDIS_GAME_SESSION_commander_head_top_str
        " " REDIS_GAME_SESSION_commander_head_middle_str
        " " REDIS_GAME_SESSION_commander_itemUnk1_str
        " " REDIS_GAME_SESSION_commander_body_armor_str
        " " REDIS_GAME_SESSION_commander_gloves_str
        " " REDIS_GAME_SESSION_commander_boots_str
        " " REDIS_GAME_SESSION_commander_itemUnk2_str
        " " REDIS_GAME_SESSION_commander_bracelet_str
        " " REDIS_GAME_SESSION_commander_weapon_str
        " " REDIS_GAME_SESSION_commander_shield_str
        " " REDIS_GAME_SESSION_commander_costume_str
        " " REDIS_GAME_SESSION_commander_itemUnk3_str
        " " REDIS_GAME_SESSION_commander_itemUnk4_str
        " " REDIS_GAME_SESSION_commander_itemUnk5_str
        " " REDIS_GAME_SESSION_commander_leg_armor_str
        " " REDIS_GAME_SESSION_commander_itemUnk6_str
        " " REDIS_GAME_SESSION_commander_itemUnk7_str
        " " REDIS_GAME_SESSION_commander_ring_left_str
        " " REDIS_GAME_SESSION_commander_ring_right_str
        " " REDIS_GAME_SESSION_commander_necklace_str
        " " REDIS_GAME_SESSION_commander_hairType_str
        // [UNKNOWN] "commander.unk4 "
        // [UNKNOWN] "commander.unk5 "
        " " REDIS_GAME_SESSION_commander_pcId_str
        " " REDIS_GAME_SESSION_commander_unk13_str
        " " REDIS_GAME_SESSION_commander_charPosition_str
        // [UNKNOWN] "commander.unk6 "
        " " REDIS_GAME_SESSION_commander_mapId_str
        " " REDIS_GAME_SESSION_commander_currentXP_str
        " " REDIS_GAME_SESSION_commander_maxXP_str
        // [UNKNOWN] "commander.unk7 "
        " " REDIS_GAME_SESSION_commander_spriteId_str
        " " REDIS_GAME_SESSION_commander_spriteIdRelated_str
        " " REDIS_GAME_SESSION_commander_currentHP_str
        " " REDIS_GAME_SESSION_commander_maxHP_str
        " " REDIS_GAME_SESSION_commander_currentSP_str
        " " REDIS_GAME_SESSION_commander_maxSP_str
        " " REDIS_GAME_SESSION_commander_posX_str
        " " REDIS_GAME_SESSION_commander_posZ_str
        // [UNKNOWN] "commander.unk10 "
        // [UNKNOWN] "commander.unk11 "
        // [UNKNOWN] "commander.unk12 "
        , key->routerId, key->mapId, key->accountId
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        result = false;
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            result = false;
            goto cleanup;
        break;

        case REDIS_REPLY_STATUS:
            // info ("Redis status : %s", reply->str);
        break;

        case REDIS_REPLY_ARRAY: {

            // Check the number of elements retrieved
            if (reply->elements != REDIS_GAME_SESSION_COUNT) {
                error ("Wrong number of elements received.");
                result = false;
                goto cleanup;
            }

            // Check if any element of the reply is NULL
            size_t elementIndex;
            if ((elementIndex = Redis_anyElementIsNull (reply->element, reply->elements)) != -1) {
                error ("Element <%s> returned by Redis is NULL.", redisGameSessionsStr[elementIndex]);
                result = false;
                goto cleanup;
            }

            // Write the reply to the session
            strncpy (gameSession->socketId, reply->element[REDIS_GAME_SESSION_socketId]->str, sizeof (gameSession->socketId));
            gameSession->charactersBarrackCount           = strtoul (reply->element[REDIS_GAME_SESSION_charactersBarrackCount]->str, NULL, 16);
            strncpy (gameSession->accountLogin, reply->element[REDIS_GAME_SESSION_accountLogin]->str, sizeof (gameSession->accountLogin));
            strncpy (gameSession->currentCommander.familyName, reply->element[REDIS_GAME_SESSION_commander_familyName]->str, sizeof (gameSession->currentCommander.familyName));
            strncpy (gameSession->currentCommander.commanderName, reply->element[REDIS_GAME_SESSION_commander_commanderName]->str, sizeof (gameSession->currentCommander.commanderName));
            // [UNKNOWN] gameSession->currentCommander.unk1,
            gameSession->currentCommander.accountId       = strtoull (reply->element[REDIS_GAME_SESSION_commander_accountId]->str, NULL, 16);
            gameSession->currentCommander.classId         = strtoul (reply->element[REDIS_GAME_SESSION_commander_classId]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk2,
            gameSession->currentCommander.jobId           = strtoul (reply->element[REDIS_GAME_SESSION_commander_jobId]->str, NULL, 16);
            gameSession->currentCommander.gender          = strtoul (reply->element[REDIS_GAME_SESSION_commander_gender]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk3,
            gameSession->currentCommander.level           = strtoul (reply->element[REDIS_GAME_SESSION_commander_level]->str, NULL, 16);
            gameSession->currentCommander.head_top        = strtoul (reply->element[REDIS_GAME_SESSION_commander_head_top]->str, NULL, 16);
            gameSession->currentCommander.head_middle     = strtoul (reply->element[REDIS_GAME_SESSION_commander_head_middle]->str, NULL, 16);
            gameSession->currentCommander.itemUnk1        = strtoul (reply->element[REDIS_GAME_SESSION_commander_itemUnk1]->str, NULL, 16);
            gameSession->currentCommander.body_armor      = strtoul (reply->element[REDIS_GAME_SESSION_commander_body_armor]->str, NULL, 16);
            gameSession->currentCommander.gloves          = strtoul (reply->element[REDIS_GAME_SESSION_commander_gloves]->str, NULL, 16);
            gameSession->currentCommander.boots           = strtoul (reply->element[REDIS_GAME_SESSION_commander_boots]->str, NULL, 16);
            gameSession->currentCommander.itemUnk2        = strtoul (reply->element[REDIS_GAME_SESSION_commander_itemUnk2]->str, NULL, 16);
            gameSession->currentCommander.bracelet        = strtoul (reply->element[REDIS_GAME_SESSION_commander_bracelet]->str, NULL, 16);
            gameSession->currentCommander.weapon          = strtoul (reply->element[REDIS_GAME_SESSION_commander_weapon]->str, NULL, 16);
            gameSession->currentCommander.shield          = strtoul (reply->element[REDIS_GAME_SESSION_commander_shield]->str, NULL, 16);
            gameSession->currentCommander.costume         = strtoul (reply->element[REDIS_GAME_SESSION_commander_costume]->str, NULL, 16);
            gameSession->currentCommander.itemUnk3        = strtoul (reply->element[REDIS_GAME_SESSION_commander_itemUnk3]->str, NULL, 16);
            gameSession->currentCommander.itemUnk4        = strtoul (reply->element[REDIS_GAME_SESSION_commander_itemUnk4]->str, NULL, 16);
            gameSession->currentCommander.itemUnk5        = strtoul (reply->element[REDIS_GAME_SESSION_commander_itemUnk5]->str, NULL, 16);
            gameSession->currentCommander.leg_armor       = strtoul (reply->element[REDIS_GAME_SESSION_commander_leg_armor]->str, NULL, 16);
            gameSession->currentCommander.itemUnk6        = strtoul (reply->element[REDIS_GAME_SESSION_commander_itemUnk6]->str, NULL, 16);
            gameSession->currentCommander.itemUnk7        = strtoul (reply->element[REDIS_GAME_SESSION_commander_itemUnk7]->str, NULL, 16);
            gameSession->currentCommander.ring_left       = strtoul (reply->element[REDIS_GAME_SESSION_commander_ring_left]->str, NULL, 16);
            gameSession->currentCommander.ring_right      = strtoul (reply->element[REDIS_GAME_SESSION_commander_ring_right]->str, NULL, 16);
            gameSession->currentCommander.necklace        = strtoul (reply->element[REDIS_GAME_SESSION_commander_necklace]->str, NULL, 16);
            gameSession->currentCommander.hairType        = strtoul (reply->element[REDIS_GAME_SESSION_commander_hairType]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk4,
            // [UNKNOWN] gameSession->currentCommander.unk5,
            gameSession->currentCommander.pcId            = strtoul (reply->element[REDIS_GAME_SESSION_commander_pcId]->str, NULL, 16);
            gameSession->currentCommander.unk13           = strtoul (reply->element[REDIS_GAME_SESSION_commander_unk13]->str, NULL, 16);
            gameSession->currentCommander.charPosition    = strtoul (reply->element[REDIS_GAME_SESSION_commander_charPosition]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk6,
            gameSession->currentCommander.mapId           = strtoul (reply->element[REDIS_GAME_SESSION_commander_mapId]->str, NULL, 16);
            gameSession->currentCommander.currentXP       = strtoul (reply->element[REDIS_GAME_SESSION_commander_currentXP]->str, NULL, 16);
            gameSession->currentCommander.maxXP           = strtoul (reply->element[REDIS_GAME_SESSION_commander_maxXP]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk7,
            gameSession->currentCommander.spriteId        = strtoul (reply->element[REDIS_GAME_SESSION_commander_spriteId]->str, NULL, 16);
            gameSession->currentCommander.spriteIdRelated = strtoul (reply->element[REDIS_GAME_SESSION_commander_spriteIdRelated]->str, NULL, 16);
            gameSession->currentCommander.currentHP       = strtoul (reply->element[REDIS_GAME_SESSION_commander_currentHP]->str, NULL, 16);
            gameSession->currentCommander.maxHP           = strtoul (reply->element[REDIS_GAME_SESSION_commander_maxHP]->str, NULL, 16);
            gameSession->currentCommander.currentSP       = strtoul (reply->element[REDIS_GAME_SESSION_commander_currentSP]->str, NULL, 16);
            gameSession->currentCommander.maxSP           = strtoul (reply->element[REDIS_GAME_SESSION_commander_maxSP]->str, NULL, 16);
            gameSession->currentCommander.pos.x          = strtof (reply->element[REDIS_GAME_SESSION_commander_posX]->str, NULL);
            gameSession->currentCommander.pos.z          = strtof (reply->element[REDIS_GAME_SESSION_commander_posZ]->str, NULL);
            // [UNKNOWN] gameSession->currentCommander.unk10,
            // [UNKNOWN] gameSession->currentCommander.unk11,
            // [UNKNOWN] gameSession->currentCommander.unk12,
        }
        break;

        default :
            error ("Unexpected Redis status (%d).", reply->type);
            result = false;
            goto cleanup;
        break;
    }

cleanup:
    if (reply) {
        Redis_replyDestroy (&reply);
    }

    return result;
}

bool
Redis_getGameSessionBySocketId (
    Redis *self,
    uint16_t routerId,
    uint8_t *socketId,
    GameSession *gameSession
) {
    SocketSession socketSession;
    RedisSocketSessionKey socketKey = {
        .routerId = routerId,
        .socketId = socketId
    };
    if (!(Redis_getSocketSession (self, &socketKey, &socketSession))) {
        error ("Cannot get the socket session of the client.");
        return false;
    }

    RedisGameSessionKey gameKey = {
        .routerId = socketSession.routerId,
        .mapId = socketSession.mapId,
        .accountId = socketSession.accountId
    };

    if (!(Redis_getGameSession (self, &gameKey, gameSession))) {
        error ("Cannot get the game session of the client.");
        return false;
    }

    return true;
}

bool
Redis_updateGameSession (
    Redis *self,
    RedisGameSessionKey *key,
    uint8_t *socketId,
    GameSession *gameSession
) {
    bool result = true;
    redisReply *reply = NULL;

    reply = Redis_commandDbg (self,
        "HMSET zone%x:map%x:acc%llx"
        " " REDIS_GAME_SESSION_socketId_str " %s"
        " " REDIS_GAME_SESSION_routerId_str " %x"
        " " REDIS_GAME_SESSION_charactersBarrackCount_str " %x"
        " " REDIS_GAME_SESSION_accountLogin_str " %s"
        " " REDIS_GAME_SESSION_commander_commanderName_str " %s"
        " " REDIS_GAME_SESSION_commander_familyName_str " %s"
        // [UNKNOWN] " " commander.unk1 " "%s"
        " " REDIS_GAME_SESSION_commander_accountId_str " %llx"
        " " REDIS_GAME_SESSION_commander_classId_str " %x"
        // [UNKNOWN] " " REDIS_GAME_SESSION_commander_unk2 " "%s"
        " " REDIS_GAME_SESSION_commander_jobId_str " %x"
        " " REDIS_GAME_SESSION_commander_gender_str " %x"
        // [UNKNOWN] "commander_unk3 " " %x"
        " " REDIS_GAME_SESSION_commander_level_str " %x"
        " " REDIS_GAME_SESSION_commander_head_top_str " %x"
        " " REDIS_GAME_SESSION_commander_head_middle_str " %x"
        " " REDIS_GAME_SESSION_commander_itemUnk1_str " %x"
        " " REDIS_GAME_SESSION_commander_body_armor_str " %x"
        " " REDIS_GAME_SESSION_commander_gloves_str " %x"
        " " REDIS_GAME_SESSION_commander_boots_str " %x"
        " " REDIS_GAME_SESSION_commander_itemUnk2_str " %x"
        " " REDIS_GAME_SESSION_commander_bracelet_str " %x"
        " " REDIS_GAME_SESSION_commander_weapon_str " %x"
        " " REDIS_GAME_SESSION_commander_shield_str " %x"
        " " REDIS_GAME_SESSION_commander_costume_str " %x"
        " " REDIS_GAME_SESSION_commander_itemUnk3_str " %x"
        " " REDIS_GAME_SESSION_commander_itemUnk4_str " %x"
        " " REDIS_GAME_SESSION_commander_itemUnk5_str " %x"
        " " REDIS_GAME_SESSION_commander_leg_armor_str " %x"
        " " REDIS_GAME_SESSION_commander_itemUnk6_str " %x"
        " " REDIS_GAME_SESSION_commander_itemUnk7_str " %x"
        " " REDIS_GAME_SESSION_commander_ring_left_str " %x"
        " " REDIS_GAME_SESSION_commander_ring_right_str " %x"
        " " REDIS_GAME_SESSION_commander_necklace_str " %x"
        " " REDIS_GAME_SESSION_commander_hairType_str " %x"
        // [UNKNOWN] "commander_unk4 " " %x"
        // [UNKNOWN] "commander_unk5 " " %x"
        " " REDIS_GAME_SESSION_commander_pcId_str " %x"
        " " REDIS_GAME_SESSION_commander_unk13_str " %x"
        " " REDIS_GAME_SESSION_commander_charPosition_str " %x"
        // [UNKNOWN] "commander_unk6 " " %x"
        " " REDIS_GAME_SESSION_commander_mapId_str " %x"
        " " REDIS_GAME_SESSION_commander_currentXP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxXP_str " %x"
        // [UNKNOWN] "commander_unk7 " " %x"
        " " REDIS_GAME_SESSION_commander_spriteId_str " %x"
        " " REDIS_GAME_SESSION_commander_spriteIdRelated_str " %x"
        " " REDIS_GAME_SESSION_commander_currentHP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxHP_str " %x"
        " " REDIS_GAME_SESSION_commander_currentSP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxSP_str " %x"
        " " REDIS_GAME_SESSION_commander_posX_str " %f"
        " " REDIS_GAME_SESSION_commander_posZ_str " %f"
        , key->routerId, key->mapId, key->accountId,
        socketId,
        key->routerId,
        gameSession->charactersBarrackCount,
        gameSession->accountLogin,
        (gameSession->currentCommander.commanderName[0] != '\0') ? gameSession->currentCommander.commanderName : REDIS_EMPTY_STRING,
        (gameSession->currentCommander.familyName[0] != '\0') ? gameSession->currentCommander.familyName : REDIS_EMPTY_STRING,
        // [UNKNOWN] gameSession->currentCommander.unk1,
        key->accountId,
        gameSession->currentCommander.classId,
        // [UNKNOWN] gameSession->currentCommander.unk2,
        gameSession->currentCommander.jobId,
        gameSession->currentCommander.gender,
        // [UNKNOWN] gameSession->currentCommander.unk3,
        gameSession->currentCommander.level,
        gameSession->currentCommander.head_top,
        gameSession->currentCommander.head_middle,
        gameSession->currentCommander.itemUnk1,
        gameSession->currentCommander.body_armor,
        gameSession->currentCommander.gloves,
        gameSession->currentCommander.boots,
        gameSession->currentCommander.itemUnk2,
        gameSession->currentCommander.bracelet,
        gameSession->currentCommander.weapon,
        gameSession->currentCommander.shield,
        gameSession->currentCommander.costume,
        gameSession->currentCommander.itemUnk3,
        gameSession->currentCommander.itemUnk4,
        gameSession->currentCommander.itemUnk5,
        gameSession->currentCommander.leg_armor,
        gameSession->currentCommander.itemUnk6,
        gameSession->currentCommander.itemUnk7,
        gameSession->currentCommander.ring_left,
        gameSession->currentCommander.ring_right,
        gameSession->currentCommander.necklace,
        gameSession->currentCommander.hairType,
        // [UNKNOWN] gameSession->currentCommander.unk4,
        // [UNKNOWN] gameSession->currentCommander.unk5,
        gameSession->currentCommander.pcId,
        gameSession->currentCommander.unk13,
        gameSession->currentCommander.charPosition,
        // [UNKNOWN] gameSession->currentCommander.unk6,
        gameSession->currentCommander.mapId,
        gameSession->currentCommander.currentXP,
        gameSession->currentCommander.maxXP,
        // [UNKNOWN] gameSession->currentCommander.unk7,
        gameSession->currentCommander.spriteId,
        gameSession->currentCommander.spriteIdRelated,
        gameSession->currentCommander.currentHP,
        gameSession->currentCommander.maxHP,
        gameSession->currentCommander.currentSP,
        gameSession->currentCommander.maxSP,
        gameSession->currentCommander.pos.x,
        gameSession->currentCommander.pos.z
        // [UNKNOWN] gameSession->currentCommander.unk8,
        // [UNKNOWN] gameSession->currentCommander.unk9,
        // [UNKNOWN] gameSession->currentCommander.unk10,
        // [UNKNOWN] gameSession->currentCommander.unk11,
        // [UNKNOWN] gameSession->currentCommander.unk12,
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        result = false;
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            result = false;
            goto cleanup;
        break;

        case REDIS_REPLY_STATUS:
            // info ("Redis status : %s", reply->str);
        break;

        default :
            error ("Unexpected Redis status. (%d)", reply->type);
            result = false;
            goto cleanup;
        break;
    }

cleanup:
    if (reply) {
        Redis_replyDestroy (&reply);
    }

    return result;
}


bool
Redis_moveGameSession (
    Redis *self,
    RedisGameSessionKey *from,
    RedisGameSessionKey *to
) {
    bool result = true;
    redisReply *reply = NULL;

    reply = Redis_commandDbg (self,
        "RENAME "
        "zone%x:map%x:acc%llx "
        "zone%x:map%x:acc%llx",
        from->routerId, from->mapId, from->accountId,
        to->routerId,   to->mapId,   to->accountId
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        result = false;
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            result = false;
            goto cleanup;
        break;

        case REDIS_REPLY_STATUS:
            // info ("Redis status : %s", reply->str);
        break;

        default :
            error ("Unexpected Redis status. (%d)", reply->type);
            result = false;
            goto cleanup;
        break;
    }

cleanup:
    if (reply) {
        Redis_replyDestroy (&reply);
    }

    return result;
}

zlist_t *
Redis_getClientsWithinDistance (
    Redis *self,
    uint16_t routerId, uint16_t mapId,
    PositionXZ *center,
    float range,
    char *ignoredSocketId
) {
    bool status = true;
    zlist_t *clients = NULL;
    redisReply *reply = NULL;
    redisReply *posReply = NULL;

    // TODO : Could be better. Don't allocate a new zlist everytime we call this function.
    // Who got this list then ? The Worker?
    if (!(clients = zlist_new ())) {
        error ("Cannot allocate a new zlist.");
        status = false;
        goto cleanup;
    }

    // Iterate through all the clients

    // TODO : Huge optimization to do here !
    // It iterates through all the clients of the zone server in the chosen map
    // It could use a better clustering method, but it should be enough for the moment

    // Start iterating
    int iterator = 0;

    do {
        // Get all the accounts within the same area
        reply = Redis_commandDbg (self,
            "SCAN %d MATCH zone%x:map%x:acc*",
            iterator, routerId, mapId
        );

        if (!reply) {
            error ("Redis error encountered : The request is invalid.");
            status = false;
            goto cleanup;
        }

        switch (reply->type) {
            case REDIS_REPLY_ERROR:
                error ("Redis error encountered : %s", reply->str);
                status = false;
                goto cleanup;
            break;

            case REDIS_REPLY_ARRAY: {
                // [0] = new iterator
                iterator = strtoul (reply->element[0]->str, NULL, 10);
                // [1] = results
                for (int i = 0; i < reply->element[1]->elements; i++) {
                    // Get the position of all accounts in the map
                    posReply = Redis_commandDbg (self,
                        "HMGET %s " REDIS_GAME_SESSION_commander_posX_str
                                " " REDIS_GAME_SESSION_commander_posZ_str // Get position
                                " " REDIS_GAME_SESSION_socketId_str, // SocketKey
                        reply->element[1]->element[i]->str // account key
                    );

                    if (!posReply) {
                        error ("Redis error encountered : The request is invalid.");
                        status = false;
                        goto cleanup;
                    }

                    switch (posReply->type) {

                        case REDIS_REPLY_ERROR:
                            error ("Redis error encountered : %s", reply->str);
                            status = false;
                            goto cleanup;
                        break;

                        case REDIS_REPLY_STATUS:
                            // info ("Redis status : %s", reply->str);
                        break;

                        case REDIS_REPLY_ARRAY: {
                            if (posReply->elements != 3) {
                                error ("Abnormal number of elements (%d, should be 3).", posReply->elements);
                                status = false;
                                goto cleanup;
                            }

                            // [0] = X, [1] = Z, [2] = socketId
                            PositionXZ curPpos = {
                                .x = strtof (posReply->element[0]->str, NULL),
                                .z = strtof (posReply->element[1]->str, NULL)
                            };
                            char *socketId = posReply->element[2]->str;

                            // Check range here
                            if (Math_isWithin2DManhattanDistance (&curPpos, center, range)) {
                                // The current client is within the area, add it to the list
                                // Don't include the ignored socketId
                                if (!(ignoredSocketId && strcmp (socketId, ignoredSocketId) == 0)) {
                                    zlist_append (clients, strdup (socketId));
                                }
                            }
                        } break;

                        default :
                            error ("Unexpected Redis status. (%d)", reply->type);
                            status = false;
                            goto cleanup;
                        break;
                    }

                    Redis_replyDestroy (&posReply);
                }
            } break;

            default :
                error ("Unexpected Redis status. (%d)", reply->type);
                status = false;
                goto cleanup;
            break;
        }

        Redis_replyDestroy (&reply);

    } while (iterator != 0);

cleanup:
    if (!status) {
        zlist_destroy (&clients);
    }
    Redis_replyDestroy (&reply);
    Redis_replyDestroy (&posReply);

    return clients;
}



bool
Redis_flushGameSession (
    Redis *self,
    RedisGameSessionKey *key
) {
    bool result = true;
    redisReply *reply = NULL;

    // Delete the key from the Redis
    reply = Redis_commandDbg (self,
        "DEL zone%x:map%x:acc%llx",
        key->routerId, key->mapId, key->accountId
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        result = false;
        goto cleanup;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            result = false;
            goto cleanup;
        break;

        case REDIS_REPLY_INTEGER:
            // Delete OK
        break;

        default :
            error ("Unexpected Redis status : %d", reply->type);
            result = false;
            goto cleanup;
        break;
    }

cleanup:
    if (reply) {
        Redis_replyDestroy (&reply);
    }

    return result;
}
