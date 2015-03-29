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

// Integers printf format
#include <inttypes.h>

// CZMQ environment
#include <zmq.h>
#include <czmq.h>
/** This is only for differenciate ZMQ RAW ROUTER from ZMQ ROUTER */
#define ZMQ_RAW_ROUTER ZMQ_ROUTER
#define PACKET_HEADER(x) (int[]){x}

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

// ----------- Functions ------------
/**
 * @brief High precision timestamp.
 * @return A 64 bit timestamp
 */
uint64_t zmq_clock_now ();


/**
 * @brief Return a unique seed for the random number generator.
 * @param customData A user data
 * @return a unique seed
 */
uint32_t
R1EMU_seed_random (
    uint32_t customData
);

/**
 * @brief Generates random 32 bits value.
 */
uint32_t
R1EMU_generate_random (
    uint32_t *seed
);

/**
 * @brief Generates random 64 bits value.
 */
uint64_t
R1EMU_generate_random64 (
    uint32_t *seed
);

int
rand_r (
    unsigned int *seed
);
