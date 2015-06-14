/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @license <license placeholder>
 */

// ---------- Includes ------------
#include "dbg.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// ------ Structure declaration -------
zmutex_t *mutex = NULL;
FILE *_output = NULL;

// ------ Static declaration -------

// ------ Extern function implementation ------

void
dbg_set_output (
    FILE *output
) {
    _output = output;
}


void
dbg_close (
    void
) {
    if (_output && _output != stdout && _output != stderr) {
        fclose (_output);
    }

    _output = stdout;
}

void _dbg (
    int level,
    char *format,
    ...
) {
    va_list args;

    if (mutex == NULL) {
        mutex = zmutex_new ();
    }

    if (_output == NULL) {
        _output = stdout;
    }

    zmutex_lock (mutex);

    switch (level) {
        #ifdef WIN32
        case DBG_LEVEL_INFO:    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0A); break;
        case DBG_LEVEL_DEBUG: break;
        case DBG_LEVEL_WARNING: SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0E); break;
        case DBG_LEVEL_ERROR:   SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0C); break;
        case DBG_LEVEL_SPECIAL: SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0B); break;
        #else
        case DBG_LEVEL_INFO: fprintf (_output, "\x1b[32m"); break;
        case DBG_LEVEL_DEBUG: fprintf (_output, "\x1b[37m"); break;
        case DBG_LEVEL_WARNING: fprintf (_output, "\x1b[33m"); break;
        case DBG_LEVEL_ERROR: fprintf (_output, "\x1b[31m"); break;
        case DBG_LEVEL_SPECIAL: fprintf (_output, "\x1b[35m"); break;
        #endif
    }

    va_start (args, format);
        vfprintf (_output, format, args);
    va_end (args);

    #ifdef WIN32
    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x07);
    #else
    fprintf (_output, "\033[0m");
    #endif

    fflush (_output);

    zmutex_unlock (mutex);
}

/**
 * @brief Dump a buffer in the standard output
 * @param buffer An allocated buffer to dump
 * @param bufferSize The buffer size
 * @param prefix A string printed before each line of the dump
 * @return
 */
void
_buffer_print (
    void *buffer,
    int bufferSize,
    char *prefix
) {
    if (mutex == NULL) {
        mutex = zmutex_new ();
    }

    zmutex_lock (mutex);

    int curPos = 0;

    printf ("%s ===== buffer size = %d (0x%x) ================\n", prefix, bufferSize, bufferSize);
    while (curPos < bufferSize) {
        int offset;
        printf ("%s", prefix);
        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            printf (" %02X", ((uint8_t *) buffer)[curPos]);
        }
        if (offset != 16) {
            for (int j = 0; j < 16 - offset; j++) {
                printf("   ");
            }
        }

        printf (" | ");
        curPos -= offset;

        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            uint8_t c = ((uint8_t *) buffer)[curPos];
            printf ("%c", isprint(c) ? c : '.');
        }

        printf("\n");
    }
    printf ("%s=================================================\n", prefix);

    zmutex_unlock (mutex);
}
