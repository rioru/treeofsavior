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
	[REDIS_SOCKET_SESSION_routerId] = REDIS_SOCKET_SESSION_routerId_str,
	[REDIS_SOCKET_SESSION_mapId] = REDIS_SOCKET_SESSION_mapId_str,
	[REDIS_SOCKET_SESSION_authenticated] = REDIS_SOCKET_SESSION_authenticated_str
};


// ------ Extern functions implementation -------
bool
Redis_getSocketSession (
    Redis *self,
    int routerId,
    char *socketIdKey,
    SocketSession *socketSession
) {
	redisReply *reply = NULL;

	reply = Redis_commandDbg (self,
        "HMGET zone%x:socket%s "
        REDIS_SOCKET_SESSION_accountId_str " "
        REDIS_SOCKET_SESSION_routerId_str " "
        REDIS_SOCKET_SESSION_mapId_str " "
        REDIS_SOCKET_SESSION_authenticated_str " ",
        routerId, socketIdKey
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
                // The socket session doesn't exist : create a new one and set it to its default value
                SocketSession_init (socketSession, SOCKET_SESSION_UNDEFINED_ACCOUNT, routerId, SOCKET_SESSION_UNDEFINED_MAP, socketIdKey, false);

                // Update the newly created socketSession to the Redis Session
                if (!Redis_updateSocketSession (self, socketSession)) {
                    dbg ("Cannot update the socket session");
                    freeReplyObject (reply);
                    return false;
                }
            }
            else {
                // Read the socket Session from the Redis server
                socketSession->accountId = strtoll (reply->element[REDIS_SOCKET_SESSION_accountId]->str, NULL, 16);
                socketSession->routerId = strtol (reply->element[REDIS_SOCKET_SESSION_routerId]->str, NULL, 16);
                socketSession->mapId = strtol (reply->element[REDIS_SOCKET_SESSION_mapId]->str, NULL, 16);
                socketSession->authenticated = strtol (reply->element[REDIS_SOCKET_SESSION_authenticated]->str, NULL, 16);
                memcpy (socketSession->key, socketIdKey, sizeof (socketSession->key));
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
        "HMSET zone%x:socket%s"
        " accountId %llx"
        " routerId %x"
        " mapId %x"
        " authenticated %x"
        , socketSession->routerId, socketSession->key,
        socketSession->accountId,
        socketSession->routerId,
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

