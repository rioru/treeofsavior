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
#include "SessionServer/ClientSession/ClientSession.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern declaration -------

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


void *
SessionWorker_worker (
    void *arg
) {
    zsock_t *worker;
    zframe_t *readyFrame;
    zmsg_t *sessionMsg;

    SessionWorker * self = (SessionWorker *) arg;

    // Create and connect a socket to the backend
    if (!(worker = zsock_new (ZMQ_REQ))
    ||  zsock_connect (worker, SESSION_SERVER_BACKEND_ENDPOINT) == -1
    ) {
        error ("Session worker ID = %d cannot connect to the backend socket.", self->workerId);
        return NULL;
    }

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (SESSION_SERVER_WORKER_READY, 1))
    ||  zframe_send (&readyFrame, worker, 0) == -1
    ) {
        error ("Session worker ID = %d cannot send a correct SESSION_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("Session worker ID %d is running and waiting for messages.", self->workerId);

    while (true) {

        // Process messages as they arrive
        if (!(sessionMsg = zmsg_recv (worker))) {
            dbg ("Session worker ID = %d stops working.", self->workerId);
            break; // Interrupted
        }

        // Retrieve data from the message
        zframe_t *data = zmsg_last (sessionMsg);

        // Handle PING-PONG messages
        if (memcmp (zframe_data (data), SESSION_SERVER_PING, sizeof (SESSION_SERVER_PING)) == 0) {
            zframe_reset (data, SESSION_SERVER_PONG, sizeof (SESSION_SERVER_PONG));
        }
        else {
            // Build the reply packet

            // Format the session key
            unsigned char * sessionId = zframe_data (data);
            char sessionKey[11] = {0};
            snprintf (sessionKey, sizeof (sessionKey),
                "%02X%02X%02X%02X%02X", sessionId[0], sessionId[1], sessionId[2], sessionId[3], sessionId[4]);

            // Search for it in the hashtable
            ClientSession * session = zhash_lookup (self->sessions, sessionKey);

            // Create it if it doesn't exists
            if (!session) {
                dbg ("Welcome USER_%s ! A new session has been created for you.", sessionKey);
                session = ClientSession_new (sessionKey);
                zhash_insert (self->sessions, sessionKey, session);
            } else {
                dbg ("Welcome back USER_%s !", sessionKey);
            }

            // Reply the entire session
            zframe_reset (data, session, sizeof (ClientSession));
        }

        // Send the session data back to the backend
        if (zmsg_send (&sessionMsg, worker) != 0) {
            warning ("Session worker ID = %d failed to send a message to the backend.", self->workerId);
        }
    }

    zsock_destroy (&worker);

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
