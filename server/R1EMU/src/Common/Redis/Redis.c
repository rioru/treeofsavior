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
#include "Redis.h"


// ------ Structure declaration -------
/**
 * @brief Redis is a structure containing the necessary variables for communicating with the redis server
 */
struct Redis
{
    /** Redis context, handle of the connection to the redis server */
    redisContext *context;
};


// ------ Static declaration -------


// ------ Extern function implementation -------

Redis *
Redis_new (
    char *ip,
    int port
) {
    Redis *self;

    if ((self = calloc (1, sizeof (Redis))) == NULL) {
        return NULL;
    }

    if (!Redis_init (self, ip, port)) {
        Redis_destroy (&self);
        error ("Redis failed to initialize.");
        return NULL;
    }

    return self;
}


bool
Redis_init (
    Redis *self,
    char *ip,
    int port
) {
    self->context = redisConnect (ip, port);

    return true;
}

void
Redis_getSession (
    Redis *self,
    ClientSession *session
) {

}

bool
Redis_setSession (
    Redis *self,
    ClientSession *session
) {
    redisReply *reply = redisCommand (
        self->context,
        "HMSET zone%d:map%d:acc%d"
        " zoneId " QUOTIFY("%x")
        " familyName " QUOTIFY("%s")
        " currentCommanderName " QUOTIFY("%s")
        " charactersBarrackCount " QUOTIFY("%x")
        " accountId " QUOTIFY("%llu")
        " currentCommanderId " QUOTIFY("%llu")
        " currentPcId " QUOTIFY("%x")
        // [UNKNOWN] " commander.unk1 " QUOTIFY("%s")
        " commander.classId " QUOTIFY("%x")
        // [UNKNOWN] " commander.unk2 " QUOTIFY("%s")
        " commander.jobId " QUOTIFY("%x")
        " commander.gender " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk3 " QUOTIFY("%x")
        " commander.level " QUOTIFY("%x")
        " commander.head_top " QUOTIFY("%x")
        " commander.head_middle " QUOTIFY("%x")
        // [UNKNOWN] "commander.itemUnk1 " QUOTIFY("%x")
        " commander.body_armor " QUOTIFY("%x")
        " commander.gloves " QUOTIFY("%x")
        " commander.boots " QUOTIFY("%x")
        // [UNKNOWN] "commander.itemUnk2 " QUOTIFY("%x")
        " commander.bracelet " QUOTIFY("%x")
        " commander.weapon " QUOTIFY("%x")
        " commander.shield " QUOTIFY("%x")
        " commander.costume " QUOTIFY("%x")
        // [UNKNOWN] "commander.itemUnk3 " QUOTIFY("%x")
        // [UNKNOWN] "commander.itemUnk4 " QUOTIFY("%x")
        // [UNKNOWN] "commander.itemUnk5 " QUOTIFY("%x")
        " commander.leg_armor " QUOTIFY("%x")
        // [UNKNOWN] "commander.itemUnk6 " QUOTIFY("%x")
        // [UNKNOWN] "commander.itemUnk7 " QUOTIFY("%x")
        " commander.ring_left " QUOTIFY("%x")
        " commander.ring_right " QUOTIFY("%x")
        " commander.necklace " QUOTIFY("%x")
        " commander.hairType " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk4 " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk5 " QUOTIFY("%x")
        " commander.pcId " QUOTIFY("%x")
        " commander.listPosition " QUOTIFY("%x")
        " commander.charPosition " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk6 " QUOTIFY("%x")
        " commander.mapId " QUOTIFY("%x")
        " commander.currentXP " QUOTIFY("%x")
        " commander.maxXP " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk7 " QUOTIFY("%x")
        " commander.pose " QUOTIFY("%x")
        " commander.spriteRelated " QUOTIFY("%x")
        " commander.currentHP " QUOTIFY("%x")
        " commander.maxHP " QUOTIFY("%x")
        " commander.currentSP " QUOTIFY("%x")
        " commander.maxSP " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk8 " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk9 " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk10 " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk11 " QUOTIFY("%x")
        // [UNKNOWN] "commander.unk12 " QUOTIFY("%x")
        ,
        session->zoneId, session->currentCommander.mapId, session->currentCommander.accountId, // Identifiers
        session->zoneId,
        session->familyName,
        session->currentCommanderName,
        session->charactersBarrackCount,
        session->accountId,
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


void
Redis_destroy (
    Redis **_self
) {
    Redis *self = *_self;

    if (self->context) {
        redisFree (self->context);
    }

    free (self);
    *_self = NULL;
}
