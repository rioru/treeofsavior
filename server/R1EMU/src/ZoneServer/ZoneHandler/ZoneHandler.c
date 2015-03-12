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
#include "ZoneHandler.h"
#include "Common/Packet/Packet.h"
#include "Common/Commander/Commander.h"
#include "Common/Packet/PacketStream.h"

// ------ Static declaration -------
/** Connect to the zone server */
static PacketHandlerState ZoneHandler_connect (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);


// ------ Structure declaration -------
/**
 * @brief zoneHandlers is a global table containing all the zone handlers.
 */
const PacketHandler zoneHandlers [ZONE_HANDLER_ARRAY_SIZE] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CZ_CONNECT, ZoneHandler_connect),

    #undef REGISTER_PACKET_HANDLER
};



static PacketHandlerState
ZoneHandler_connect (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    buffer_print (packet, packetSize, "CONNECT : ");

    return PACKET_HANDLER_OK;
}
