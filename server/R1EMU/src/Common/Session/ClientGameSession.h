/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file ClientGameSession.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Commander/Commander.h"
#include "Common/Session/SocketSession.h"

// ---------- Defines -------------

// ------ Structure declaration -------
/**
 * @brief ClientGameSession is a session created when a client authenticates
 *
 * ClientGameSession is created during the Barrack phase.
 * This structure size must be kept as small as possible.
 * /!\ It shouldn't contain any pointer, because it is sent to others worker via TCP
 */
struct ClientGameSession
{
    /** Copy of the socket Session */
    SocketSession socketSession;

    /** Family name (also called barrack name) */
    unsigned char familyName [64];

    /** current commander name */
    unsigned char currentCommanderName [64];

    /** Number of characters registered in the barrack */
    uint8_t charactersBarrackCount;

    /** Unique current commander ID */
    uint64_t currentCommanderId;

    /** PCID of the current character */
    uint32_t currentPcId;

    /** Current commander */
    CommanderInfo currentCommander;
};

typedef struct ClientGameSession ClientGameSession;

/**
 * @brief ClientGameSessionPrivileges enumerates the different levels of privileges
 *  for an account.
 */
typedef enum {
    CLIENT_SESSION_PRIVILEGES_ADMIN   = 0,
    CLIENT_SESSION_PRIVILEGES_UNKNOWN = 1,
    CLIENT_SESSION_PRIVILEGES_GM      = 2,
    CLIENT_SESSION_PRIVILEGES_PLAYER  = 3
} ClientGameSessionPrivileges;

// ----------- Functions ------------

/**
 * @brief Allocate a new ClientGameSession structure.
 * @return A pointer to an allocated ClientGameSession.
 */
ClientGameSession *
ClientGameSession_new (
);


/**
 * @brief Initialize an allocated ClientGameSession structure.
 * @param self An allocated ClientGameSession to initialize.
 * @return true on success, false otherwise.
 */
bool
ClientGameSession_init (
    ClientGameSession *self
);

/**
 * @brief Look up a session in the session hashtable
 * @param sessions The session hashtable
 * @param sessionKey The session key to lookup
 * @return A client session or NULL if it doesn't exists
 */
ClientGameSession *
ClientGameSession_lookupSession (
    zhash_t *sessions,
    unsigned char *sessionKey
);

/**
 * @brief Request a session from the session server
 * @param sessionServer An opened socket to the session server
 * @param clientIdentity A frame containing the identity of the client
 * @return a zframe_t containing a ClientGameSession on success, NULL otherwise
 */
zframe_t *
ClientGameSession_getSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity
);


/**
 * @brief Update a session for the session server
 * @param sessionServer An opened socket to the session server
 * @param clientIdentity A frame containing the identity of the client
 * @param session An allocated session to update
 * @return true on success, false otherwise
 */
bool
ClientGameSession_updateSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity,
    ClientGameSession *session
);


/**
 * @brief Delete a session from the session server
 * @param sessionServer An opened socket to the session server
 * @param clientIdentity A frame containing the identity of the client
 * @return true on success, false otherwise
 */
bool
ClientGameSession_deleteSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity
);

/**
 * @brief Format a session key from the session id
 * @param sessionId The sessionId of the session requested
 * @param[out] sessionKey The sessionKey generated
 * @param sessionKeySize The sessionKey size
 * @return
 */
void
ClientGameSession_genSessionKey (
    unsigned char *sessionId,
    unsigned char *sessionKey,
    size_t sessionKeySize
);

/**
 * @brief Prints a ClientGameSession structure.
 * @param self An allocated ClientGameSession
 */
void
ClientGameSession_print (
    ClientGameSession *self
);

/**
 * @brief Free an allocated ClientGameSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ClientGameSession.
 */
void
ClientGameSession_destroy (
    ClientGameSession **self
);


/**
 * @brief Free an allocated ClientGameSession structure
 * @param self A pointer to an allocated ClientGameSession.
 */
void
ClientGameSession_free (
    ClientGameSession *self
);
