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
 * @brief GameSessionPrivileges enumerates the different levels of privileges
 *  for an account.
 */
typedef enum GameSessionPrivileges {
    GAME_SESSION_PRIVILEGES_ADMIN   = 0,
    GAME_SESSION_PRIVILEGES_UNKNOWN = 1,
    GAME_SESSION_PRIVILEGES_GM      = 2,
    GAME_SESSION_PRIVILEGES_PLAYER  = 3
}   GameSessionPrivileges;

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

    /** Account privilege level */
    GameSessionPrivileges accountPrivilege;

    /** The account login */
    char accountLogin [GAME_SESSION_ACCOUNT_LOGIN_MAXSIZE];

    /** Current commander */
    CommanderInfo currentCommander;
};

typedef struct GameSession GameSession;

// ----------- Functions ------------

/**
 * @brief Allocate a new GameSession structure.
 * @param commanderInfo An initialized commander information
 * @return A pointer to an allocated GameSession.
 */
GameSession *
GameSession_new (
    CommanderInfo *commanderInfo
);


/**
 * @brief Initialize an allocated GameSession structure.
 * @param self An allocated GameSession to initialize.
 * @param commanderInfo An initialized commander information
 * @return true on success, false otherwise.
 */
bool
GameSession_init (
    GameSession *self,
    CommanderInfo *commanderInfo
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
