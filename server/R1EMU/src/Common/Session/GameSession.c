/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file GameSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "GameSession.h"
#include "SessionServer/SessionServer.h"


GameSession *
GameSession_new (
    void
) {
    GameSession *self;

    if ((self = calloc (1, sizeof (GameSession))) == NULL) {
        return NULL;
    }

    if (!GameSession_init (self)) {
        GameSession_destroy (&self);
        error ("GameSession failed to initialize.");
        return NULL;
    }

    return self;
}

bool
GameSession_init (
    GameSession *self
) {
    memset (self, 0, sizeof (GameSession));
    return true;
}

GameSession *
GameSession_lookupSession (
    zhash_t *sessions,
    unsigned char *sessionKey
) {
    return zhash_lookup (sessions, sessionKey);
}

bool
GameSession_deleteSession (
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
GameSession_getSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity
) {
    GameSession *session;
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
    ||  !(session = (GameSession *) zframe_data (sessionFrame))
    ||  !(sizeof (GameSession) == zframe_size (sessionFrame))
    ) {
        error ("Cannot extract correctly the session from the session server");
        return NULL;
    }

    // Cleanup
    zmsg_destroy (&msg);

    return sessionFrame;
}

zframe_t *
GameSession_getBarrackSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity,
    zframe_t *accountIdFrame
) {
    GameSession *session;
    zframe_t *sessionFrame;
    zmsg_t *msg;

    // Build a session request message
    if (!(msg = zmsg_new ())
    ||  zmsg_addmem (msg, PACKET_HEADER (SESSION_SERVER_REQUEST_BARRACK_SESSION), sizeof (SESSION_SERVER_REQUEST_BARRACK_SESSION)) != 0
    ||  zmsg_addmem (msg, zframe_data (clientIdentity), zframe_size (clientIdentity)) != 0
    ||  zmsg_addmem (msg, zframe_data (accountIdFrame), zframe_size (accountIdFrame)) != 0
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
    ||  !(session = (GameSession *) zframe_data (sessionFrame))
    ||  !(sizeof (GameSession) == zframe_size (sessionFrame))
    ) {
        error ("Cannot extract correctly the session from the session server");
        return NULL;
    }

    // Cleanup
    zmsg_destroy (&msg);

    return sessionFrame;
}

bool
GameSession_updateSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity,
    GameSession *session
) {
    zframe_t *answerFrame;
    SessionServerSendHeader answer;
    zmsg_t *msg;

    // Build the update sesion packet, and send it to the server
    if (!(msg = zmsg_new ())
    ||  zmsg_addmem (msg, PACKET_HEADER (SESSION_SERVER_UPDATE_SESSION), sizeof (SESSION_SERVER_UPDATE_SESSION)) != 0
    ||  zmsg_addmem (msg, zframe_data (clientIdentity), zframe_size (clientIdentity)) != 0
    ||  zmsg_addmem (msg, session, sizeof (GameSession)) != 0
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
GameSession_print (
    GameSession *self
) {
    dbg ("==== GameSession %p ====", self);
    dbg ("charactersBarrackCount = %u", self->charactersBarrackCount);
    dbg ("currentCommanderId = 0x%llX", self->currentCommanderId);
    dbg ("currentPcId = 0x%X", self->currentPcId);
    SocketSession_print (&self->socketSession);
}

void
GameSession_destroy (
    GameSession **_self
) {
    GameSession_free (*_self);
    *_self = NULL;
}

void
GameSession_free (
    GameSession *self
) {
    free (self);
}
