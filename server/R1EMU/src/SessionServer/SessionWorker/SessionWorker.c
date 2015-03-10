/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SessionWorker.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "SessionWorker.h"
#include "SessionServer/SessionServer.h"
#include "Common/ClientSession/ClientSession.h"


// ------ Structure declaration -------


// ------ Static declaration -------

/**
 * @brief Handle a PING request from any entity.
 * @return a zframe_t containing the PONG. Never returns NULL.
 */
static zframe_t *
SessionWorker_handlePingPacket (
    void
);

/**
 * @brief Handle a session request. Creates a new session if the sessionId doesn't exist.
 * @param self The SessionWorker
 * @param sessionIdFrame The identity of the entity
 * @return a zframe_t containing the session. Never returns NULL.
 */
static zframe_t *
SessionWorker_handleRequestSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame
);


/**
 * @brief Handle a session update.
 * @param self The SessionWorker
 * @param sessionIdFrame The identity of the entity
 * @param sessionFrame The session to update
 * @return a zframe_t containing the status code. Never returns NULL.
 */
static zframe_t *
SessionWorker_updateRequestSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame,
    zframe_t *sessionFrame
);


/**
 * @brief Get a Session from the session hashtable
 * @param self The SessionWorker
 * @param sessionId The sessionId of the session requested
 * @return An allocated ClientSession on success, false otherwise
 */
static ClientSession *
SessionWorker_getSession (
    SessionWorker *self,
    unsigned char *sessionId
);

/**
 * @brief Format a session key from the session id
 * @param sessionId The sessionId of the session requested
 * @param[out] sessionKey The sessionKey generated
 * @param sessionKeySize The sessionKey size
 * @return
 */
static void
SessionWorker_getSessionKey (
    unsigned char *sessionId,
    unsigned char *sessionKey,
    size_t sessionKeySize
);

// ------ Extern function implementation -------

SessionWorker *
SessionWorker_new (
    int workerId,
    zhash_t *sessions
) {
    SessionWorker *self;

    if ((self = calloc (1, sizeof (SessionWorker))) == NULL) {
        return NULL;
    }

    if (!SessionWorker_init (self, workerId, sessions)) {
        SessionWorker_destroy (&self);
        error ("SessionWorker failed to initialize.");
        return NULL;
    }

    return self;
}


bool
SessionWorker_init (
    SessionWorker *self,
    int workerId,
    zhash_t *sessions
) {
    self->workerId = workerId;
    self->sessions = sessions;

    return true;
}


static zframe_t *
SessionWorker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (SESSION_SERVER_PONG), sizeof (SESSION_SERVER_PONG));
}

static ClientSession *
SessionWorker_getSession (
    SessionWorker *self,
    unsigned char *sessionKey
) {
    return zhash_lookup (self->sessions, sessionKey);
}

static void
SessionWorker_getSessionKey (
    unsigned char *sessionId,
    unsigned char *sessionKey,
    size_t sessionKeySize
) {
    // Format the session key from the sessionId
    snprintf (sessionKey, sessionKeySize,
        "%02X%02X%02X%02X%02X", sessionId[0], sessionId[1], sessionId[2], sessionId[3], sessionId[4]);
}

static zframe_t *
SessionWorker_updateRequestSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame,
    zframe_t *sessionFrame
) {
    ClientSession *newSession, *oldSession;
    unsigned char *sessionId;
    unsigned char sessionKey[11];

    sessionId = zframe_data (sessionIdFrame);
    SessionWorker_getSessionKey (sessionId, sessionKey, sizeof (sessionKey));

    newSession = (ClientSession *) zframe_data (sessionFrame);

    // Search for it in the hashtable
    if (!(oldSession = SessionWorker_getSession (self, sessionKey))) {
        // It doesn't exist, throw an error
        error ("The session server cannot update a session that doesn't exist.");
        return zframe_new (PACKET_HEADER (SESSION_SERVER_UPDATE_SESSION_FAILED), sizeof (SESSION_SERVER_UPDATE_SESSION_FAILED));
    }

    // Session exists
    memcpy (oldSession, newSession, sizeof (ClientSession));
    dbg ("Your session has been updated, USER_%s !", sessionKey);

    ClientSession_print (newSession);

    return zframe_new (PACKET_HEADER (SESSION_SERVER_UPDATE_SESSION_OK), sizeof (SESSION_SERVER_UPDATE_SESSION_OK));
}

