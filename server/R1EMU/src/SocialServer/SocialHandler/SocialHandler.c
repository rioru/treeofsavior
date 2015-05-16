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
#include "SocialHandler.h"
#include "Common/Packet/Packet.h"
#include "Common/Commander/Commander.h"
#include "Common/Packet/PacketStream.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Server/Worker.h"
#include "BarrackServer/BarrackServer.h"


// ------ Static declaration -------
/** On log in */
static PacketHandlerState SocialHandler_login        (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);


// ------ Structure declaration -------
/**
 * @brief socialHandlers is a global table containing all the social handlers.
 */
const PacketHandler socialHandlers [PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CS_LOGIN, SocialHandler_login),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState
SocialHandler_login (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {

    return PACKET_HANDLER_OK;
}
