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

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Commander/Commander.h"
#include "Common/Session/SocketSession.h"

// ---------- Defines -------------
/** Max size of the Account Login */
#define GAME_SESSION_ACCOUNT_LOGIN_MAXSIZE 17

// ------ Structure declaration -------
/**
 * @brief GameSession is a session created when a client authenticates
 *
 * GameSession is created during the Barrack phase.
 * This structure size must be kept as small as possible.
 * /!\ It shouldn't contain any pointer, because it is sent to others worker via TCP
 */
struct GameSession
{
    /** Socket ID */
    uint8_t socketId[SOCKET_SESSION_ID_SIZE];

    /** Number of characters registered in the barrack */
    uint8_t charactersBarrackCount;

    /** The account login */
    char accountLogin [GAME_SESSION_ACCOUNT_LOGIN_MAXSIZE];

    /** Current commander */
    CommanderInfo currentCommander;
};

typedef struct GameSession GameSession;

/**
 * @brief GameSessionPrivileges enumerates the different levels of privileges
 *  for an account.
 */
typedef enum {
    CLIENT_SESSION_PRIVILEGES_ADMIN   = 0,
    CLIENT_SESSION_PRIVILEGES_UNKNOWN = 1,
    CLIENT_SESSION_PRIVILEGES_GM      = 2,
    CLIENT_SESSION_PRIVILEGES_PLAYER  = 3
} GameSessionPrivileges;

// ----------- Functions ------------

/**
 * @brief Allocate a new GameSession structure.
 * @return A pointer to an allocated GameSession.
 */
GameSession *
GameSession_new (
);


/**
 * @brief Initialize an allocated GameSession structure.
 * @param self An allocated GameSession to initialize.
 * @return true on success, false otherwise.
 */
bool
GameSession_init (
    GameSession *self
);

/**
 * @brief Look up a session in the session hashtable
 * @param sessions The session hashtable
 * @param sessionKey The session key to lookup
 * @return A client session or NULL if it doesn't exists
 */
GameSession *
GameSession_lookupSession (
    zhash_t *sessions,
    unsigned char *sessionKey
);


/**
 * @brief Request a game session from the session of the barrack server
 * @param sessionServer An opened socket to the session server
 * @param clientIdentity A frame containing the identity of the client
 * @param accountIdFrame A frame containing the accountId of the client.
 * @return a zframe_t containing a GameSession on success, NULL otherwise
 */
zframe_t *
GameSession_getBarrackSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity,
    zframe_t *accountIdFrame
);

/**
 * @brief Update a session for the session server
 * @param sessionServer An opened socket to the session server
 * @param clientIdentity A frame containing the identity of the client
 * @param session An allocated session to update
 * @return true on success, false otherwise
 */
bool
GameSession_updateSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity,
    GameSession *session
);


/**
 * @brief Delete a session from the session server
 * @param sessionServer An opened socket to the session server
 * @param clientIdentity A frame containing the identity of the client
 * @return true on success, false otherwise
 */
bool
GameSession_deleteSession (
    zsock_t *sessionServer,
    zframe_t *clientIdentity
);

/**
 * @brief Prints a GameSession structure.
 * @param self An allocated GameSession
 */
void
GameSession_print (
    GameSession *self
);

/**
 * @brief Free an allocated GameSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated GameSession.
 */
void
GameSession_destroy (
    GameSession **self
);


/**
 * @brief Free an allocated GameSession structure
 * @param self A pointer to an allocated GameSession.
 */
void
GameSession_free (
    GameSession *self
);
