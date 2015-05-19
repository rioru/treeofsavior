/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file zlib.h
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
#define ZLIB_MAGIC_HEADER 0xFA8D

// ------ Structure declaration -------
typedef struct {
    uint16_t magic;
    uint16_t size;
} ZlibHeader;

typedef struct {
    ZlibHeader header;
    char buffer [0x40000];
} ZlibPacket;

// ----------- Functions ------------
/**
 * @brief : Compress a given data to a ZlibPacket
 */
bool
ZlibPacket_compress (
    ZlibPacket *self,
    char *data,
    size_t dataSize
);

/**
 * @brief : Decompress a given data to a ZlibPacket
 */
bool
ZlibPacket_decompress (
    ZlibPacket *self,
    char *data,
    size_t dataSize
);
