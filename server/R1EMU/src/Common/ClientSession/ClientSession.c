/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ClientSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "ClientSession.h"
#include "SessionServer/SessionServer.h"


ClientSession *
ClientSession_new (
    void
) {
    ClientSession *self;

    if ((self = calloc (1, sizeof (ClientSession))) == NULL) {
        return NULL;
    }

    if (!ClientSession_init (self)) {
        ClientSession_destroy (&self);
        error ("ClientSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
ClientSession_init (
    ClientSession *self
) {
    return true;
}


ClientSession *
ClientSession_lookupSession (
    zhash_t *sessions,
    unsigned char *sessionKey
) {
    return zhash_lookup (sessions, sessionKey);
}

void
ClientSession_getSessionKey (
    unsigned char *sessionId,
    unsigned char *sessionKey,
    size_t sessionKeySize
) {
    // Format the session key from the sessionId
    snprintf (sessionKey, sessionKeySize,
        "%02X%02X%02X%02X%02X", sessionId[0], sessionId[1], sessionId[2], sessionId[3], sessionId[4]);
}

bool
ClientSession_deleteSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity
) {
    zframe_t *answerFrame;
    SessionServerSendHeader answer;
    zmsg_t *msg;

    // Build a session request message
    if (!(msg = zmsg_new ())
    ||  zmsg_addmem (msg, PACKET_HEADER (SESSION_SERVER_DELETE_SESSION), sizeof (SESSION_SERVER_DELETE_SESSION)) != 0
    ||  zmsg_addmem (msg, zframe_data (clientIdentity), zframe_size (clientIdentity)) != 0
    ||  zmsg_send (&msg, sessionServer) != 0
    ) {
        error ("Cannot build and send a session message for the session server");
        return false;
    }

    // Wait for the session server answer
    if (!(msg = zmsg_recv (sessionServer))) {
        error ("Cannot receive a session from the session server");
        return false;
    }

    // Extract the answer of the session server
    if (!(answerFrame = zmsg_pop (msg))
    ||  !(answer = *((SessionServerSendHeader *) zframe_data (answerFrame)))
    ||  !(sizeof (answer) == zframe_size (answerFrame))
    ) {
        if (answerFrame) {
            zframe_destroy (&answerFrame);
        }
        error ("Cannot extract correctly the answer from the session server");
        return false;
    }

    // Verify the status code
    if (answer != SESSION_SERVER_DELETE_SESSION_OK) {
        error ("The session server failed to delete the session");
        return false;
    }

    // Cleanup
    zframe_destroy (&answerFrame);
    zmsg_destroy (&msg);

    return true;

}

zframe_t *
ClientSession_getSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity
) {
    ClientSession *session;
    zframe_t *sessionFrame;
    zmsg_t *msg;

    // Build a session request message
    if (!(msg = zmsg_new ())
    ||  zmsg_addmem (msg, PACKET_HEADER (SESSION_SERVER_REQUEST_SESSION), sizeof (SESSION_SERVER_REQUEST_SESSION)) != 0
    ||  zmsg_addmem (msg, zframe_data (clientIdentity), zframe_size (clientIdentity)) != 0
    ||  zmsg_send (&msg, sessionServer) != 0
    ) {
        error ("Cannot build and send a session message for the session server");
        return NULL;
    }

    // Wait for the session server answer
    if (!(msg = zmsg_recv (sessionServer))) {
        error ("Cannot receive a session from the session server");
        return NULL;
    }

    // Extract the session from the answer
    if (!(sessionFrame = zmsg_pop (msg))
    ||  !(session = (ClientSession *) zframe_data (sessionFrame))
    ||  !(sizeof (ClientSession) == zframe_size (sessionFrame))
    ) {
        error ("Cannot extract correctly the session from the session server");
        return NULL;
    }

    // Cleanup
    zmsg_destroy (&msg);

    return sessionFrame;
}

bool
ClientSession_updateSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity,
    ClientSession *session
) {
    zframe_t *answerFrame;
    SessionServerSendHeader answer;
    zmsg_t *msg;

    // Build the update sesion packet, and send it to the server
    if (!(msg = zmsg_new ())
    ||  zmsg_addmem (msg, PACKET_HEADER (SESSION_SERVER_UPDATE_SESSION), sizeof (SESSION_SERVER_UPDATE_SESSION)) != 0
    ||  zmsg_addmem (msg, zframe_data (clientIdentity), zframe_size (clientIdentity)) != 0
    ||  zmsg_addmem (msg, session, sizeof (ClientSession)) != 0
    ||  zmsg_send (&msg, sessionServer) != 0
    ) {
        error ("Cannot build and send a session message for the session server");
        return false;
    }

    // Wait for the answer of the session server
    if (!(msg = zmsg_recv (sessionServer))) {
        error ("Cannot receive a session from the session server");
        return false;
    }

    // Extract the answer of the session server
    if (!(answerFrame = zmsg_pop (msg))
    ||  !(answer = *((SessionServerSendHeader *) zframe_data (answerFrame)))
    ||  !(sizeof (answer) == zframe_size (answerFrame))
    ) {
        if (answerFrame) {
            zframe_destroy (&answerFrame);
        }
        error ("Cannot extract correctly the answer from the session server");
        return false;
    }

    // Verify the status code
    if (answer != SESSION_SERVER_UPDATE_SESSION_OK) {
        error ("The session server failed to update the session");
        return false;
    }

    // Cleanup
    zframe_destroy (&answerFrame);
    zmsg_destroy (&msg);

    return true;
}

void
ClientSession_print (
    ClientSession *self
) {
    dbg ("==== Session %p ====", self);
    dbg ("familyName = <%s>", self->familyName);
    dbg ("currentCommanderName = <%s>", self->currentCommanderName);
    dbg ("charactersBarrackCount = %u", self->charactersBarrackCount);
    dbg ("accountId = 0x%llX", self->accountId);
    dbg ("currentCommanderId = 0x%llX", self->currentCommanderId);
    dbg ("currentPcId = 0x%X", self->currentPcId);
}

void
ClientSession_destroy (
    ClientSession **_self
) {
    ClientSession_free (*_self);
    *_self = NULL;
}

void
ClientSession_free (
    ClientSession *self
) {
    free (self);
}
