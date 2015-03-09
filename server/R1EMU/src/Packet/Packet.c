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


// ------ Extern declaration -------

void
ClientPacket_unwrapHeader (
    unsigned char **packet,
    ClientPacketHeader *header
) {
    memcpy (header, *packet, sizeof (*header));

    *packet = (*packet) + sizeof (*header);
}

void
CryptPacket_unwrapHeader (
    unsigned char **packet,
    CryptPacketHeader *header
) {
    memcpy (header, *packet, sizeof (*header));

    *packet = (*packet) + sizeof (*header);
}

void
BarrackPacket_normalHeader (
    BarrackPacketNormalHeader *normalHeader,
    uint32_t subtype,
    uint32_t packetSize
) {
    normalHeader->variableSizeHeader.type = BC_NORMAL;
    normalHeader->variableSizeHeader.reserved = 0;
    normalHeader->variableSizeHeader.packetSize = packetSize;
    normalHeader->subtype = subtype;
}
