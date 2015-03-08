/**
 *
 *   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
 *   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
 *   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
 *   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
 *   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
 *   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
 *
 * @file Packet.h
 * @brief
 *
 *
 *
 * @license <license placeholder>
 */

#pragma once

// ---------- Includes ------------
#include "R1EMU.h"
#include "PacketType/PacketType.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/**
 * @brief ClientPacketHeader is the header of each packet sent by the client.
 */
#pragma pack(push, 1)
struct ClientPacketHeader
{
    uint16_t type;
    uint32_t sequence;
    uint16_t checksum;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct CryptPacketHeader
{
    uint16_t size;
};
#pragma pack(pop)


#pragma pack(push, 1)
struct ServerPacketHeader
{
    uint16_t type;
    uint32_t reserved;
};
#pragma pack(pop)

typedef struct ClientPacketHeader ClientPacketHeader;
typedef struct ServerPacketHeader ServerPacketHeader;
typedef struct CryptPacketHeader CryptPacketHeader;

// ----------- Functions ------------

/**
 * @brief Unwrap the client packet header and stores it in the ClientPacketHeader argument
 * @param[in,out] packet A pointer to the packet to unwrap. Afterward, it points to the data of the packet.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void
ClientPacket_unwrapHeader (
    unsigned char **packet,
    ClientPacketHeader *header
);

/**
 * @brief Unwrap the crypt packet header and stores it in the CryptPacketHeader argument
 * @param[in,out] packet A pointer to the packet to unwrap. Afterward, it points to the data of the packet.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void
CryptPacket_unwrapHeader (
    unsigned char **packet,
    CryptPacketHeader *header
);
