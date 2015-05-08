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
    uint16_t serverId,
    uint16_t mapId,
    unsigned char *socketIdKey,
    bool authenticated
) {
    SocketSession *self;

    if ((self = calloc (1, sizeof (SocketSession))) == NULL) {
        return NULL;
    }

    if (!SocketSession_init (self, accountId, serverId, mapId, socketIdKey, authenticated)) {
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
    uint16_t serverId,
    uint16_t mapId,
    unsigned char *socketIdKey,
    bool authenticated
) {
    self->accountId = accountId;
    self->serverId = serverId;
    self->mapId = mapId;
    memcpy (self->key, socketIdKey, sizeof (self->key));

    self->authenticated = authenticated;

    return true;
}

void
SocketSession_genKey (
    unsigned char *sessionId,
    unsigned char sessionKey[SOCKET_SESSION_KEY_SIZE]
) {
    // Format the session key from the sessionId
    snprintf (sessionKey, SOCKET_SESSION_KEY_SIZE,
        "%02X%02X%02X%02X%02X", sessionId[0], sessionId[1], sessionId[2], sessionId[3], sessionId[4]);
}

void
SocketSession_genId (
    unsigned char *sessionKey,
    unsigned char sessionId[5]
) {
    uint64_t identity = strtoll (sessionKey, NULL, 16);
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
    dbg ("key = <%s>", self->key);
    dbg ("accountId = <%llx>", self->accountId);
    dbg ("serverId = <%x>", self->serverId);
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
