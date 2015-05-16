/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Crypto.h
 * @brief Crypto contains cryptographic functions needed for the communication between the client and the server
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "Common/Packet/Packet.h"

// ---------- Defines -------------



// ----------- Functions ------------


/**
 * @brief Unwrap the client packet header and decrypt the packet.
 * @param[in,out] packet The packet ciphered. After this call, the packet is decrypted.
 * @param[in] packetSize The crypted packet size
 * @return true on success, false otherwise
 */
bool
Crypto_decryptPacket (
    unsigned char **packet,
    size_t packetSize
);
