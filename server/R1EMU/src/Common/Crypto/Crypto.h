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
 * @brief Unwrap the client packet header and stores it in the ClientPacketHeader argument
 * @param[out] header The header containing the crypto information of the packet
 * @param[in,out] packet The packet ciphered. After this call, the packet is decrypted.
 * @return true on success, false otherwise
 */
bool
Crypto_uncryptPacket (
    CryptPacketHeader *header,
    unsigned char **packet
);
