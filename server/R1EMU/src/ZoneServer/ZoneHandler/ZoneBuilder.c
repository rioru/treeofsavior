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
#include "ZoneBuilder.h"
#include "Common/Server/Worker.h"
#include "Common/Packet/Packet.h"
#include "Common/Packet/PacketType.h"

// ---------- Defines -------------
#define BUILD_REPLY_PACKET(structureName, packetName, msgName) \
    for (structureName packetName, *__sent = NULL; \
         !__sent && memset (&packetName, 0, sizeof (structureName)); \
         zmsg_add (msgName, zframe_new (&packetName, sizeof (structureName))), __sent = (void *) 1 \
    )

// ------ Static declaration -------

// ------ Structure declaration -------

// ------ Extern functions implementation -------

void
ZoneBuilder_restSit (
    uint32_t targetPcId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint8_t isSit;
    } ZcRestSitPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (ZcRestSitPacket, replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_REST_SIT;
        replyPacket.pcId = targetPcId;
        replyPacket.isSit = 0;
    }
}
