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
