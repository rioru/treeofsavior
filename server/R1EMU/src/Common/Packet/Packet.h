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
#include "PacketType.h"

// ---------- Defines -------------


// ------ Structure declaration -------
/**
 * @brief ClientPacketHeader is the header of each packet sent by the client.
 */
#pragma pack(push, 1)
typedef struct ClientPacketHeader
{
    uint16_t type;
    uint32_t sequence;
    uint32_t checksum;
}   ClientPacketHeader;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct CryptPacketHeader
{
    uint16_t plainSize;
}   CryptPacketHeader;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct ServerPacketHeader
{
    uint16_t type;
    uint32_t reserved;
}   ServerPacketHeader;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct VariableSizePacketHeader
{
    ServerPacketHeader serverHeader;
    uint16_t packetSize;
}   VariableSizePacketHeader;
#pragma pack(pop)


#pragma pack(push, 1)
typedef struct BarrackPacketNormalHeader
{
    VariableSizePacketHeader variableSizeHeader;
    uint32_t subtype;
}   BarrackPacketNormalHeader;
#pragma pack(pop)


// ----------- Functions ------------

/**
 * @brief Unwrap the client packet header and stores it in the ClientPacketHeader argument
 * @param[in,out] packet A pointer to the packet to unwrap. Afterward, it points to the data of the packet.
 * @param[in,out] packetSize A pointer to the packetSize. Afterward, it is updated.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void
ClientPacket_unwrapHeader (
    unsigned char **packet,
    size_t *packetSize,
    ClientPacketHeader *header,
    bool isCrypted
);

/**
 * @brief Unwrap the crypt packet header and stores it in the header argument
 * @param[in,out] packet A pointer to the packet to unwrap. Afterward, it points to the data of the packet.
 * @param[in,out] packetSize A pointer to the packetSize. Afterward, it is updated.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void
CryptPacket_unwrapHeader (
    unsigned char **packet,
    size_t *packetSize,
    CryptPacketHeader *header
);

/**
 * @brief Get the crypt packet header and stores it in the header argument
 * @param[in] packet A pointer to the packet to decode.
 * @param[out] header The header unwrapped from the packet
 * @return
 */
void
CryptPacket_getHeader (
    unsigned char *packet,
    CryptPacketHeader *header
);

/**
 * @brief Creates a barrack normal header based on the subtype argument
 * @param[out] normalHeader An allocated normal header
 * @param[in] subtype The subtype of the normal barrack packet
 * @param[in] packetSize The total size of the packet
 * @return
 */
void
BarrackPacket_normalHeader (
    BarrackPacketNormalHeader *normalHeader,
    uint32_t subtype,
    uint32_t packetSize
);
