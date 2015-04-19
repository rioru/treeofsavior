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
#include "RedisSocketSession.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern variables implementation -------
const char *redisSocketSessionsStr [] = {
	[REDIS_SOCKET_SESSION_accountId] = REDIS_SOCKET_SESSION_accountId_str,
	[REDIS_SOCKET_SESSION_zoneId] = REDIS_SOCKET_SESSION_zoneId_str,
	[REDIS_SOCKET_SESSION_mapId] = REDIS_SOCKET_SESSION_mapId_str,
	[REDIS_SOCKET_SESSION_authenticated] = REDIS_SOCKET_SESSION_authenticated_str,
	[REDIS_SOCKET_SESSION_isInBarrack] = REDIS_SOCKET_SESSION_isInBarrack_str
};


// ------ Extern functions implementation -------
bool
Redis_getSocketSession (
    Redis *self,
    char *socketIdKey,
    SocketSession *socketSession
) {
	redisReply *reply = NULL;

	reply = Redis_commandDbg (self,
        "HMGET socket%s "
        REDIS_SOCKET_SESSION_accountId_str " "
        REDIS_SOCKET_SESSION_zoneId_str " "
        REDIS_SOCKET_SESSION_mapId_str " "
        REDIS_SOCKET_SESSION_authenticated_str " "
        REDIS_SOCKET_SESSION_isInBarrack_str " ",
        socketIdKey
    );

    if (!reply) {
        error ("Redis error encountered : The request is invalid.");
        return false;
    }

    switch (reply->type)
    {
        case REDIS_REPLY_ERROR:
            error ("Redis error encountered : %s", reply->str);
            freeReplyObject (reply);
            return false;
        break;

        case REDIS_REPLY_STATUS:
            info ("Redis status : %s", reply->str);
            freeReplyObject (reply);
            return false;
        break;

        case REDIS_REPLY_ARRAY:
            if (reply->elements != REDIS_SOCKET_SESSION_COUNT) {
                error ("Wrong number of elements received.");
                freeReplyObject (reply);
                return false;
            }

            if (Redis_anyElementIsNull (reply->element, reply->elements) != -1) {
                // The socket ID doesn't exist : set them to their default values
                SocketSession_init (socketSession, 0, 0, 0, socketIdKey);

                // Update the newly created socketSession to the Redis Session
                if (!Redis_updateSocketSession (self, socketSession)) {
                    dbg ("Cannot update the socket session");
                    freeReplyObject (reply);
                    return false;
                }
            }
            else {
                Redis_printElements (reply->element, reply->elements, redisSocketSessionsStr);

                // Read the socket Session from the Redis server
                socketSession->accountId = strtoll (reply->element[REDIS_SOCKET_SESSION_accountId]->str, NULL, 16);
                socketSession->zoneId = strtol (reply->element[REDIS_SOCKET_SESSION_zoneId]->str, NULL, 16);
                socketSession->mapId = strtol (reply->element[REDIS_SOCKET_SESSION_mapId]->str, NULL, 16);
                socketSession->authenticated = strtol (reply->element[REDIS_SOCKET_SESSION_authenticated]->str, NULL, 16);
                socketSession->isInBarrack = strtol (reply->element[REDIS_SOCKET_SESSION_isInBarrack]->str, NULL, 16);
            }
        break;

        default :
            warning ("Unexpected Redis status : %d", reply->type);
            freeReplyObject (reply);
            return false;
        break;
    }

    freeReplyObject (reply);
    return true;
}


bool
Redis_updateSocketSession (
    Redis *self,
    SocketSession *socketSession
) {
    redisReply *reply = NULL;

    reply = Redis_commandDbg (self,
        "HMSET socket%s"
        " accountId %llx"
        " zoneId %x"
        " mapId %x"
        " authenticated %x"
        " isInBarrack %x",
        socketSession->key,
        socketSession->accountId,
        socketSession->zoneId,
        socketSession->mapId,
        socketSession->authenticated,
        socketSession->isInBarrack
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
