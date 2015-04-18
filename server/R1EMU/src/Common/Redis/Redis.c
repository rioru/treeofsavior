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

    char *commandBuffer;
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
    info ("Connecting to the Redis Server (%s:%d)...", ip, port);

    while (1) {
        self->context = redisConnect (ip, port);
        //self->context = redisConnectWithTimeout (ip, port, (struct timeval) {.tv_sec = 3, .tv_usec = 0});
        if (self->context != NULL && self->context->err) {
            warning ("Redis server not detected (%s)... Retrying in 3 seconds.", self->context->errstr);
            zclock_sleep (3000);
        } else {
            break;
        }
    }

    info ("Connected to the Redis Server !");

    if (!(self->commandBuffer = calloc (1, REDIS_COMMAND_BUFFER_SIZE))) {
        error ("Cannot allocate a command buffer.");
        return false;
    }

    return true;
}

void
Redis_getSession (
    Redis *self,
    ClientGameSession *session
) {

}

bool
Redis_set (
    Redis *self,
    ClientGameSession *session,
    ...
) {
    va_list args;

	va_start (args, session);

	char * token = va_arg (args, char *);
	int tokenLen;
	int curPos;

    // Build the Redis command
	sprintf (self->commandBuffer, "HMSET zone%d:map%d:acc%I64u ",
        session->socketSession.zoneId, session->currentCommander.mapId, session->socketSession.accountId); // Identifiers

    // Current position to the buffer = end of the HMSET key
    curPos = strlen (self->commandBuffer);

    // Iterate through all the tokens
    while (token != NULL) {
        tokenLen = strlen (token);
        char *value = va_arg (args, char *);
        int valueLen = strlen (value);

        // Check for BoF
        if (curPos + tokenLen + valueLen + 3 >= REDIS_COMMAND_BUFFER_SIZE) {
            // CurPos + value + token + 2*space + 1*'\0'.
            error ("Redis command buffer overflow.");
            return false;
        }

        // Concatenate the token + value at the end of the command
        curPos += sprintf (&self->commandBuffer[curPos], "%s %s ", token, value);

        token = va_arg (args, char *);
    }
	va_end (args);

	self->commandBuffer[curPos] = '\0';

	redisReply *reply = redisCommand (self->context, self->commandBuffer);

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

        default : warning ("Unexpected Redis status : %d.", reply->type); return false;
    }

	return true;
}

bool
Redis_getSocketSession (
    Redis *self,
    char *socketIdKey,
    SocketSession *socketSession
) {
	redisReply *reply = redisCommand (self->context, "HMGET socket%s accountId zoneId mapId authenticated", socketIdKey);

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

        case REDIS_REPLY_ARRAY:
            if (reply->elements != 4) {
                error ("Wrong number of elements received.");
                return false;
            }

            if (reply->element[0]->str == NULL
            ||  reply->element[1]->str == NULL
            ||  reply->element[2]->str == NULL
            ||  reply->element[3]->str == NULL) {
                // The socket ID doesn't exist : set them to their default values
                SocketSession_init (socketSession, 0, 0, 0, socketIdKey);
                if (!Redis_updateSocketSession (self, socketSession)) {
                    dbg ("Cannot update the socket session");
                    return false;
                }
            }
            else {
                socketSession->accountId = strtoll (reply->element[0]->str, NULL, 16);
                socketSession->zoneId = strtol (reply->element[1]->str, NULL, 16);
                socketSession->mapId = strtol (reply->element[2]->str, NULL, 16);
                socketSession->authenticated = strtol (reply->element[3]->str, NULL, 16);
            }

        break;

        default : warning ("Unexpected Redis status : %d", reply->type); return false; break;
    }

    return true;
}

bool
Redis_updateSocketSession (
    Redis *self,
    SocketSession *socketSession
) {
    redisReply *reply = NULL;

    reply = redisCommand (
        self->context,
        "HMSET socket%s"
        " accountId %llx"
        " zoneId %x"
        " mapId %x"
        " authenticated %x",
        socketSession->key,
        socketSession->accountId,
        socketSession->zoneId,
        socketSession->mapId,
        socketSession->authenticated
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

bool
Redis_updateGameSession (
    Redis *self,
    ClientGameSession *session
) {
    redisReply *reply = NULL;

    reply = redisCommand (
        self->context,
        "HMSET zone%d:map%d:acc%llx"
        " zoneId " "%x"
        " familyName " "%s"
        " currentCommanderName " "%s"
        " charactersBarrackCount " "%x"
        " accountId " "%llx"
        " currentCommanderId " "%llx"
        " currentPcId " "%x"
        // [UNKNOWN] " commander.unk1 " ("%s")
        " commander.classId " "%x"
        // [UNKNOWN] " commander.unk2 " ("%s")
        " commander.jobId " "%x"
        " commander.gender " "%x"
        // [UNKNOWN] "commander.unk3 " "%x"
        " commander.level " "%x"
        " commander.head_top " "%x"
        " commander.head_middle " "%x"
        // [UNKNOWN] "commander.itemUnk1 " "%x"
        " commander.body_armor " "%x"
        " commander.gloves " "%x"
        " commander.boots " "%x"
        // [UNKNOWN] "commander.itemUnk2 " "%x"
        " commander.bracelet " "%x"
        " commander.weapon " "%x"
        " commander.shield " "%x"
        " commander.costume " "%x"
        // [UNKNOWN] "commander.itemUnk3 " "%x"
        // [UNKNOWN] "commander.itemUnk4 " "%x"
        // [UNKNOWN] "commander.itemUnk5 " "%x"
        " commander.leg_armor " "%x"
        // [UNKNOWN] "commander.itemUnk6 " "%x"
        // [UNKNOWN] "commander.itemUnk7 " "%x"
        " commander.ring_left " "%x"
        " commander.ring_right " "%x"
        " commander.necklace " "%x"
        " commander.hairType " "%x"
        // [UNKNOWN] "commander.unk4 " "%x"
        // [UNKNOWN] "commander.unk5 " "%x"
        " commander.pcId " "%x"
        " commander.listPosition " "%x"
        " commander.charPosition " "%x"
        // [UNKNOWN] "commander.unk6 " "%x"
        " commander.mapId " "%x"
        " commander.currentXP " "%x"
        " commander.maxXP " "%x"
        // [UNKNOWN] "commander.unk7 " "%x"
        " commander.pose " "%x"
        " commander.spriteRelated " "%x"
        " commander.currentHP " "%x"
        " commander.maxHP " "%x"
        " commander.currentSP " "%x"
        " commander.maxSP " "%x"
        // [UNKNOWN] "commander.unk8 " "%x"
        // [UNKNOWN] "commander.unk9 " "%x"
        // [UNKNOWN] "commander.unk10 " "%x"
        // [UNKNOWN] "commander.unk11 " "%x"
        // [UNKNOWN] "commander.unk12 " "%x"
        ,
        session->socketSession.zoneId, session->currentCommander.mapId, session->socketSession.accountId, // Identifiers
        session->socketSession.zoneId,
        session->familyName,
        session->currentCommanderName,
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


void
Redis_destroy (
    Redis **_self
) {
    Redis *self = *_self;

    if (self->context) {
        redisFree (self->context);
    }

    if (self->commandBuffer) {
        free (self->commandBuffer);
    }

    free (self);
    *_self = NULL;
}
