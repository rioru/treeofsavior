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
#include "SocketSession.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

SocketSession *
SocketSession_new (
    uint64_t accountId,
    uint16_t zoneId,
    uint16_t mapId,
    unsigned char *socketIdKey
) {
    SocketSession *self;

    if ((self = calloc (1, sizeof (SocketSession))) == NULL) {
        return NULL;
    }

    if (!SocketSession_init (self, accountId, zoneId, mapId, socketIdKey)) {
        SocketSession_destroy (&self);
        error ("SocketSession failed to initialize.");
        return NULL;
    }

    return self;
}


bool
SocketSession_init (
    SocketSession *self,
    uint64_t accountId,
    uint16_t zoneId,
    uint16_t mapId,
    unsigned char *socketIdKey
) {
    self->accountId = accountId;
    self->zoneId = zoneId;
    self->mapId = mapId;
    memcpy (self->key, socketIdKey, sizeof (self->key));

    self->authenticated = false;

    return true;
}


void
SocketSession_destroy (
    SocketSession **_self
) {
    SocketSession *self = *_self;

    free (self);
    *_self = NULL;
}
