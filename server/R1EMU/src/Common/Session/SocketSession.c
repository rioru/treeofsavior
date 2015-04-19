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

    // Consider it just connected, so it is in the Barrack
    self->isInBarrack = true;

    return true;
}

void
SocketSession_genKey (
    unsigned char *sessionId,
    unsigned char *sessionKey,
    size_t sessionKeySize
) {
    // Format the session key from the sessionId
    snprintf (sessionKey, sessionKeySize,
        "%02X%02X%02X%02X%02X", sessionId[0], sessionId[1], sessionId[2], sessionId[3], sessionId[4]);
}

void
SocketSession_print (
    SocketSession *self
) {
    dbg ("==== SocketSession %p ====", self);
    dbg ("key = <%s>", self->key);
    dbg ("accountId = <%llx>", self->accountId);
    dbg ("zoneId = <%x>", self->zoneId);
    dbg ("authenticated = <%x>", self->authenticated);
    dbg ("isInBarrack = <%x>", self->isInBarrack);
}


void
SocketSession_destroy (
    SocketSession **_self
) {
    SocketSession *self = *_self;

    free (self);
    *_self = NULL;
}
