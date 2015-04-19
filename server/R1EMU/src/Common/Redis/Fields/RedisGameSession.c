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


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern variables implementation -------
const char *redisGameSessionsStr [] = {
	[REDIS_GAME_SESSION_zoneId] = REDIS_GAME_SESSION_zoneId_str,
	[REDIS_GAME_SESSION_familyName] = REDIS_GAME_SESSION_familyName_str,
	[REDIS_GAME_SESSION_currentCommanderName] = REDIS_GAME_SESSION_currentCommanderName_str,
	[REDIS_GAME_SESSION_charactersBarrackCount] = REDIS_GAME_SESSION_charactersBarrackCount_str,
	[REDIS_GAME_SESSION_accountId] = REDIS_GAME_SESSION_accountId_str,
	[REDIS_GAME_SESSION_currentCommanderId] = REDIS_GAME_SESSION_currentCommanderId_str,
	[REDIS_GAME_SESSION_currentPcId] = REDIS_GAME_SESSION_currentPcId_str,
	[REDIS_GAME_SESSION_commander_classId] = REDIS_GAME_SESSION_commander_classId_str,
	[REDIS_GAME_SESSION_commander_jobId] = REDIS_GAME_SESSION_commander_jobId_str,
	[REDIS_GAME_SESSION_commander_gender] = REDIS_GAME_SESSION_commander_gender_str,
	[REDIS_GAME_SESSION_commander_level] = REDIS_GAME_SESSION_commander_level_str,
	[REDIS_GAME_SESSION_commander_head_top] = REDIS_GAME_SESSION_commander_head_top_str,
	[REDIS_GAME_SESSION_commander_head_middle] = REDIS_GAME_SESSION_commander_head_middle_str,
	[REDIS_GAME_SESSION_commander_body_armor] = REDIS_GAME_SESSION_commander_body_armor_str,
	[REDIS_GAME_SESSION_commander_gloves] = REDIS_GAME_SESSION_commander_gloves_str,
	[REDIS_GAME_SESSION_commander_boots] = REDIS_GAME_SESSION_commander_boots_str,
	[REDIS_GAME_SESSION_commander_bracelet] = REDIS_GAME_SESSION_commander_bracelet_str,
	[REDIS_GAME_SESSION_commander_weapon] = REDIS_GAME_SESSION_commander_weapon_str,
	[REDIS_GAME_SESSION_commander_shield] = REDIS_GAME_SESSION_commander_shield_str,
	[REDIS_GAME_SESSION_commander_costume] = REDIS_GAME_SESSION_commander_costume_str,
	[REDIS_GAME_SESSION_commander_leg_armor] = REDIS_GAME_SESSION_commander_leg_armor_str,
	[REDIS_GAME_SESSION_commander_ring_left] = REDIS_GAME_SESSION_commander_ring_left_str,
	[REDIS_GAME_SESSION_commander_ring_right] = REDIS_GAME_SESSION_commander_ring_right_str,
	[REDIS_GAME_SESSION_commander_necklace] = REDIS_GAME_SESSION_commander_necklace_str,
	[REDIS_GAME_SESSION_commander_hairType] = REDIS_GAME_SESSION_commander_hairType_str,
	[REDIS_GAME_SESSION_commander_pcId] = REDIS_GAME_SESSION_commander_pcId_str,
	[REDIS_GAME_SESSION_commander_listPosition] = REDIS_GAME_SESSION_commander_listPosition_str,
	[REDIS_GAME_SESSION_commander_charPosition] = REDIS_GAME_SESSION_commander_charPosition_str,
	[REDIS_GAME_SESSION_commander_mapId] = REDIS_GAME_SESSION_commander_mapId_str,
	[REDIS_GAME_SESSION_commander_currentXP] = REDIS_GAME_SESSION_commander_currentXP_str,
	[REDIS_GAME_SESSION_commander_maxXP] = REDIS_GAME_SESSION_commander_maxXP_str,
	[REDIS_GAME_SESSION_commander_pose] = REDIS_GAME_SESSION_commander_pose_str,
	[REDIS_GAME_SESSION_commander_spriteRelated] = REDIS_GAME_SESSION_commander_spriteRelated_str,
	[REDIS_GAME_SESSION_commander_currentHP] = REDIS_GAME_SESSION_commander_currentHP_str,
	[REDIS_GAME_SESSION_commander_maxHP] = REDIS_GAME_SESSION_commander_maxHP_str,
	[REDIS_GAME_SESSION_commander_currentSP] = REDIS_GAME_SESSION_commander_currentSP_str,
	[REDIS_GAME_SESSION_commander_maxSP] = REDIS_GAME_SESSION_commander_maxSP_str
};


