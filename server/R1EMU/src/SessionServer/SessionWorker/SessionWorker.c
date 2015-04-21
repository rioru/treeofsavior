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
#include "BarrackServer/BarrackServer.h"
#include "Common/Session/GameSession.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Redis/Fields/RedisSocketSession.h"


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

static zframe_t *
SessionWorker_deleteSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame
);


// ------ Extern function implementation -------

SessionWorker *
SessionWorker_new (
    int workerId,
    int serverId,
    char *confFilePath
) {
    SessionWorker *self;

    if ((self = calloc (1, sizeof (SessionWorker))) == NULL) {
        return NULL;
    }

    if (!SessionWorker_init (self, workerId, serverId, confFilePath)) {
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
    int serverId,
    char *confFilePath
) {
    zconfig_t *conf;

    self->workerId = workerId;
    self->serverId = serverId;

    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the global configuration file (%s).", confFilePath);
        return false;
    }

    // Close the configuration file
    zconfig_destroy (&conf);

    dbg ("Session worker ID %d successfully connected to the database.", workerId);

    return true;
}


static zframe_t *
SessionWorker_handlePingPacket (
    void
) {
    return zframe_new (PACKET_HEADER (SESSION_SERVER_PONG), sizeof (SESSION_SERVER_PONG));
}

static zframe_t *
SessionWorker_updateSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame,
    zframe_t *sessionFrame
) {
    unsigned char *socketId;
    GameSession *newSession;

    // Retrieve the session from the session frame
    newSession = (GameSession *) zframe_data (sessionFrame);

    // Request to refresh the entire socket session to the Redis Server
    socketId = zframe_data (sessionIdFrame);
    SocketSession_genKey (socketId, newSession->socketSession.key, sizeof (newSession->socketSession.key));
    Redis_updateSocketSession (self->redis, &newSession->socketSession);

    // Request to refresh the entire game session to the Redis Server
    // TODO: Optimization : send only the delta(oldSession, newSession)
    Redis_updateGameSession (self->redis, newSession);

    dbg ("Your session has been updated, PLAYER_%llx !", newSession->socketSession.accountId);

    return zframe_new (PACKET_HEADER (SESSION_SERVER_UPDATE_SESSION_OK), sizeof (SESSION_SERVER_UPDATE_SESSION_OK));
}

static zframe_t *
SessionWorker_getSessionWithSocket (
    SessionWorker *self,
    zframe_t *socketIdFrame
) {
    GameSession gameSession = {0};
    SocketSession socketSession;
    unsigned char *socketId;
    unsigned char socketKey[11];

    // Generate the socketId key
    socketId = zframe_data (socketIdFrame);
    SocketSession_genKey (socketId, socketKey, sizeof (socketKey));

    // Search for the Socket Session
    if (!Redis_getSocketSession (self->redis, self->serverId, socketKey, &socketSession)) {
        error ("Cannot get Socket Session.");
        return false;
    }

    if (!socketSession.authenticated) {
        // This is the first time the client connect.
        // Initialize an empty game session
        GameSession_init (&gameSession);
        dbg ("Welcome, SOCKET_%s ! A new session has been initialized for you.", socketKey);
    } else {
        if (!Redis_getGameSession (self->redis, &socketSession, &gameSession)) {
            error ("Cannot get Game Session.");
            return NULL;
        }
        dbg ("Welcome back, SOCKET_%s !", socketKey);
    }

    // Copy the socketSession in the gameSession
    memcpy (&gameSession.socketSession, &socketSession, sizeof (SocketSession));

    // Reply with the socketSession
    return zframe_new (&gameSession, sizeof (GameSession));
}

