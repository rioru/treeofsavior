/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file RouterMonitor.h
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
#define ROUTER_MONITOR_FDKEY_SIZE ((sizeof (int) * 2) + 1)
#define ROUTER_MONITOR_SUBSCRIBER_ENDPOINT "inproc://routerMonitorSubscriber-%d"

/** All the Router Monitor Packet headers */
typedef enum {
    ROUTER_MONITOR_ADD_FD
} RouterMonitorHeader;


// ------ Structure declaration -------
typedef struct RouterMonitor RouterMonitor;

typedef struct {

    /** The Router Id */
    uint16_t routerId;

    /** The Router frontend socket */
    zsock_t *frontend;

} RouterMonitorStartupInfo;

// ----------- Functions ------------

/**
 * @brief Allocate a new RouterMonitor structure.
 * @param info A RouterMonitorStartupInfo initialized
 * @return A pointer to an allocated RouterMonitor, or NULL if an error occured.
 */
RouterMonitor *
RouterMonitor_new (
    RouterMonitorStartupInfo *info
);


/**
 * @brief Initialize an allocated RouterMonitor structure.
 * @param self An allocated RouterMonitor to initialize.
 * @param info A RouterMonitorStartupInfo initialized
 * @return true on success, false otherwise.
 */
bool
RouterMonitor_init (
    RouterMonitor *self,
    RouterMonitorStartupInfo *info
);


/**
 * @brief Allocate a new RouterMonitorStartupInfo structure.
 * @param
 * @return A pointer to an allocated RouterMonitorStartupInfo, or NULL if an error occured.
 */
RouterMonitorStartupInfo *
RouterMonitorStartupInfo_new (
    zsock_t *frontend,
    uint16_t routerId
);


/**
 * @brief Initialize an allocated RouterMonitorStartupInfo structure.
 * @param self An allocated RouterMonitorStartupInfo to initialize.
 * @param
 * @return true on success, false otherwise.
 */
bool
RouterMonitorStartupInfo_init (
    RouterMonitorStartupInfo *self,
    zsock_t *frontend,
    uint16_t routerId
);


/**
 * @brief Start the Router Monitor
 * @param info a RouterMonitorStartupInfo initialized
 */
void *
RouterMonitor_start (
    void *info
);


/**
 * @brief Initialize the FD key from the fd value
 * @param fd The file descriptor
 * @param[out] fdKey The formated fd key
 * @return
 */
void
RouterMonitor_genKey (
    int fd,
    unsigned char fdKey[ROUTER_MONITOR_FDKEY_SIZE]
);


/**
 * @brief Free the content of a RouterMonitor structure.
 * @param self A pointer to an allocated RouterMonitor.
 */
void
RouterMonitor_free (
    RouterMonitor *self
);


/**
 * @brief Free an allocated RouterMonitor structure and nullify the content of the pointer.
 * @param self A pointer to an allocated RouterMonitor.
 */
void
RouterMonitor_destroy (
    RouterMonitor **self
);


/**
 * @brief Free an allocated RouterMonitorStartupInfo structure and nullify the content of the pointer.
 * @param self A pointer to an allocated RouterMonitorStartupInfo.
 */
void
RouterMonitorStartupInfo_destroy (
    RouterMonitorStartupInfo **_self
);
