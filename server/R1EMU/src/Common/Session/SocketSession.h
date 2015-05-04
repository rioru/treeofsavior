/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file SocketSession.h
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
#define SOCKET_SESSION_UNDEFINED_MAP -1
#define SOCKET_SESSION_UNDEFINED_ACCOUNT -1

#define SOCKET_SESSION_KEY_SIZE 11

// ------ Structure declaration -------

/**
 * @brief SocketSession identifies a session socket to an account
 */
struct SocketSession
{
    /** The account ID of the account associated with the socket */
    uint64_t accountId;
    /** The zone server handling the client socket */
    uint16_t zoneId;
    /** The map Id of the commander currently played */
    uint16_t mapId;
    /** Socket Redis key */
    unsigned char key[SOCKET_SESSION_KEY_SIZE];

    /** States */
    bool authenticated;
};

typedef struct SocketSession SocketSession;

// ----------- Functions ------------

/**
 * @brief Allocate a new SocketSession structure.
 * @return A pointer to an allocated SocketSession.
 */
SocketSession *
SocketSession_new (
    uint64_t accountId,
    uint16_t zoneId,
    uint16_t mapId,
    unsigned char *socketIdKey,
    bool authenticated
);


/**
 * @brief Initialize an allocated SocketSession structure.
 * @param self An allocated SocketSession to initialize.
 * @return true on success, false otherwise.
 */
bool
SocketSession_init (
    SocketSession *self,
    uint64_t accountId,
    uint16_t zoneId,
    uint16_t mapId,
    unsigned char *socketIdKey,
    bool authenticated
);

/**
 * @brief Format a session key from the session id
 * @param sessionId The sessionId of the session requested
 * @param[out] sessionKey The sessionKey generated
 * @param sessionKeySize The sessionKey size
 * @return
 */
void
SocketSession_genKey (
    unsigned char *sessionId,
    unsigned char *sessionKey,
    size_t sessionKeySize
);

/**
 * @brief Prints a SocketSession structure.
 * @param self An allocated SocketSession
 */
void
SocketSession_print (
    SocketSession *self
);

/**
 * @brief Free an allocated SocketSession structure and nullify the content of the pointer.
 * @param self A pointer to an allocated SocketSession.
 */
void
SocketSession_destroy (
    SocketSession **self
);

