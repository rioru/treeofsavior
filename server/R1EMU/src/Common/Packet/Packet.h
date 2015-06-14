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
#define BUILD_REPLY_PACKET(packetName, msgName) \
    for (bool __sent = false; \
         !__sent && memset (&packetName, 0, sizeof (packetName)); \
         zmsg_add (msgName, zframe_new (&packetName, sizeof (packetName))), __sent = true \
    )


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
typedef struct PacketNormalHeader
{
    VariableSizePacketHeader variableSizeHeader;
    uint32_t subtype;
}   PacketNormalHeader;
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
 * @brief Creates a server header
 * @param[out] variableSizeHeader An allocated server header
 * @param[in] packetSize The type of the packet
 * @return
 */
void
PacketServer_header (
    ServerPacketHeader *serverHeader,
    uint16_t packetType
);

/**
 * @brief Creates a variable size header
 * @param[out] variableSizeHeader An allocated variable size header
 * @param[in] packetSize The type of the packet
 * @param[in] packetSize The total size of the packet
 * @return
 */
void
PacketVariableSize_header (
    VariableSizePacketHeader *variableSizeHeader,
    uint16_t packetType,
    uint32_t packetSize
);

/**
 * @brief Creates a normal header based on the subtype argument
 * @param[out] normalHeader An allocated normal header
 * @param[in] subtype The subtype of the normal barrack packet
 * @param[in] packetSize The total size of the packet
 * @return
 */
void
PacketNormal_header (
    PacketNormalHeader *normalHeader,
    uint32_t subtype,
    uint32_t packetSize
);
