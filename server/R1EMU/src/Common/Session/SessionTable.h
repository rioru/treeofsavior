/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SessionTable.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Session/ClientSession.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/**
 * @brief SessionTable is
 *
 * SessionTable is
 */
typedef struct SessionTable SessionTable;

// ----------- Functions ------------

/**
 * @brief Allocate a new SessionTable structure.
 * @return A pointer to an allocated SessionTable.
 */
SessionTable *
SessionTable_new (
    void
);


/**
 * @brief Initialize an allocated SessionTable structure.
 * @param self An allocated SessionTable to initialize.
 * @return true on success, false otherwise.
 */
bool
SessionTable_init (
    SessionTable *self
);


/**
 * @brief Look up a session in the sessions hashtable
 * @param self The session table
 * @param sessionKey The session key to lookup
 * @return A client session or NULL if it doesn't exists
 */
ClientSession *
SessionTable_lookup (
    SessionTable *self,
    unsigned char *key
);


/**
 * @brief Look up a session in the sessions hashtable based on the account ID.
 *        This function is O(N), not O(1) !
 * @param self The session table
 * @param accountId The account ID to lookup
 * @param sessionKey The session key to lookup
 * @return A client session or NULL if it doesn't exists
 */
ClientSession *
SessionTable_getByAccountId (
    SessionTable *self,
    uint64_t accountId,
    const unsigned char **key
);

/**
 * @brief Create a session in the sessions hashtable
 * @param self The session table
 * @param sessionKey The session key to create
 * @return A new client session
 */
ClientSession *
SessionTable_create (
    SessionTable *self,
    unsigned char *sessionKey
);


/**
 * @brief Delete a session in the sessions hashtable
 * @param self The session table
 * @param sessionKey The session key to delete
 * @return
 */
void
SessionTable_delete (
    SessionTable *self,
    unsigned char *key
);

/**
 * @brief Free an allocated SessionTable structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SessionTable.
 */
void
SessionTable_destroy (
    SessionTable **self
);

