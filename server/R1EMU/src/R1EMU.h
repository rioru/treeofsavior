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
#define BARRACK_SERVER_ROUTER_ID ((uint16_t) -1)
#define GLOBAL_SERVER_ROUTER_ID  ((uint16_t) -2)

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
