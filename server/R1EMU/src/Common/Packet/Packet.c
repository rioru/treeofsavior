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
BarrackPacket_normalHeader (
    BarrackPacketNormalHeader *normalHeader,
    uint32_t subtype,
    uint32_t packetSize
) {
    normalHeader->variableSizeHeader.serverHeader.type = BC_NORMAL;
    normalHeader->variableSizeHeader.serverHeader.reserved = 0;
    normalHeader->variableSizeHeader.packetSize = packetSize;
    normalHeader->subtype = subtype;
}
