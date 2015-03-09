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

// CZMQ environnement
#include <zmq.h>
#include <czmq.h>
/** This is only for differenciate ZMQ RAW ROUTER from ZMQ ROUTER */
#define ZMQ_RAW_ROUTER ZMQ_ROUTER


// Debugging environnement
/** If defined, the debugging functions will be included in the executable */
#define __DBG_ACTIVATED__ TRUE
#include "dbg/dbg.h"

// Configuration environnement
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
 * @brief Seeds the random number generator.
 */
void R1EMU_seed_random ();

/**
 * @brief Generates random 32 bits value.
 */
uint32_t R1EMU_generate_random ();

/**
 * @brief Generates random 64 bits value.
 */
uint64_t R1EMU_generate_random64 ();
