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

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"

// ---------- Defines -------------

// ------ Structure declaration -------
/**
 * @brief ClientSession is a session created when a client authenticates
 *
 * ClientSession is created during the Barrack phase.
 * This structure size must be kept as small as possible.
 * It shouldn't contain any pointer, because it is sent to others worker via TCP
 */
struct ClientSession
{
    /** Family name (also called barrack name) */
    unsigned char familyName [64];

    /** Number of characters registered in the barrack */
    uint8_t charactersBarrackCount;

    /** Unique account ID */
    uint64_t accountId;

    /** Unique current commander ID */
    uint64_t currentCommanderId;

    /** PCID of the current character */
    uint32_t currentPCId;
};

typedef struct ClientSession ClientSession;

/**
 * @brief ClientSessionPrivileges enumerates the different levels of privileges
 *  for an account.
 */
typedef enum {
    CLIENT_SESSION_PRIVILEGES_ADMIN   = 0,
    CLIENT_SESSION_PRIVILEGES_UNKNOWN = 1,
    CLIENT_SESSION_PRIVILEGES_GM      = 2,
    CLIENT_SESSION_PRIVILEGES_PLAYER  = 3
} ClientSessionPrivileges;

// ----------- Functions ------------

/**
 * @brief Allocate a new ClientSession structure.
 * @return A pointer to an allocated ClientSession.
 */
ClientSession *
ClientSession_new (
);


/**
 * @brief Initialize an allocated ClientSession structure.
 * @param self An allocated ClientSession to initialize.
 * @return true on success, false otherwise.
 */
bool
ClientSession_init (
    ClientSession *self
);


/**
 * @brief Prints a ClientSession structure.
 * @param self An allocated ClientSession
 */
void
ClientSession_print (
    ClientSession *self
);

/**
 * @brief Free an allocated ClientSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated ClientSession.
 */
void
ClientSession_destroy (
    ClientSession **self
);
