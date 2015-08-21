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
    uint16_t routerId,
    uint16_t mapId,
    uint8_t *socketId,
    bool authenticated
) {
    SocketSession *self;

    if ((self = calloc (1, sizeof (SocketSession))) == NULL) {
        return NULL;
    }

    if (!SocketSession_init (self, accountId, routerId, mapId, socketId, authenticated)) {
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
    uint16_t routerId,
    uint16_t mapId,
    uint8_t *sessionKey,
    bool authenticated
) {
    self->accountId = accountId;
    self->routerId = routerId;
    self->mapId = mapId;
    memcpy (self->sessionKey, sessionKey, sizeof (self->sessionKey));

    self->authenticated = authenticated;

    return true;
}

void
SocketSession_genSessionKey (
    uint8_t *sessionId,
    uint8_t sessionKey[SOCKET_SESSION_ID_SIZE]
) {
    // Format the session key from the sessionId
    snprintf (sessionKey, SOCKET_SESSION_ID_SIZE,
        "%02X%02X%02X%02X%02X", sessionId[0], sessionId[1], sessionId[2], sessionId[3], sessionId[4]);
}

void
SocketSession_genId (
    uint8_t *sessionKey,
    uint8_t sessionId[5]
) {
    uint64_t identity = strtoull (sessionKey, NULL, 16);
    // Format the sessionId from the session key
    // Swap the bytes
    sessionId[0] = (identity >> 32) & 0xFF;
    sessionId[1] = (identity >> 24) & 0xFF;
    sessionId[2] = (identity >> 16) & 0xFF;
    sessionId[3] = (identity >>  8) & 0xFF;
    sessionId[4] =  identity        & 0xFF;
}

void
SocketSession_print (
    SocketSession *self
) {
    dbg ("==== SocketSession %p ====", self);
    dbg ("sessionKey = <%s>", self->sessionKey);
    dbg ("accountId = <%llx>", self->accountId);
    dbg ("routerId = <%x>", self->routerId);
    dbg ("authenticated = <%x>", self->authenticated);
}


void
SocketSession_destroy (
    SocketSession **_self
) {
    SocketSession *self = *_self;

    free (self);
    *_self = NULL;
}
