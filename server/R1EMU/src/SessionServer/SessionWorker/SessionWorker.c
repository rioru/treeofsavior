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
#include "Common/Session/ClientSession.h"


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
    SessionTable *sessionsTable,
    char *confFilePath
) {
    SessionWorker *self;

    if ((self = calloc (1, sizeof (SessionWorker))) == NULL) {
        return NULL;
    }

    if (!SessionWorker_init (self, workerId, serverId, sessionsTable, confFilePath)) {
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
    SessionTable *sessionsTable,
    char *confFilePath
) {
    zconfig_t *conf;
    const char *sqlHostname;
    const char *sqlLogin;
    const char *sqlPassword;
    const char *sqlDatabase;

    self->workerId = workerId;
    self->sessionsTable = sessionsTable;
    self->serverId  = serverId;

    if (!(conf = zconfig_load (confFilePath))) {
        error ("Cannot read the global configuration file (%s).", confFilePath);
        return false;
    }

    // ===================================
    //        Initialize MySQL
    // ===================================
    if (!(sqlHostname = zconfig_resolve (conf, "database/mysql_host", NULL))
    ) {
        warning ("Cannot read correctly the MySQL host in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", SESSION_WORKER_SQL_HOSTNAME_DEFAULT);
        sqlHostname = SESSION_WORKER_SQL_HOSTNAME_DEFAULT;
    }
    if (!(sqlLogin = zconfig_resolve (conf, "database/mysql_user", NULL))
    ) {
        warning ("Cannot read correctly the MySQL user in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", SESSION_WORKER_SQL_LOGIN_DEFAULT);
        sqlLogin = SESSION_WORKER_SQL_LOGIN_DEFAULT;
    }
    if (!(sqlPassword = zconfig_resolve (conf, "database/mysql_password", NULL))
    ) {
        warning ("Cannot read correctly the MySQL password in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", SESSION_WORKER_SQL_PASSWORD_DEFAULT);
        sqlPassword = SESSION_WORKER_SQL_PASSWORD_DEFAULT;
    }
    if (!(sqlDatabase = zconfig_resolve (conf, "database/mysql_database", NULL))
    ) {
        warning ("Cannot read correctly the MySQL database in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", SESSION_WORKER_SQL_DATABASE_DEFAULT);
        sqlDatabase = SESSION_WORKER_SQL_DATABASE_DEFAULT;
    }

    self->sqlConn = malloc (sizeof (SQL));
    self->sqlConn->result = NULL;
    self->sqlConn->handle = mysql_init (NULL);

    if (!mysql_real_connect (self->sqlConn->handle, sqlHostname, sqlLogin, sqlPassword, sqlDatabase, 0, NULL, 0)) {
        error ("The session worker ID %d could not connect to the database at %s. (mysql_errno = %d)",
            workerId, sqlHostname, mysql_errno (self->sqlConn->handle));
        return false;
    }

    // ===================================
    //   Initialize Redis connection
    // ===================================
    char *redisHostname;
    char *redisPort;
    if (!(redisHostname = zconfig_resolve (conf, "redisServer/redis_host", NULL))
    ) {
        warning ("Cannot read correctly the Redis host in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", SESSION_WORKER_REDIS_HOSTNAME_DEFAULT);
        redisHostname = SESSION_WORKER_REDIS_HOSTNAME_DEFAULT;
    }
    if (!(redisPort = zconfig_resolve (conf, "redisServer/redis_port", NULL))
    ) {
        warning ("Cannot read correctly the Redis port in the configuration file (%s). ", confFilePath);
        warning ("The default hostname = %s has been used.", SESSION_WORKER_REDIS_PORT_DEFAULT);
        redisPort = SESSION_WORKER_REDIS_PORT_DEFAULT;
    }

    if (!(self->redis = Redis_new (redisHostname, atoi (redisPort)))) {
        error ("Cannot initialize a new Redis connection.");
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
    ClientSession *newSession;

    // Retrieve the session from the session frame
    newSession = (ClientSession *) zframe_data (sessionFrame);

    // Request to refresh the entire session to the Redis Server
    // TODO: Optimization : send only the delta(oldSession, newSession)
    Redis_refreshSession (self->redis, newSession);

    dbg ("Your session has been updated, USER_%" PRIu64 "!", newSession->accountId);
    ClientSession_print (newSession);

    return zframe_new (PACKET_HEADER (SESSION_SERVER_UPDATE_SESSION_OK), sizeof (SESSION_SERVER_UPDATE_SESSION_OK));
}

static zframe_t *
SessionWorker_deleteSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame
) {
    unsigned char *sessionId;
    unsigned char sessionKey[11];

    // Get hashtable key
    sessionId = zframe_data (sessionIdFrame);
    ClientSession_getSessionKey (sessionId, sessionKey, sizeof (sessionKey));

    // Delete
    SessionTable_delete (self->sessionsTable, sessionKey);

    return zframe_new (PACKET_HEADER (SESSION_SERVER_DELETE_SESSION_OK), sizeof (SESSION_SERVER_DELETE_SESSION_OK));
}

static zframe_t *
SessionWorker_getSession (
    SessionWorker *self,
    zframe_t *sessionIdFrame
) {
    ClientSession *session;
    unsigned char *sessionId;
    unsigned char sessionKey[11];

    // Get hashtable key
    sessionId = zframe_data (sessionIdFrame);
    ClientSession_getSessionKey (sessionId, sessionKey, sizeof (sessionKey));

    // Search for it in the hashtable
    if (!(session = SessionTable_lookup (self->sessionsTable, sessionKey))) {
        // Create it if it doesn't exists
        dbg ("Welcome USER_%s ! A new session has been created for you.", sessionKey);
        session = SessionTable_create (self->sessionsTable, sessionKey);
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
    zframe_t *requestAnswer = NULL;

    SessionServerRecvHeader header = *((SessionServerRecvHeader *) zframe_data (headerFrame));

    // Handle the request
    switch (header) {
        case SESSION_SERVER_PING:
            requestAnswer = SessionWorker_handlePingPacket ();
        break;

        case SESSION_SERVER_REQUEST_SESSION: {
            zframe_t *clientIdentityFrame = zmsg_pop (msg);
            requestAnswer = SessionWorker_getSession (self, clientIdentityFrame);
            zframe_destroy (&clientIdentityFrame);
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
    ClientSession *session
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