// ------ Extern functions implementation -------

bool
Redis_getGameSession (
    Redis *self,
    SocketSession *socketSession,
    GameSession *session
) {
    redisReply *reply = NULL;
    char key[100];

    if (socketSession->isInBarrack) {
        snprintf (key, sizeof (key), "barrack%llx", socketSession->accountId);
    } else {
        snprintf (key, sizeof (key), "zone%d:map%d:acc%llx", socketSession->zoneId, socketSession->mapId, socketSession->accountId);
    }

    reply = Redis_commandDbg (self,
        "HMGET %s"
        " " REDIS_GAME_SESSION_zoneId_str
        " " REDIS_GAME_SESSION_familyName_str
        " " REDIS_GAME_SESSION_currentCommanderName_str
        " " REDIS_GAME_SESSION_charactersBarrackCount_str
        " " REDIS_GAME_SESSION_accountId_str
        " " REDIS_GAME_SESSION_currentCommanderId_str
        " " REDIS_GAME_SESSION_currentPcId_str
        // [UNKNOWN] " commander.unk1 "
        " " REDIS_GAME_SESSION_commander_classId_str
        // [UNKNOWN] " commander.unk2 "
        " " REDIS_GAME_SESSION_commander_jobId_str
        " " REDIS_GAME_SESSION_commander_gender_str
        // [UNKNOWN] "commander.unk3 "
        " " REDIS_GAME_SESSION_commander_level_str
        " " REDIS_GAME_SESSION_commander_head_top_str
        " " REDIS_GAME_SESSION_commander_head_middle_str
        // [UNKNOWN] "commander.itemUnk1 "
        " " REDIS_GAME_SESSION_commander_body_armor_str
        " " REDIS_GAME_SESSION_commander_gloves_str
        " " REDIS_GAME_SESSION_commander_boots_str
        // [UNKNOWN] "commander.itemUnk2 "
        " " REDIS_GAME_SESSION_commander_bracelet_str
        " " REDIS_GAME_SESSION_commander_weapon_str
        " " REDIS_GAME_SESSION_commander_shield_str
        " " REDIS_GAME_SESSION_commander_costume_str
        // [UNKNOWN] "commander.itemUnk3 "
        // [UNKNOWN] "commander.itemUnk4 "
        // [UNKNOWN] "commander.itemUnk5 "
        " " REDIS_GAME_SESSION_commander_leg_armor_str
        // [UNKNOWN] "commander.itemUnk6 "
        // [UNKNOWN] "commander.itemUnk7 "
        " " REDIS_GAME_SESSION_commander_ring_left_str
        " " REDIS_GAME_SESSION_commander_ring_right_str
        " " REDIS_GAME_SESSION_commander_necklace_str
        " " REDIS_GAME_SESSION_commander_hairType_str
        // [UNKNOWN] "commander.unk4 "
        // [UNKNOWN] "commander.unk5 "
        " " REDIS_GAME_SESSION_commander_pcId_str
        " " REDIS_GAME_SESSION_commander_listPosition_str
        " " REDIS_GAME_SESSION_commander_charPosition_str
        // [UNKNOWN] "commander.unk6 "
        " " REDIS_GAME_SESSION_commander_mapId_str
        " " REDIS_GAME_SESSION_commander_currentXP_str
        " " REDIS_GAME_SESSION_commander_maxXP_str
        // [UNKNOWN] "commander.unk7 "
        " " REDIS_GAME_SESSION_commander_pose_str
        " " REDIS_GAME_SESSION_commander_spriteRelated_str
        " " REDIS_GAME_SESSION_commander_currentHP_str
        " " REDIS_GAME_SESSION_commander_maxHP_str
        " " REDIS_GAME_SESSION_commander_currentSP_str
        " " REDIS_GAME_SESSION_commander_maxSP_str
        // [UNKNOWN] "commander.unk8 "
        // [UNKNOWN] "commander.unk9 "
        // [UNKNOWN] "commander.unk10 "
        // [UNKNOWN] "commander.unk11 "
        // [UNKNOWN] "commander.unk12 "
        , key
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            return false;
        break;

        case REDIS_REPLY_STATUS:
            info ("Redis status : %s", reply->str);
        break;

        case REDIS_REPLY_ARRAY: {

            // Check the number of elements retrieved
            if (reply->elements != REDIS_GAME_SESSION_COUNT) {
                error ("Wrong number of elements received.");
                return false;
            }

            // Check if any element of the reply is NULL
            size_t elementIndex;
            if ((elementIndex = Redis_anyElementIsNull (reply->element, reply->elements)) != -1) {
                error ("Element <%s> returned by Redis is NULL.", redisGameSessionsStr[elementIndex]);
                return false;
            }

            Redis_printElements (reply->element, reply->elements, redisGameSessionsStr);

            // Write the reply to the session
            strncpy (session->familyName, reply->element[REDIS_GAME_SESSION_familyName]->str, sizeof (session->familyName));
            strncpy (session->currentCommanderName, reply->element[REDIS_GAME_SESSION_currentCommanderName]->str, sizeof (session->currentCommanderName));

            session->charactersBarrackCount = strtol (reply->element[REDIS_GAME_SESSION_charactersBarrackCount]->str, NULL, 16);
            session->currentCommanderId = strtoll (reply->element[REDIS_GAME_SESSION_currentCommanderId]->str, NULL, 16);
            session->currentPcId = strtoll (reply->element[REDIS_GAME_SESSION_currentPcId]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.unk1,
            session->currentCommander.classId = strtol (reply->element[REDIS_GAME_SESSION_commander_classId]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.unk2,
            session->currentCommander.jobId = strtol (reply->element[REDIS_GAME_SESSION_commander_jobId]->str, NULL, 16);
            session->currentCommander.gender = strtol (reply->element[REDIS_GAME_SESSION_commander_gender]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.unk3,
            session->currentCommander.level = strtol (reply->element[REDIS_GAME_SESSION_commander_level]->str, NULL, 16);
            session->currentCommander.head_top = strtol (reply->element[REDIS_GAME_SESSION_commander_head_top]->str, NULL, 16);
            session->currentCommander.head_middle = strtol (reply->element[REDIS_GAME_SESSION_commander_head_middle]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.itemUnk1,
            session->currentCommander.body_armor = strtol (reply->element[REDIS_GAME_SESSION_commander_body_armor]->str, NULL, 16);
            session->currentCommander.gloves = strtol (reply->element[REDIS_GAME_SESSION_commander_gloves]->str, NULL, 16);
            session->currentCommander.boots = strtol (reply->element[REDIS_GAME_SESSION_commander_boots]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.itemUnk2,
            session->currentCommander.bracelet = strtol (reply->element[REDIS_GAME_SESSION_commander_bracelet]->str, NULL, 16);
            session->currentCommander.weapon = strtol (reply->element[REDIS_GAME_SESSION_commander_weapon]->str, NULL, 16);
            session->currentCommander.shield = strtol (reply->element[REDIS_GAME_SESSION_commander_shield]->str, NULL, 16);
            session->currentCommander.costume = strtol (reply->element[REDIS_GAME_SESSION_commander_costume]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.itemUnk3,
            // [UNKNOWN] session->currentCommander.itemUnk4,
            // [UNKNOWN] session->currentCommander.itemUnk5,
            session->currentCommander.leg_armor = strtol (reply->element[REDIS_GAME_SESSION_commander_leg_armor]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.itemUnk6,
            // [UNKNOWN] session->currentCommander.itemUnk7,
            session->currentCommander.ring_left = strtol (reply->element[REDIS_GAME_SESSION_commander_ring_left]->str, NULL, 16);
            session->currentCommander.ring_right = strtol (reply->element[REDIS_GAME_SESSION_commander_ring_right]->str, NULL, 16);
            session->currentCommander.necklace = strtol (reply->element[REDIS_GAME_SESSION_commander_necklace]->str, NULL, 16);
            session->currentCommander.hairType = strtol (reply->element[REDIS_GAME_SESSION_commander_hairType]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.unk4,
            // [UNKNOWN] session->currentCommander.unk5,
            session->currentCommander.pcId = strtol (reply->element[REDIS_GAME_SESSION_commander_pcId]->str, NULL, 16);
            session->currentCommander.listPosition = strtol (reply->element[REDIS_GAME_SESSION_commander_listPosition]->str, NULL, 16);
            session->currentCommander.charPosition = strtol (reply->element[REDIS_GAME_SESSION_commander_charPosition]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.unk6,
            session->currentCommander.mapId = strtol (reply->element[REDIS_GAME_SESSION_commander_mapId]->str, NULL, 16);
            session->currentCommander.currentXP = strtol (reply->element[REDIS_GAME_SESSION_commander_currentXP]->str, NULL, 16);
            session->currentCommander.maxXP = strtol (reply->element[REDIS_GAME_SESSION_commander_maxXP]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.unk7,
            session->currentCommander.pose = strtol (reply->element[REDIS_GAME_SESSION_commander_pose]->str, NULL, 16);
            session->currentCommander.spriteRelated = strtol (reply->element[REDIS_GAME_SESSION_commander_spriteRelated]->str, NULL, 16);
            session->currentCommander.currentHP = strtol (reply->element[REDIS_GAME_SESSION_commander_currentHP]->str, NULL, 16);
            session->currentCommander.maxHP = strtol (reply->element[REDIS_GAME_SESSION_commander_maxHP]->str, NULL, 16);
            session->currentCommander.currentSP = strtol (reply->element[REDIS_GAME_SESSION_commander_currentSP]->str, NULL, 16);
            session->currentCommander.maxSP = strtol (reply->element[REDIS_GAME_SESSION_commander_maxSP]->str, NULL, 16);
            // [UNKNOWN] session->currentCommander.unk8,
            // [UNKNOWN] session->currentCommander.unk9,
            // [UNKNOWN] session->currentCommander.unk10,
            // [UNKNOWN] session->currentCommander.unk11,
            // [UNKNOWN] session->currentCommander.unk12,
        }
        break;

        default : warning ("Unexpected Redis status (%d).", reply->type); return false;
    }

    return true;
}



bool
Redis_updateGameSession (
    Redis *self,
    GameSession *session
) {
    redisReply *reply = NULL;
    char key[100];

    if (session->socketSession.isInBarrack) {
        snprintf (key, sizeof (key), "barrack%llx", session->socketSession.accountId);
    } else {
        snprintf (key, sizeof (key), "zone%d:map%d:acc%llx",
            session->socketSession.zoneId, session->socketSession.mapId, session->socketSession.accountId);
    }

    reply = Redis_commandDbg (self,
        "HMSET %s"
        " " REDIS_GAME_SESSION_zoneId_str " " "%x"
        " " REDIS_GAME_SESSION_familyName_str " %s"
        " " REDIS_GAME_SESSION_currentCommanderName_str " %s"
        " " REDIS_GAME_SESSION_charactersBarrackCount_str " %x"
        " " REDIS_GAME_SESSION_accountId_str " %llx"
        " " REDIS_GAME_SESSION_currentCommanderId_str " %llx"
        " " REDIS_GAME_SESSION_currentPcId_str " %x"
        // [UNKNOWN] " " commander.unk1 " "%s"
        " " REDIS_GAME_SESSION_commander_classId_str " %x"
        // [UNKNOWN] " " REDIS_GAME_SESSION_commander_unk2 " "%s"
        " " REDIS_GAME_SESSION_commander_jobId_str " %x"
        " " REDIS_GAME_SESSION_commander_gender_str " %x"
        // [UNKNOWN] "commander_unk3 " " %x"
        " " REDIS_GAME_SESSION_commander_level_str " %x"
        " " REDIS_GAME_SESSION_commander_head_top_str " %x"
        " " REDIS_GAME_SESSION_commander_head_middle_str " %x"
        // [UNKNOWN] "commander_itemUnk1 " " %x"
        " " REDIS_GAME_SESSION_commander_body_armor_str " %x"
        " " REDIS_GAME_SESSION_commander_gloves_str " %x"
        " " REDIS_GAME_SESSION_commander_boots_str " %x"
        // [UNKNOWN] "commander_itemUnk2 " " %x"
        " " REDIS_GAME_SESSION_commander_bracelet_str " %x"
        " " REDIS_GAME_SESSION_commander_weapon_str " %x"
        " " REDIS_GAME_SESSION_commander_shield_str " %x"
        " " REDIS_GAME_SESSION_commander_costume_str " %x"
        // [UNKNOWN] "commander_itemUnk3 " " %x"
        // [UNKNOWN] "commander_itemUnk4 " " %x"
        // [UNKNOWN] "commander_itemUnk5 " " %x"
        " " REDIS_GAME_SESSION_commander_leg_armor_str " %x"
        // [UNKNOWN] "commander_itemUnk6 " " %x"
        // [UNKNOWN] "commander_itemUnk7 " " %x"
        " " REDIS_GAME_SESSION_commander_ring_left_str " %x"
        " " REDIS_GAME_SESSION_commander_ring_right_str " %x"
        " " REDIS_GAME_SESSION_commander_necklace_str " %x"
        " " REDIS_GAME_SESSION_commander_hairType_str " %x"
        // [UNKNOWN] "commander_unk4 " " %x"
        // [UNKNOWN] "commander_unk5 " " %x"
        " " REDIS_GAME_SESSION_commander_pcId_str " %x"
        " " REDIS_GAME_SESSION_commander_listPosition_str " %x"
        " " REDIS_GAME_SESSION_commander_charPosition_str " %x"
        // [UNKNOWN] "commander_unk6 " " %x"
        " " REDIS_GAME_SESSION_commander_mapId_str " %x"
        " " REDIS_GAME_SESSION_commander_currentXP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxXP_str " %x"
        // [UNKNOWN] "commander_unk7 " " %x"
        " " REDIS_GAME_SESSION_commander_pose_str " %x"
        " " REDIS_GAME_SESSION_commander_spriteRelated_str " %x"
        " " REDIS_GAME_SESSION_commander_currentHP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxHP_str " %x"
        " " REDIS_GAME_SESSION_commander_currentSP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxSP_str " %x"
        , key,
        session->socketSession.zoneId,
        (session->familyName[0] != '\0') ? session->familyName : REDIS_EMPTY_STRING,
        (session->currentCommanderName[0] != '\0') ? session->currentCommanderName : REDIS_EMPTY_STRING,
        session->charactersBarrackCount,
        session->socketSession.accountId,
        session->currentCommanderId,
        session->currentPcId,
        // [UNKNOWN] session->currentCommander.unk1,
        session->currentCommander.classId,
        // [UNKNOWN] session->currentCommander.unk2,
        session->currentCommander.jobId,
        session->currentCommander.gender,
        // [UNKNOWN] session->currentCommander.unk3,
        session->currentCommander.level,
        session->currentCommander.head_top,
        session->currentCommander.head_middle,
        // [UNKNOWN] session->currentCommander.itemUnk1,
        session->currentCommander.body_armor,
        session->currentCommander.gloves,
        session->currentCommander.boots,
        // [UNKNOWN] session->currentCommander.itemUnk2,
        session->currentCommander.bracelet,
        session->currentCommander.weapon,
        session->currentCommander.shield,
        session->currentCommander.costume,
        // [UNKNOWN] session->currentCommander.itemUnk3,
        // [UNKNOWN] session->currentCommander.itemUnk4,
        // [UNKNOWN] session->currentCommander.itemUnk5,
        session->currentCommander.leg_armor,
        // [UNKNOWN] session->currentCommander.itemUnk6,
        // [UNKNOWN] session->currentCommander.itemUnk7,
        session->currentCommander.ring_left,
        session->currentCommander.ring_right,
        session->currentCommander.necklace,
        session->currentCommander.hairType,
        // [UNKNOWN] session->currentCommander.unk4,
        // [UNKNOWN] session->currentCommander.unk5,
        session->currentCommander.pcId,
        session->currentCommander.listPosition,
        session->currentCommander.charPosition,
        // [UNKNOWN] session->currentCommander.unk6,
        session->currentCommander.mapId,
        session->currentCommander.currentXP,
        session->currentCommander.maxXP,
        // [UNKNOWN] session->currentCommander.unk7,
        session->currentCommander.pose,
        session->currentCommander.spriteRelated,
        session->currentCommander.currentHP,
        session->currentCommander.maxHP,
        session->currentCommander.currentSP,
        session->currentCommander.maxSP
        // [UNKNOWN] session->currentCommander.unk8,
        // [UNKNOWN] session->currentCommander.unk9,
        // [UNKNOWN] session->currentCommander.unk10,
        // [UNKNOWN] session->currentCommander.unk11,
        // [UNKNOWN] session->currentCommander.unk12,
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            return false;
        break;

        case REDIS_REPLY_STATUS:
            info ("Redis status : %s", reply->str);
        break;

        default : warning ("Unexpected Redis status."); return false;
    }

    return true;
}
