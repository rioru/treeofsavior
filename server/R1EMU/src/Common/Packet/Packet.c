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
#include "Packet.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation ------

void
ClientPacket_unwrapHeader (
    unsigned char **packet,
    size_t *packetSize,
    ClientPacketHeader *header,
    bool isCrypted
) {
    int headerSize = sizeof (ClientPacketHeader);

    if (!isCrypted) {
        headerSize -= sizeof_struct_member (ClientPacketHeader, checksum);
    }

    memcpy (header, *packet, headerSize);

    *packet = (*packet) + headerSize;
    *packetSize -= headerSize;
}

void
CryptPacket_unwrapHeader (
    unsigned char **packet,
    size_t *packetSize,
    CryptPacketHeader *header
) {
    memcpy (header, *packet, sizeof (CryptPacketHeader));

    *packet = (*packet) + sizeof (CryptPacketHeader);
    *packetSize -= sizeof (CryptPacketHeader);
}

void
CryptPacket_getHeader (
    unsigned char *packet,
    CryptPacketHeader *header
) {
    memcpy (header, packet, sizeof (CryptPacketHeader));
}

void
PacketNormal_header (
    PacketNormalHeader *normalHeader,
    uint32_t subtype,
    uint32_t packetSize
) {
    PacketVariableSize_header (&normalHeader->variableSizeHeader, BC_NORMAL, packetSize);
    normalHeader->subtype = subtype;
}

void
PacketVariableSize_header (
    VariableSizePacketHeader *variableSizeHeader,
    uint16_t packetType,
    uint32_t packetSize
) {
    PacketServer_header (&variableSizeHeader->serverHeader, packetType);
    variableSizeHeader->packetSize = packetSize;
}

void
PacketServer_header (
    ServerPacketHeader *serverHeader,
    uint16_t packetType
) {
    serverHeader->type = packetType;
    serverHeader->reserved = -1;
}