static zframe_t *
SessionWorker_getSessionFromBarrack (
    SessionWorker *self,
    zframe_t *socketIdFrame,
    zframe_t *accountIdFrame
) {
    unsigned char *socketId;
    GameSession gameSession = {0};

    uint64_t accountId = *((uint64_t *) zframe_data (accountIdFrame));

    SocketSession socketSession = {
        .accountId = accountId,
        .zoneId = BARRACK_SERVER_ZONE_ID,
        .mapId = SOCKET_SESSION_UNDEFINED_MAP
    };

    // Generate the socket key
    socketId = zframe_data (socketIdFrame);
    SocketSession_genKey (socketId, socketSession.key, sizeof (socketSession.key));

    if (!Redis_getGameSession (self->redis, &socketSession, &gameSession)) {
        error ("Cannot get Game Session.");
        return NULL;
    }

    // Update the socketSession for the target zone Id
    socketSession.zoneId = self->serverId;
    if (!Redis_updateSocketSession (self->redis, &socketSession)) {
        error ("Cannot update Socket Session.");
        return NULL;
    }

    // Copy the socketSession in the gameSession
    memcpy (&gameSession.socketSession, &socketSession, sizeof (SocketSession));

    // Reply with the socketSession
    return zframe_new (&gameSession, sizeof (GameSession));
}

static zframe_t *
SessionWorker_deleteSession (
    SessionWorker *self,
    zframe_t *socketIdFrame
) {
    unsigned char *socketId;
    unsigned char socketKey[11];

    // Generate the socket key
    socketId = zframe_data (socketIdFrame);
    SocketSession_genKey (socketId, socketKey, sizeof (socketKey));

    // Delete the session
    // TODO

    return zframe_new (PACKET_HEADER (SESSION_SERVER_DELETE_SESSION_OK), sizeof (SESSION_SERVER_DELETE_SESSION_OK));
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
    zframe_t *requestAnswer = NULL;

    SessionServerRecvHeader header = *((SessionServerRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case SESSION_SERVER_PING:
            requestAnswer = SessionWorker_handlePingPacket ();
        break;

        case SESSION_SERVER_REQUEST_SESSION: {
            zframe_t *clientIdentityFrame = zmsg_pop (msg);
            requestAnswer = SessionWorker_getSessionWithSocket (self, clientIdentityFrame);
            zframe_destroy (&clientIdentityFrame);
        } break;

        case SESSION_SERVER_REQUEST_BARRACK_SESSION: {
            zframe_t *clientIdentityFrame = zmsg_pop (msg);
            zframe_t *accountIdFrame = zmsg_pop (msg);
            requestAnswer = SessionWorker_getSessionFromBarrack (self, clientIdentityFrame, accountIdFrame);
            zframe_destroy (&clientIdentityFrame);
            zframe_destroy (&accountIdFrame);
        } break;

        case SESSION_SERVER_UPDATE_SESSION: {
            zframe_t *clientIdentityFrame = zmsg_pop (msg);
            zframe_t *sessionFrame = zmsg_pop (msg);
            requestAnswer = SessionWorker_updateSession (self, clientIdentityFrame, sessionFrame);
            zframe_destroy (&clientIdentityFrame);
            zframe_destroy (&sessionFrame);
        } break;

        case SESSION_SERVER_DELETE_SESSION: {
            zframe_t *clientIdentityFrame = zmsg_pop (msg);
            requestAnswer = SessionWorker_deleteSession (self, clientIdentityFrame);
            zframe_destroy (&clientIdentityFrame);
        } break;

        default:
            error ("Packet type %d not handled.", header);
        break;
    }

    // Rebuild the answer message
    zmsg_add (msg, identityFrame);
    zmsg_add (msg, emptyFrame);

    if (!requestAnswer) {
        error ("Cannot answer to the request.");
        zmsg_add (msg, zframe_new (NULL, 0));
    } else {
        zmsg_add (msg, requestAnswer);
    }

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
    ||  zsock_connect (self->worker, SESSION_SERVER_BACKEND_ENDPOINT, self->serverId) == -1
    ) {
        error ("Session worker ID %d cannot connect to the backend socket.", self->workerId);
        return NULL;
    }
    dbg ("Session worker ID %d connected to %s", self->workerId, zsys_sprintf (SESSION_SERVER_BACKEND_ENDPOINT, self->serverId));

    // Tell to the broker we're ready for work
    if (!(readyFrame = zframe_new (PACKET_HEADER (SESSION_SERVER_WORKER_READY), sizeof (SESSION_SERVER_WORKER_READY)))
    ||  zframe_send (&readyFrame, self->worker, 0) == -1
    ) {
        error ("Session worker ID %d cannot send a correct SESSION_WORKER_READY state.", self->workerId);
        return NULL;
    }

    dbg ("Session worker ID %d is running and waiting for messages.", self->workerId);

    while (true) {

        // Process messages as they arrive
        if (!(msg = zmsg_recv (self->worker))) {
            dbg ("Session worker ID %d stops working.", self->workerId);
            break; // Interrupted
        }

        // No message should be with less than 3 frames
        if (zmsg_size (msg) < 3) {
            error ("Session worker ID %d received a malformed message.", self->workerId);
            zmsg_destroy (&msg);
            continue;
        }

        // Handle the request
        SessionWorker_handleRequest (self, msg);

        // Reply back to the sender
        if (zmsg_send (&msg, self->worker) != 0) {
            warning ("Session worker ID %d failed to send a message to the backend.", self->workerId);
        }
    }

    // Cleanup
    zsock_destroy (&self->worker);

    dbg ("Session worker ID %d exits.", self->workerId);
    return NULL;
}

