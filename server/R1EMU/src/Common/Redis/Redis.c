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

// GameSession Fields
#include "Fields/RedisGameSession.h"


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
    void
) {
    Redis *self;

    if ((self = calloc (1, sizeof (Redis))) == NULL) {
        return NULL;
    }

    if (!Redis_init (self)) {
        Redis_destroy (&self);
        error ("Redis failed to initialize.");
        return NULL;
    }

    return self;
}

bool
Redis_init (
    Redis *self
) {
    if (!(self->commandBuffer = calloc (1, REDIS_COMMAND_BUFFER_SIZE))) {
        error ("Cannot allocate a command buffer.");
        return false;
    }

    return true;
}

bool
Redis_connect (
    Redis *self,
    RedisInfo *redisInfo
) {
    info ("Connecting to the Redis Server (%s:%d)...", redisInfo->hostname, atoi(redisInfo->port));

    while (1) {
        self->context = redisConnectWithTimeout (redisInfo->hostname, atoi(redisInfo->port), (struct timeval) {.tv_sec = 3, .tv_usec = 0});

        if (self->context != NULL && self->context->err) {
            warning ("Redis server not detected (%s)... Retrying in 3 seconds.", self->context->errstr);
            zclock_sleep (3000);
        } else {
            // Connection OK
            break;
        }
    }

    info ("Connected to the Redis Server !");
    return false;
}

void
Redis_printElements (
    redisReply **elements,
    size_t nbElements,
    const char **elementsName
) {
    char buffer[100];
    for (size_t i = 0; i < nbElements; i++) {
        char *elementName;

        if (!elementsName) {
            sprintf (buffer, "%02u", (unsigned int) i);
            elementName = buffer;
        } else {
            elementName = (char *) elementsName[i];
        }

        dbg ("[%s] = <%s>", elementName, elements[i]->str);
    }
}

size_t
Redis_anyElementIsNull (
    redisReply **elements,
    size_t nbElements
) {
    for (size_t i = 0; i < nbElements; i++) {
        if (elements[i]->type == REDIS_REPLY_NIL) {
            return i;
        }
    }

    return -1;
}

redisReply *
Redis_commandDbg (
    Redis *self,
    char * format,
    ...
) {
    char buffer [1024*1024];
    va_list args;

    va_start (args, format);
        vsnprintf (buffer, sizeof(buffer), format, args);
    va_end (args);

    // special ("%s", buffer);
    return redisCommand (self->context, buffer);
}

/*
bool
Redis_set (
    Redis *self,
    SocketSession *socketSession,
    ...
) {
    va_list args;

	va_start (args, socketSession);

	char * token = va_arg (args, char *);
	int tokenLen;
	int curPos;

    // Build the Redis command
	sprintf (self->commandBuffer, "HMSET zone%x:map%x:acc%llx ",
        socketSession->zoneId, socketSession->mapId, socketSession->accountId); // Identifiers

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

	redisReply *reply = Redis_commandDbg (self, self->commandBuffer);

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
*/

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
