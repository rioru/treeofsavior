/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file utils.h
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


// ------ Structure declaration -------


// ----------- Functions ------------

/**
 * @brief Convert a log dump to memory
 * @param dump a formated dump, for instance :
 * [11:09:59][           ToSClient:                     dbgBuffer]  56 0B FF FF FF FF 2B 00 10 00 00 00 00 16 00 00 | V.....+.........
 * [11:09:59][           ToSClient:                     dbgBuffer]  00 13 9D 01 00 01 D1 A8 01 44 00 00 00 00 D0 13 | .........D......
 * [11:09:59][           ToSClient:                     dbgBuffer]  00 00 01 00 00 00 00 00 00 00 00                | ...........
 * @param memory target memory. Can be NULL
 * @param memSize the size of the target memory
 * @return memory pointer (newly allocated if memory is NULL)
 */

void *dumpToMem (
    char *dump,
    void *memory,
    size_t *memSize
);