static zframe_t *
SessionWorker_handleRequestSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame
) {
    ClientSession *session;
    unsigned char *sessionId;
    unsigned char sessionKey[11];

    sessionId = zframe_data (sessionIdFrame);
    SessionWorker_getSessionKey (sessionId, sessionKey, sizeof (sessionKey));

    // Search for it in the hashtable
    if (!(session = SessionWorker_getSession (self, sessionKey))) {
        // Create it if it doesn't exists
        dbg ("Welcome USER_%s ! A new session has been created for you.", sessionKey);
        session = ClientSession_new (sessionKey);
        zhash_insert (self->sessions, sessionKey, session);
    } else {
        // Session already exist
        dbg ("Welcome back USER_%s !", sessionKey);
    }

    // Reply with the session
    return zframe_new (session, sizeof (ClientSession));
}


void
SessionWorker_handleRequest (
    SessionWorker *self,
    zmsg_t *msg
) {
    // Extract the request
    zframe_t *identityFrame = zmsg_pop (msg); (void) identityFrame;
    zframe_t *emptyFrame = zmsg_pop (msg); (void) emptyFrame;
    zframe_t *headerFrame = zmsg_pop (msg);
    zframe_t *requestAnswer;

    SessionServerRecvHeader header = *((SessionServerRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case SESSION_SERVER_PING:
            requestAnswer = SessionWorker_handlePingPacket ();
        break;

        case SESSION_SERVER_REQUEST_SESSION: {
            zframe_t *clientIdentityFrame = zmsg_pop (msg);
            requestAnswer = SessionWorker_handleRequestSession (self, clientIdentityFrame);
            zframe_destroy (&clientIdentityFrame);
        } break;

        case SESSION_SERVER_UPDATE_SESSION: {
            zframe_t *clientIdentityFrame = zmsg_pop (msg);
            zframe_t *sessionFrame = zmsg_pop (msg);
            requestAnswer = SessionWorker_updateRequestSession (self, clientIdentityFrame, sessionFrame);
            zframe_destroy (&clientIdentityFrame);
            zframe_destroy (&sessionFrame);
        } break;

        default:
            error ("Packet type %d not handled.", header);
        break;
    }

    // Rebuild the answer message
    zmsg_add (msg, identityFrame);
    zmsg_add (msg, emptyFrame);
    zmsg_add (msg, requestAnswer);

    // Clean up
    zframe_destroy (&headerFrame);
}


void *
SessionWorker_worker (
    void *arg
) {
    zframe_t *readyFrame;
    zmsg_t *msg;

    SessionWorker * self = (SessionWorker *) arg;

    // Create and connect a socket to the backend
    if (!(self->worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (self->worker, SESSION_SERVER_BACKEND_ENDPOINT) == -1
    ) {
        error ("Session worker ID = %d cannot connect to the backend socket.", self->workerId);
        return NULL;
    }

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (PACKET_HEADER (SESSION_SERVER_WORKER_READY), sizeof (SESSION_SERVER_WORKER_READY)))
    ||  zframe_send (&readyFrame, self->worker, 0) == -1
    ) {
        error ("Session worker ID = %d cannot send a correct SESSION_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("Session worker ID %d is running and waiting for messages.", self->workerId);

    while (true) {

        // Process messages as they arrive
        if (!(msg = zmsg_recv (self->worker))) {
            dbg ("Session worker ID = %d stops working.", self->workerId);
            break; // Interrupted
        }

        // No message should be with less than 3 frames
        if (zmsg_size (msg) < 3) {
            error ("A malformed message has been received.");
            zmsg_destroy (&msg);
            continue;
        }

        // Handle the request
        SessionWorker_handleRequest (self, msg);

        // Reply back to the sender
        if (zmsg_send (&msg, self->worker) != 0) {
            warning ("Session worker ID = %d failed to send a message to the backend.", self->workerId);
        }
    }

    // Cleanup
    zsock_destroy (&self->worker);

    dbg ("Session worker ID = %d exits.", self->workerId);
    return NULL;
}

void
SessionWorker_destroy (
    SessionWorker **_self
) {
    SessionWorker *self = *_self;

    free (self);
    *_self = NULL;
}
