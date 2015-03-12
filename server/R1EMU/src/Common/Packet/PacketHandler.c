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
#include "PacketHandler.h"
#include "Common/Packet/Packet.h"
#include "Common/Crypto/Crypto.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------
PacketHandlerState
PacketHandler_buildReply (
    const PacketHandler *handlers,
    size_t handlersCount,
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    PacketHandlerFunction handler;

    // Preconditions
    if (packetSize < sizeof (CryptPacketHeader)) {
        error ("The packet received is too small to be read. Ignore request.");
        return PACKET_HANDLER_ERROR;
    }

    // Unwrap the crypt packet header
    CryptPacketHeader cryptHeader;
    CryptPacket_unwrapHeader (&packet, &cryptHeader);
    if (packetSize - sizeof (cryptHeader) != cryptHeader.size) {
        error ("The real packet size (0x%x) doesn't match with the packet size in the header (0x%x). Ignore request.",
            packetSize, cryptHeader.size);
        return PACKET_HANDLER_ERROR;
    }

    // Uncrypt the packet
    if (!Crypto_uncryptPacket (&cryptHeader, &packet)) {
        error ("Cannot uncrypt the client packet. Ignore request.");
        return PACKET_HANDLER_ERROR;
    }

    // Read the packet
    ClientPacketHeader header;
    ClientPacket_unwrapHeader (&packet, &header);
    size_t dataSize = packetSize - sizeof (CryptPacketHeader) - sizeof (ClientPacketHeader);

    // Get the corresponding packet handler
    if (header.type > handlersCount) {
        error ("Invalid packet type. Ignore request.");
        return PACKET_HANDLER_ERROR;
    }

    // Test if a handler is associated with the packet type requested.
    if (!(handler = handlers [header.type].handler)) {
        error ("Cannot find handler for the requested packet type : %s",
            (header.type <PACKET_TYPES_MAX_INDEX) ?
               packetTypeInfo.packets[header.type].string : "UNKNOWN"
        );
        return PACKET_HANDLER_ERROR;
    }

    // Call the handler
    dbg ("Calling [%s] handler", packetTypeInfo.packets[header.type].string);
    return handler (session, packet, dataSize, reply);
}
