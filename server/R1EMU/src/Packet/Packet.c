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
