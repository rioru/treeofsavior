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
    ClientPacketHeader *header
) {
    memcpy (header, *packet, sizeof (ClientPacketHeader));

    *packet = (*packet) + sizeof (ClientPacketHeader);
}

void
CryptPacket_unwrapHeader (
    unsigned char **packet,
    CryptPacketHeader *header
) {
    memcpy (header, *packet, sizeof (CryptPacketHeader));

    *packet = (*packet) + sizeof (CryptPacketHeader);
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
