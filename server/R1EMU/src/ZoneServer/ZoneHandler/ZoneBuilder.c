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

void
ZoneBuilder_skillReady (
    uint32_t targetPcId,
    uint32_t skillId,
    Position3D *pos1,
    Position3D *pos2,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t skillId;
        float unk3;
        uint16_t unk4;
        uint16_t unk5;
        Position3D pos1;
        Position3D pos2;
    } ZcSkillReadyPacket;
    #pragma pack(pop)

    /*  pcId     skillId  u3       u4        x        y        z        x2       y2       z2
        5A730100 419C0000 0000803F 011C ECC7 DAFD24C4 74768243 1B77F1C3 DAFD24C4 74768243 1B77F1C3
        5A730100 419C0000 0000803F 011C ECC7 720344C4 74768243 2178F6C3 720344C4 74768243 2178F6C3
        5A730100 419C0000 0000803F 011C ECC7 C9EC91C4 74768243 17060AC4 C9EC91C4 74768243 17060AC4
    */
    BUILD_REPLY_PACKET (ZcSkillReadyPacket, replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_SKILL_READY;
        replyPacket.pcId = targetPcId; // session->game.currentCommander.pcId;
        replyPacket.skillId = skillId;
        replyPacket.unk3 = 1.0;
        replyPacket.unk4 = SWAP_UINT16 (0x011C);
        replyPacket.unk5 = SWAP_UINT16 (0xECC7);
        memcpy (&replyPacket.pos1, pos1, sizeof (replyPacket.pos1));
        memcpy (&replyPacket.pos2, pos1, sizeof (replyPacket.pos2));
    }
}

void
ZoneBuilder_playAni (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t unkId1;
        uint32_t unkId2;
        uint16_t unk3;
        float timeDelay;
        float unk5;
    } ZcPlayAniPacket;
    #pragma pack(pop)

    /*
        4B010000 25182700 0101 00000000 0000803F
    */

    BUILD_REPLY_PACKET (ZcPlayAniPacket, replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_PLAY_ANI;
        replyPacket.unkId1 = SWAP_UINT32 (0x4B010000);
        replyPacket.unkId2 = SWAP_UINT32 (0x25182700);
        replyPacket.unk3 = SWAP_UINT16 (0x0101);
        replyPacket.timeDelay = 0;
        replyPacket.unk5 = SWAP_UINT32 (0x0000803F);
    }
}


void
ZoneBuilder_skillCast (
    uint32_t targetPcId,
    uint32_t skillId,
    Position3D *position1,
    Position3D *position2,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint16_t unk2;
        uint16_t unk3;
        Position3D position1;
        Position3D position2;
    } ZcSkillCastPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (ZcSkillCastPacket, replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_SKILL_CAST;
        replyPacket.pcId = targetPcId;
        replyPacket.unk2 = 0xECC7;
        replyPacket.unk3 = 0;
        memcpy (&replyPacket.position1, position1, sizeof (Position3D));
        memcpy (&replyPacket.position2, position2, sizeof (Position3D));
    }
}

void
ZoneBuilder_playSkillCastAni (
    uint32_t targetPcId,
    Position3D *position,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        Position3D position;
        uint16_t unk1;
        uint16_t unk2;
        uint32_t unk3;
    } ZcPlaySkillCastAniPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (ZcPlaySkillCastAniPacket, replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_PLAY_SKILL_CAST_ANI;
        replyPacket.pcId = targetPcId;
        memcpy (&replyPacket.position, position, sizeof (Position3D));
        replyPacket.unk1 = 0xECC7;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = 0;
    }
}