void
SessionWorker_destroy (
    SessionWorker **_self
) {
    SessionWorker *self = *_self;

    mysql_close(self->sqlConn->handle);
    free (self->sqlConn);
    free (self);
    *_self = NULL;
}

void
SessionWorker_flushSession (
    SessionWorker *self,
    GameSession *session
) {
    MYSQL_ROW count;

    // Flush the commander
    if (session->currentCommanderId)
    {
        if (MySQL_query(self->sqlConn, "SELECT count(*) FROM commander WHERE commander_id = %u", session->currentCommanderId))
            error ("SQL Error : %s" , mysql_error(self->sqlConn->handle));
        else
        {
            count = mysql_fetch_row(self->sqlConn->result);
            if (atoi(count[0]) == 0)
            {
                // Insert a new Commander
                if (MySQL_query(self->sqlConn, "INSERT INTO commander (`commander_id`, `account_id`, `commander_num`, `name`, `family_name`, `class`, `gender`, `level`, `head_top`, `head_middle`, `necklace`, `body_armor`, `leg_armor`, `gloves`, `shoes`, `weapon`, `shield`, `costume`, `ring`, `bracelet_left`, `bracelet_right`, `hair_type`, `last_map`, `last_x`, `last_y`, `current_xp`, `pose`, `current_hp`, `current_sp`) VALUES (%u, %lu, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d)",
                    session->currentCommander.commanderId,
                    session->currentCommander.accountId,
                    session->currentCommander.charPosition,
                    session->currentCommander.charName,
                    session->currentCommander.familyName,
                    session->currentCommander.classId,
                    session->currentCommander.gender,
                    session->currentCommander.level,
                    session->currentCommander.head_top,
                    session->currentCommander.head_middle,
                    session->currentCommander.necklace,
                    session->currentCommander.body_armor,
                    session->currentCommander.leg_armor,
                    session->currentCommander.gloves,
                    session->currentCommander.boots,
                    session->currentCommander.weapon,
                    session->currentCommander.shield,
                    session->currentCommander.costume,
                    session->currentCommander.bracelet,
                    session->currentCommander.ring_left,
                    session->currentCommander.ring_right,
                    session->currentCommander.hairType,
                    session->currentCommander.mapId,
                    0,
                    0,
                    session->currentCommander.currentXP,
                    session->currentCommander.pose,
                    session->currentCommander.currentHP,
                    session->currentCommander.currentSP
                    ))
                {
                    error ("SQL Error : %s" , mysql_error(self->sqlConn->handle));
                }
                dbg("Inserting a new commander");
            }
            else
            {
                // Update the commander
                dbg("Updating the commander");
            }
        }
    }
}
