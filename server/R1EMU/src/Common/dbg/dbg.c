#include "dbg.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/**
 * @brief Output a formated message to a chosen stream
 * @param output A destination stream
 * @param format the format of the message
 * @return
 */
void _dbg (
    FILE *output,
    char *format,
    ...
) {
    va_list args;

    va_start (args, format);
        vfprintf (output, format, args);
        fflush (output);
    va_end (args);
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
    unsigned char *buffer,
    int bufferSize,
    char *prefix
) {
    static bool isPrinting = false;
    int curPos = 0;

    while (isPrinting) {
        zclock_sleep (1);
    }

    isPrinting = true;

    printf ("%s ===== [buffer size = %d (0x%x) ================\n", prefix, bufferSize, bufferSize);
    while (curPos < bufferSize) {
        int offset;
        printf ("%s", prefix);
        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            printf (" %02X", buffer[curPos]);
        }
        if (offset != 16) {
            for (int j = 0; j < 16 - offset; j++) {
                printf("   ");
            }
        }

        printf (" | ");
        curPos -= offset;

        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            unsigned char c = buffer[curPos];
            printf ("%c", isprint(c) ? c : '.');
        }

        printf("\n");
    }
    printf ("%s=================================================\n", prefix);

    isPrinting = false;
}
