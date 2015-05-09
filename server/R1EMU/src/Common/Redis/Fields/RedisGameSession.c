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
	[REDIS_GAME_SESSION_socketKey]               = REDIS_GAME_SESSION_socketKey_str,
	[REDIS_GAME_SESSION_routerId]                = REDIS_GAME_SESSION_routerId_str,
	[REDIS_GAME_SESSION_familyName]              = REDIS_GAME_SESSION_familyName_str,
	[REDIS_GAME_SESSION_commanderName]           = REDIS_GAME_SESSION_commanderName_str,
	[REDIS_GAME_SESSION_charactersBarrackCount]  = REDIS_GAME_SESSION_charactersBarrackCount_str,
	[REDIS_GAME_SESSION_accountId]               = REDIS_GAME_SESSION_accountId_str,
	[REDIS_GAME_SESSION_currentCommanderId]      = REDIS_GAME_SESSION_currentCommanderId_str,
	[REDIS_GAME_SESSION_currentPcId]             = REDIS_GAME_SESSION_currentPcId_str,
	[REDIS_GAME_SESSION_commander_classId]       = REDIS_GAME_SESSION_commander_classId_str,
	[REDIS_GAME_SESSION_commander_jobId]         = REDIS_GAME_SESSION_commander_jobId_str,
	[REDIS_GAME_SESSION_commander_gender]        = REDIS_GAME_SESSION_commander_gender_str,
	[REDIS_GAME_SESSION_commander_level]         = REDIS_GAME_SESSION_commander_level_str,
	[REDIS_GAME_SESSION_commander_head_top]      = REDIS_GAME_SESSION_commander_head_top_str,
	[REDIS_GAME_SESSION_commander_head_middle]   = REDIS_GAME_SESSION_commander_head_middle_str,
	[REDIS_GAME_SESSION_commander_itemUnk1]      = REDIS_GAME_SESSION_commander_itemUnk1_str,
	[REDIS_GAME_SESSION_commander_body_armor]    = REDIS_GAME_SESSION_commander_body_armor_str,
	[REDIS_GAME_SESSION_commander_gloves]        = REDIS_GAME_SESSION_commander_gloves_str,
	[REDIS_GAME_SESSION_commander_boots]         = REDIS_GAME_SESSION_commander_boots_str,
	[REDIS_GAME_SESSION_commander_bracelet]      = REDIS_GAME_SESSION_commander_bracelet_str,
	[REDIS_GAME_SESSION_commander_weapon]        = REDIS_GAME_SESSION_commander_weapon_str,
	[REDIS_GAME_SESSION_commander_shield]        = REDIS_GAME_SESSION_commander_shield_str,
	[REDIS_GAME_SESSION_commander_costume]       = REDIS_GAME_SESSION_commander_costume_str,
	[REDIS_GAME_SESSION_commander_itemUnk3]      = REDIS_GAME_SESSION_commander_itemUnk3_str,
	[REDIS_GAME_SESSION_commander_itemUnk4]      = REDIS_GAME_SESSION_commander_itemUnk4_str,
	[REDIS_GAME_SESSION_commander_itemUnk5]      = REDIS_GAME_SESSION_commander_itemUnk5_str,
	[REDIS_GAME_SESSION_commander_leg_armor]     = REDIS_GAME_SESSION_commander_leg_armor_str,
	[REDIS_GAME_SESSION_commander_itemUnk6]      = REDIS_GAME_SESSION_commander_itemUnk6_str,
	[REDIS_GAME_SESSION_commander_itemUnk7]      = REDIS_GAME_SESSION_commander_itemUnk7_str,
	[REDIS_GAME_SESSION_commander_ring_left]     = REDIS_GAME_SESSION_commander_ring_left_str,
	[REDIS_GAME_SESSION_commander_ring_right]    = REDIS_GAME_SESSION_commander_ring_right_str,
	[REDIS_GAME_SESSION_commander_necklace]      = REDIS_GAME_SESSION_commander_necklace_str,
	[REDIS_GAME_SESSION_commander_hairType]      = REDIS_GAME_SESSION_commander_hairType_str,
	[REDIS_GAME_SESSION_commander_pcId]          = REDIS_GAME_SESSION_commander_pcId_str,
	[REDIS_GAME_SESSION_commander_unk13]         = REDIS_GAME_SESSION_commander_unk13_str,
	[REDIS_GAME_SESSION_commander_charPosition]  = REDIS_GAME_SESSION_commander_charPosition_str,
	[REDIS_GAME_SESSION_commander_mapId]         = REDIS_GAME_SESSION_commander_mapId_str,
	[REDIS_GAME_SESSION_commander_currentXP]     = REDIS_GAME_SESSION_commander_currentXP_str,
	[REDIS_GAME_SESSION_commander_maxXP]         = REDIS_GAME_SESSION_commander_maxXP_str,
	[REDIS_GAME_SESSION_commander_pose]          = REDIS_GAME_SESSION_commander_pose_str,
	[REDIS_GAME_SESSION_commander_spriteRelated] = REDIS_GAME_SESSION_commander_spriteRelated_str,
	[REDIS_GAME_SESSION_commander_currentHP]     = REDIS_GAME_SESSION_commander_currentHP_str,
	[REDIS_GAME_SESSION_commander_maxHP]         = REDIS_GAME_SESSION_commander_maxHP_str,
	[REDIS_GAME_SESSION_commander_currentSP]     = REDIS_GAME_SESSION_commander_currentSP_str,
	[REDIS_GAME_SESSION_commander_maxSP]         = REDIS_GAME_SESSION_commander_maxSP_str,
	[REDIS_GAME_SESSION_commander_cPosX]         = REDIS_GAME_SESSION_commander_cPosX_str,
	[REDIS_GAME_SESSION_commander_cPosY]         = REDIS_GAME_SESSION_commander_cPosY_str
};


