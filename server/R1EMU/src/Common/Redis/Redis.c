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

    return true;
}


void
Redis_destroy (
    Redis **_self
) {
    Redis *self = *_self;

    free (self);
    *_self = NULL;
}
