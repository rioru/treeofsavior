/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file dbg.h
 * @brief Provides debug and dump functions
 * @date 2015-03-05-20.13
 *
 * Provides debug functions with multiple levels and dump utilities.
 *
 */


#pragma once

// ---------- Includes ------------
#include <stdio.h>
#include <czmq.h>

typedef enum {
    DBG_LEVEL_DEBUG,
    DBG_LEVEL_WARNING,
    DBG_LEVEL_ERROR,
} DbgLevel;


// ---------- Defines -------------
#if defined(__DBG_ACTIVATED__) && __DBG_ACTIVATED__ == TRUE

    /** Get the name of the module object from the filename. __FILE__ differs between GCC on MinGW and Linux. */
    #define __FILENAME_WIN32__ (((strrchr(__FILE__, '\\')) != NULL) ? &(strrchr(__FILE__, '\\'))[1] : __FILE__)
    #define __FILENAME_LINUX__ (((strrchr(__FILE__,  '/')) != NULL) ? &(strrchr(__FILE__,  '/'))[1] : __FILE__)
    #ifdef WIN32
    #define __FILENAME__ __FILENAME_WIN32__
    #else
    #define __FILENAME__ __FILENAME_LINUX__
    #endif

    /** Debug line template */
    #define dbg_ex(level, output, format, ...)                            \
        do {                                                              \
            char *__time_now__ = zclock_timestr();                        \
            _dbg (level, output, "[%s][%s:%d in %s] " format,             \
                __time_now__,                                             \
                __FILENAME__,                                             \
                __LINE__,                                                 \
                __FUNCTION__,                                             \
                ##__VA_ARGS__);                                           \
                zstr_free (&__time_now__);                                \
        } while (0)

    /** Buffer dump template */
    #define buffer_print_ex(buffer, size, prefix)                         \
        do {                                                              \
            char *__time_now__ = zclock_timestr();                        \
            char *__prefix__ = zsys_sprintf (                             \
                "[%s][%s:%d in %s] %s",                                   \
                __time_now__,                                             \
                __FILENAME__,                                             \
                __LINE__,                                                 \
                __FUNCTION__,                                             \
                (prefix) ? prefix : "");                                  \
                _buffer_print (buffer, size, __prefix__);                 \
                zstr_free (&__prefix__);                                  \
                zstr_free (&__time_now__);                                \
        } while (0)

    // Declare debug functions here
    /** Basic level debug function. */
    #define dbg(format, ...)                                              \
        dbg_ex (DBG_LEVEL_DEBUG, stdout, format "\n", ##__VA_ARGS__)

    /** Warning level debug function. */
    #define warning(format, ...)                                          \
        dbg_ex (DBG_LEVEL_WARNING, stderr, "[WARNING] " format "\n", ##__VA_ARGS__)

    /** Error level debug function. */
    #define error(format, ...)                                            \
        dbg_ex (DBG_LEVEL_ERROR, stderr, "[ERROR] " format "\n", ##__VA_ARGS__)

    /** Fatal error level debug function. */
    #define die(format, ...)                                              \
        do {                                                              \
            dbg_ex (DBG_LEVEL_ERROR, stderr, "[FATAL ERROR] " format "\n", ##__VA_ARGS__); \
            exit (-1);                                                    \
        } while (0)

    /** Dump a buffer into the standard output.
     * See _buffer_print for the arguments documentation. */
    #define buffer_print(buffer, size, prefix) \
        buffer_print_ex (buffer, size, prefix)

#else

    // Don't output anything if __DBG_ACTIVATED__ is not enabled
	#define dbg(format, ...)
	#define warning(format, ...)
	#define error(format, ...)
	#define die(format, ...)
	#define buffer_print(format, ...)

#endif


// ----------- Functions ------------

/**
 * @brief Output a formated message to a chosen stream
 * @param level The debug level
 * @param output A destination stream
 * @param format the format of the message
 * @return
 */
void _dbg (
    int level,
    FILE *output,
    char *format,
    ...
);

/**
 * @brief Dump a buffer in the standard output
 * @param buffer An allocated buffer to dump
 * @param bufferSize The buffer size
 * @param prefix A string printed before each line of the dump (optional)
 * @return
 */
void
_buffer_print (
    void *buffer,
    int bufferSize,
    char *prefix
);