// ------ Extern functions implementation -------

bool
Redis_requestSession (
    Redis *self,
    int routerId,
    char *socketId,
    Session *session
) {
    GameSession *gameSession = &session->game;
    SocketSession *socketSession = &session->socket;

    unsigned char socketKey[SOCKET_SESSION_KEY_SIZE];

    // Generate the socketId key
    SocketSession_genKey (socketId, socketKey);

    // Search for the Socket Session
    if (!Redis_getSocketSession (self, routerId, socketKey, socketSession)) {
        error ("Cannot get Socket Session.");
        return false;
    }

    if (!socketSession->authenticated) {
        // This is the first time the client connect.
        // Initialize an empty game session
        GameSession_init (gameSession);
        dbg ("Welcome, SOCKET_%s ! A new session has been initialized for you.", socketKey);
    } else {
        if (!Redis_getGameSession (self, session)) {
            error ("Cannot get Game Session.");
            return false;
        }
        dbg ("Welcome back, SOCKET_%s !", socketKey);
    }

    return true;
}

bool
Redis_getGameSession (
    Redis *self,
    Session *session
) {
    GameSession *gameSession = &session->game;
    SocketSession *socketSession = &session->socket;

    redisReply *reply = NULL;

    reply = Redis_commandDbg (self,
        "HMGET zone%x:map%x:acc%llx"
        " " REDIS_GAME_SESSION_socketKey_str
        " " REDIS_GAME_SESSION_routerId_str
        " " REDIS_GAME_SESSION_familyName_str
        " " REDIS_GAME_SESSION_commanderName_str
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
        " " REDIS_GAME_SESSION_commander_pose_str
        " " REDIS_GAME_SESSION_commander_spriteRelated_str
        " " REDIS_GAME_SESSION_commander_currentHP_str
        " " REDIS_GAME_SESSION_commander_maxHP_str
        " " REDIS_GAME_SESSION_commander_currentSP_str
        " " REDIS_GAME_SESSION_commander_maxSP_str
        " " REDIS_GAME_SESSION_commander_cPosX_str
        " " REDIS_GAME_SESSION_commander_cPosY_str
        // [UNKNOWN] "commander.unk10 "
        // [UNKNOWN] "commander.unk11 "
        // [UNKNOWN] "commander.unk12 "
        , socketSession->routerId, socketSession->mapId, socketSession->accountId
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

            // Write the reply to the session
            strncpy (gameSession->socketKey, reply->element[REDIS_GAME_SESSION_socketKey]->str, sizeof (gameSession->socketKey));
            strncpy (gameSession->currentCommander.familyName, reply->element[REDIS_GAME_SESSION_familyName]->str, sizeof (gameSession->currentCommander.familyName));
            strncpy (gameSession->currentCommander.charName, reply->element[REDIS_GAME_SESSION_commanderName]->str, sizeof (gameSession->currentCommander.charName));

            gameSession->charactersBarrackCount         = strtol (reply->element[REDIS_GAME_SESSION_charactersBarrackCount]->str, NULL, 16);
            gameSession->currentCommanderId             = strtoll (reply->element[REDIS_GAME_SESSION_currentCommanderId]->str, NULL, 16);
            gameSession->currentPcId                    = strtoll (reply->element[REDIS_GAME_SESSION_currentPcId]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk1,
            gameSession->currentCommander.classId       = strtol (reply->element[REDIS_GAME_SESSION_commander_classId]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk2,
            gameSession->currentCommander.jobId         = strtol (reply->element[REDIS_GAME_SESSION_commander_jobId]->str, NULL, 16);
            gameSession->currentCommander.gender        = strtol (reply->element[REDIS_GAME_SESSION_commander_gender]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk3,
            gameSession->currentCommander.level         = strtol (reply->element[REDIS_GAME_SESSION_commander_level]->str, NULL, 16);
            gameSession->currentCommander.head_top      = strtol (reply->element[REDIS_GAME_SESSION_commander_head_top]->str, NULL, 16);
            gameSession->currentCommander.head_middle   = strtol (reply->element[REDIS_GAME_SESSION_commander_head_middle]->str, NULL, 16);
            gameSession->currentCommander.itemUnk1      = strtol (reply->element[REDIS_GAME_SESSION_commander_itemUnk1]->str, NULL, 16);
            gameSession->currentCommander.body_armor    = strtol (reply->element[REDIS_GAME_SESSION_commander_body_armor]->str, NULL, 16);
            gameSession->currentCommander.gloves        = strtol (reply->element[REDIS_GAME_SESSION_commander_gloves]->str, NULL, 16);
            gameSession->currentCommander.boots         = strtol (reply->element[REDIS_GAME_SESSION_commander_boots]->str, NULL, 16);
            gameSession->currentCommander.itemUnk2      = strtol (reply->element[REDIS_GAME_SESSION_commander_itemUnk2]->str, NULL, 16);
            gameSession->currentCommander.bracelet      = strtol (reply->element[REDIS_GAME_SESSION_commander_bracelet]->str, NULL, 16);
            gameSession->currentCommander.weapon        = strtol (reply->element[REDIS_GAME_SESSION_commander_weapon]->str, NULL, 16);
            gameSession->currentCommander.shield        = strtol (reply->element[REDIS_GAME_SESSION_commander_shield]->str, NULL, 16);
            gameSession->currentCommander.costume       = strtol (reply->element[REDIS_GAME_SESSION_commander_costume]->str, NULL, 16);
            gameSession->currentCommander.itemUnk3      = strtol (reply->element[REDIS_GAME_SESSION_commander_itemUnk3]->str, NULL, 16);
            gameSession->currentCommander.itemUnk4      = strtol (reply->element[REDIS_GAME_SESSION_commander_itemUnk4]->str, NULL, 16);
            gameSession->currentCommander.itemUnk5      = strtol (reply->element[REDIS_GAME_SESSION_commander_itemUnk5]->str, NULL, 16);
            gameSession->currentCommander.leg_armor     = strtol (reply->element[REDIS_GAME_SESSION_commander_leg_armor]->str, NULL, 16);
            gameSession->currentCommander.itemUnk6      = strtol (reply->element[REDIS_GAME_SESSION_commander_itemUnk6]->str, NULL, 16);
            gameSession->currentCommander.itemUnk7      = strtol (reply->element[REDIS_GAME_SESSION_commander_itemUnk7]->str, NULL, 16);
            gameSession->currentCommander.ring_left     = strtol (reply->element[REDIS_GAME_SESSION_commander_ring_left]->str, NULL, 16);
            gameSession->currentCommander.ring_right    = strtol (reply->element[REDIS_GAME_SESSION_commander_ring_right]->str, NULL, 16);
            gameSession->currentCommander.necklace      = strtol (reply->element[REDIS_GAME_SESSION_commander_necklace]->str, NULL, 16);
            gameSession->currentCommander.hairType      = strtol (reply->element[REDIS_GAME_SESSION_commander_hairType]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk4,
            // [UNKNOWN] gameSession->currentCommander.unk5,
            gameSession->currentCommander.pcId          = strtol (reply->element[REDIS_GAME_SESSION_commander_pcId]->str, NULL, 16);
            gameSession->currentCommander.unk13         = strtol (reply->element[REDIS_GAME_SESSION_commander_unk13]->str, NULL, 16);
            gameSession->currentCommander.charPosition  = strtol (reply->element[REDIS_GAME_SESSION_commander_charPosition]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk6,
            gameSession->currentCommander.mapId         = strtol (reply->element[REDIS_GAME_SESSION_commander_mapId]->str, NULL, 16);
            gameSession->currentCommander.currentXP     = strtol (reply->element[REDIS_GAME_SESSION_commander_currentXP]->str, NULL, 16);
            gameSession->currentCommander.maxXP         = strtol (reply->element[REDIS_GAME_SESSION_commander_maxXP]->str, NULL, 16);
            // [UNKNOWN] gameSession->currentCommander.unk7,
            gameSession->currentCommander.pose          = strtol (reply->element[REDIS_GAME_SESSION_commander_pose]->str, NULL, 16);
            gameSession->currentCommander.spriteRelated = strtol (reply->element[REDIS_GAME_SESSION_commander_spriteRelated]->str, NULL, 16);
            gameSession->currentCommander.currentHP     = strtol (reply->element[REDIS_GAME_SESSION_commander_currentHP]->str, NULL, 16);
            gameSession->currentCommander.maxHP         = strtol (reply->element[REDIS_GAME_SESSION_commander_maxHP]->str, NULL, 16);
            gameSession->currentCommander.currentSP     = strtol (reply->element[REDIS_GAME_SESSION_commander_currentSP]->str, NULL, 16);
            gameSession->currentCommander.maxSP         = strtol (reply->element[REDIS_GAME_SESSION_commander_maxSP]->str, NULL, 16);
            gameSession->currentCommander.cPosX         = strtof (reply->element[REDIS_GAME_SESSION_commander_cPosX]->str, NULL);
            gameSession->currentCommander.cPosY         = strtof (reply->element[REDIS_GAME_SESSION_commander_cPosY]->str, NULL);
            // [UNKNOWN] gameSession->currentCommander.unk10,
            // [UNKNOWN] gameSession->currentCommander.unk11,
            // [UNKNOWN] gameSession->currentCommander.unk12,
        }
        break;

        default : warning ("Unexpected Redis status (%d).", reply->type); return false;
    }

    return true;
}

bool
Redis_updateGameSession (
    Redis *self,
    SocketSession *socketSession,
    GameSession *gameSession
) {
    redisReply *reply = NULL;

    reply = Redis_commandDbg (self,
        "HMSET zone%x:map%x:acc%llx"
        " " REDIS_GAME_SESSION_socketKey_str " %s"
        " " REDIS_GAME_SESSION_routerId_str " %x"
        " " REDIS_GAME_SESSION_familyName_str " %s"
        " " REDIS_GAME_SESSION_commanderName_str " %s"
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
        " " REDIS_GAME_SESSION_commander_pose_str " %x"
        " " REDIS_GAME_SESSION_commander_spriteRelated_str " %x"
        " " REDIS_GAME_SESSION_commander_currentHP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxHP_str " %x"
        " " REDIS_GAME_SESSION_commander_currentSP_str " %x"
        " " REDIS_GAME_SESSION_commander_maxSP_str " %x"
        " " REDIS_GAME_SESSION_commander_cPosX_str " %f"
        " " REDIS_GAME_SESSION_commander_cPosY_str " %f"
        , socketSession->routerId, socketSession->mapId, socketSession->accountId,
        socketSession->key,
        socketSession->routerId,
        (gameSession->currentCommander.familyName[0] != '\0') ? gameSession->currentCommander.familyName : REDIS_EMPTY_STRING,
        (gameSession->currentCommander.charName[0] != '\0') ? gameSession->currentCommander.charName : REDIS_EMPTY_STRING,
        gameSession->charactersBarrackCount,
        socketSession->accountId,
        gameSession->currentCommanderId,
        gameSession->currentPcId,
        // [UNKNOWN] gameSession->currentCommander.unk1,
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
        gameSession->currentCommander.pose,
        gameSession->currentCommander.spriteRelated,
        gameSession->currentCommander.currentHP,
        gameSession->currentCommander.maxHP,
        gameSession->currentCommander.currentSP,
        gameSession->currentCommander.maxSP,
        gameSession->currentCommander.cPosX,
        gameSession->currentCommander.cPosY
        // [UNKNOWN] gameSession->currentCommander.unk8,
        // [UNKNOWN] gameSession->currentCommander.unk9,
        // [UNKNOWN] gameSession->currentCommander.unk10,
        // [UNKNOWN] gameSession->currentCommander.unk11,
        // [UNKNOWN] gameSession->currentCommander.unk12,
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

        default : warning ("Unexpected Redis status. (%d)", reply->type); return false;
    }

    return true;
}

zlist_t *
Redis_getClientsWithinDistance (
    Redis *self,
    uint16_t routerId, uint16_t mapId,
    float posX, float posY, float posZ,
    float range
) {
    zlist_t *clients;
    redisReply *reply;

    // TODO : Could be better. Don't allocate a new zlist everytime we call this function.
    // Who got this list then ? The Worker?
    if (!(clients = zlist_new ())) {
        error ("Cannot allocate a new zlist.");
        return NULL;
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
            return false;
        }

        switch (reply->type) {
            case REDIS_REPLY_ERROR:
                error ("Redis error encountered : %s", reply->str);
                return false;
            break;

            case REDIS_REPLY_ARRAY: {
                // [0] = new iterator
                iterator = strtol (reply->element[0]->str, NULL, 10);
                // [1] = results
                for (int i = 0; i < reply->element[1]->elements; i++) {
                    // Get the position of all accounts in the map
                    redisReply *posReply = Redis_commandDbg (self,
                        "HMGET %s " REDIS_GAME_SESSION_commander_cPosX_str
                                " " REDIS_GAME_SESSION_commander_cPosY_str // Get position
                                " " REDIS_GAME_SESSION_socketKey_str, // SocketKey
                        reply->element[1]->element[i]->str // account key
                    );

                    if (!posReply) {
                        error ("Redis error encountered : The request is invalid.");
                        return false;
                    }

                    switch (posReply->type) {

                        case REDIS_REPLY_ERROR:
                            error ("Redis error encountered : %s", reply->str);
                            return false;
                        break;

                        case REDIS_REPLY_STATUS:
                            info ("Redis status : %s", reply->str);
                        break;

                        case REDIS_REPLY_ARRAY: {
                            if (posReply->elements != 3) {
                                error ("Abnormal number of elements (%d, should be 3).", posReply->elements);
                                return false;
                            }

                            // [0] = X, [1] = Y, [2] = socketKey
                            float x = strtof (posReply->element[0]->str, NULL),
                                  y = strtof (posReply->element[1]->str, NULL);
                            char *socketKey = posReply->element[2]->str;

                            // Check range here
                            if (Math_isWithin2DManhattanDistance (x, y, posX, posY, 300.0)) {
                                // The current client is within the area, add it to the list
                                zlist_append (clients, strdup (socketKey));
                            }
                        } break;

                        default : warning ("Unexpected Redis status. (%d)", reply->type); return NULL;
                    }

                    freeReplyObject (posReply);
                }
            } break;

            default : warning ("Unexpected Redis status. (%d)", reply->type); return NULL;
        }

        freeReplyObject (reply);

    } while (iterator != 0);

    return clients;
}

