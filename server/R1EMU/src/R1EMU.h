/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file R1EMU.h
 * @brief Global variables shared between all the files
 *
 *  It also contains utilities functions.
 *
 */

#pragma once

// Integers printf format
#include <inttypes.h>

// CZMQ environment
#include <zmq.h>
#include <czmq.h>
/** This is only for differenciate ZMQ RAW ROUTER from ZMQ ROUTER */
#define ZMQ_RAW_ROUTER ZMQ_ROUTER
#define PACKET_HEADER(x) (typeof(x)[]){x}
/** Router unique IDs */
typedef enum RouterUniqueId {
    BARRACK_SERVER_ROUTER_ID = 0,
    GLOBAL_SERVER_ROUTER_ID = -1
} RouterUniqueId;

// MySQL environment
#include <mysql/mysql.h>

// Redis environment
#include <hiredis.h>

// Debugging environment
/** If defined, the debugging functions will be included in the executable */
#define __DBG_ACTIVATED__ TRUE
#include "Common/dbg/dbg.h"

// Configuration environment
#define DEFAULT_SERVER_CONF_PATH "../cfg/server.cfg"

// Utils
#include "Common/utils/utils.h"
#define STRINGIFY(x) # x
#ifndef bool
#define bool char
#endif
#ifndef false
#define false 0
#endif
#ifndef true
#define true (!false)
#endif

// Size of local array keyword
#define sizeof_array(array) \
    ((int)(sizeof(array) / sizeof(*(array))))

// Size of structure member
#define sizeof_struct_member(struct, member) \
   (sizeof(((struct *)0)->member))

// Quotify a string
#define QUOTIFY(x) "\"" x "\""

