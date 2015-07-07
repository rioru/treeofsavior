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

// ------ Static declaration -------

// ------ Structure declaration -------

// ------ Extern functions implementation -------

void
ZoneBuilder_restSit (
    uint32_t targetPcId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint8_t isSit;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
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
    PositionXYZ *pos1,
    PositionXYZ *pos2,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint32_t skillId;
        float unk3;
        uint16_t unk4;
        uint16_t unk5;
        PositionXYZ pos1;
        PositionXYZ pos2;
    } replyPacket;
    #pragma pack(pop)

    /*  pcId     skillId  u3       u4        x        y        z        x2       y2       z2
        5A730100 419C0000 0000803F 011C ECC7 DAFD24C4 74768243 1B77F1C3 DAFD24C4 74768243 1B77F1C3
        5A730100 419C0000 0000803F 011C ECC7 720344C4 74768243 2178F6C3 720344C4 74768243 2178F6C3
        5A730100 419C0000 0000803F 011C ECC7 C9EC91C4 74768243 17060AC4 C9EC91C4 74768243 17060AC4
    */
    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_SKILL_READY;
        replyPacket.pcId = targetPcId;
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
    struct {
        ServerPacketHeader header;
        uint32_t unkId1;
        uint32_t unkId2;
        uint16_t unk3;
        float timeDelay;
        float unk5;
    } replyPacket;
    #pragma pack(pop)

    /*
        4B010000 25182700 0101 00000000 0000803F
    */

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
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
    PositionXYZ *position1,
    PositionXYZ *position2,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        uint16_t unk2;
        uint16_t unk3;
        PositionXYZ position1;
        PositionXYZ position2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_SKILL_CAST;
        replyPacket.pcId = targetPcId;
        replyPacket.unk2 = 0xECC7;
        replyPacket.unk3 = 0;
        memcpy (&replyPacket.position1, position1, sizeof (PositionXYZ));
        memcpy (&replyPacket.position2, position2, sizeof (PositionXYZ));
    }
}

void
ZoneBuilder_playSkillCastAni (
    uint32_t targetPcId,
    PositionXYZ *position,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        PositionXYZ position;
        uint16_t unk1;
        uint16_t unk2;
        uint32_t unk3;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_PLAY_SKILL_CAST_ANI;
        replyPacket.pcId = targetPcId;
        memcpy (&replyPacket.position, position, sizeof (PositionXYZ));
        replyPacket.unk1 = 0xECC7;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = 0;
    }
}

void
ZoneBuilder_normalUnk8 (
    uint32_t targetPcId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t pcId;
        uint32_t unk1;
        uint32_t skillId;
        uint32_t skillLevel; // ? not sure
        uint8_t unk2[61];
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (
            "[11:37:20][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 59 00 54 00 00 00 36 F8 01 00 | 0.....Y.T...6...\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  11 18 27 00 41 9C 00 00 02 00 00 00 BC BD A9 C4 | ..'.A...........\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  6A 1F 02 43 57 94 34 C3 4C EF 2E B4 00 00 80 3F | j..CW.4.L......?\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  6A 21 22 B4 00 00 A0 41 1A 74 00 00 01 00 00 00 | j!.....A.t......\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:37:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00                      | .........\n"
          , &replyPacket, &memSize
        );

        replyPacket.pcId = targetPcId;
    }
}

void
ZoneBuilder_normalUnk9 (
    uint32_t targetPcId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t pcId;
        uint8_t unk1;
        uint32_t unk2;
        uint32_t unk3;
        uint32_t unk4;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (
            "[11:10:23][           ToSClient:                     dbgBuffer]  30 0D 2E D3 5E 00 1D 00 24 01 00 00 5A 73 01 00 | 0...^...$...Zs..\n"
            "[11:10:23][           ToSClient:                     dbgBuffer]  00 6E 00 00 00 FF FF FF FF 57 04 00 00          | .n.......W...\n"
          , &replyPacket, &memSize
        );

        replyPacket.pcId = targetPcId;
    }
}

void
ZoneBuilder_partyList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  82 0C FF FF FF FF 44 01 00 00 34 54 A3 00 00 00 | ......D...4T....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 16 00 00 00 13 9D 01 00 34 35 34 31 37 39 | ..........454179\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  01 30 36 36 35 30 31 33 D1 A8 01 44 00 00 00 00 | .0665013...D....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EA B5 AC EC 9B 90 EC 9E 90 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  F3 D4 56 00 30 35 E3 13 0B 05 04 00 46 CF 01 00 | ..V.05......F...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EA B5 AC EC 9B 90 EC 9E 90 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  43 42 54 EC 9A A9 00 00 00 00 00 00 00 00 00 00 | CBT.............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 DB 1F 00 0C 00 00 00 01 2E 55 00 45 95 09 00 | ..........U.E...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  4A 00 00 00 00 00 B4 C2 00 40 A2 44 00 D0 0A C5 | J........@.D....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EC 01 00 00 24 04 00 00 EC 01 00 00 24 04 00 00 | ....$.......$...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  12 00 84 1B 00 80 9D 46 76 1B 00 68 97 46 85 1B | .......Fv..h.F..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  18 72 BE 49                                     | .r.I\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_partyInfo (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  81 0C FF FF FF FF A4 00 00 00 34 54 A3 00 00 00 | ..........4T....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 16 00 00 00 13 9D 01 00 34 35 34 31 37 39 | ..........454179\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  39 30 36 36 35 30 31 33 34 00 36 00 36 00 35 00 | 906650134.6.6.5.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 00 31 00 33 00 34 00 00 00 00 00 00 00 00 00 | 0.1.3.4.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 D1 A8 01 44 00 00 00 00 EA B5 AC EC 9B 90 | .....D..........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EC 9E 90 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 02 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00                                     | ....\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_jobPts (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  18 0D FF FF FF FF 04 00 01 00                   | .........."
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_normalUnk7 (
    uint64_t accountId,
    uint32_t targetPcId,
    char *familyName,
    char *commanderName,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t unk1;
        uint32_t socialId;
        uint8_t unk2;
        uint64_t accountId;
        uint64_t accountId_2;
        char familyName [COMMANDER_FAMILY_NAME_SIZE];
        uint32_t unk3[3];
        uint32_t pcId;
        char familyName_2 [COMMANDER_FAMILY_NAME_SIZE];
        char commanderName [COMMANDER_NAME_SIZE];
        uint8_t unk4[48];
    } replyPacket;
    #pragma pack(pop)

    /*
        300D     FFFFFFFF 2501     E1000000 0016     00 0000  139D0100 D1A80144 00000000 D1A80144 00000000
        EAB5ACEC9B90EC9E9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
        F3D45600 3035E313 FD0309B8 5A730100
        EAB5ACEC9B90EC9E9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
        CommanderName
        43425432000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
        00000000 00040001
                                          x        y        z
        00000002 33560000 000000 01000000 00001DC4 00008243 002080C4
        69000000 77000000 69000000 77000000
    */

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 25 01 E1 00 00 00 00 16 00 00 | 0.....%.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 13 9D 01 00 D1 A8 01 44 00 00 00 00 D1 A8 01 | ........D.......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  44 00 00 00 00 EA B5 AC EC 9B 90 EC 9E 90 00 00 | D...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 F3 D4 56 00 30 35 E3 13 FD 03 09 | .......V.05.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  B8 5A 73 01 00 EA B5 AC EC 9B 90 EC 9E 90 00 00 | .Zs.............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 43 42 54 32 00 00 00 00 00 00 00 | .....CBT2.......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 04 00 01 00 00 00 02 33 56 | ..............3V\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 01 00 00 00 00 00 1D C4 00 00 82 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  43 00 20 80 C4 69 00 00 00 77 00 00 00 69 00 00 | C. ..i...w...i..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 77 00 00 00                                  | .w...\n"
            , &replyPacket, &memSize
        );

        replyPacket.accountId = accountId;
        replyPacket.accountId_2 = accountId;
        replyPacket.pcId = targetPcId;
        strncpy (replyPacket.familyName, familyName, sizeof (replyPacket.familyName));
        strncpy (replyPacket.familyName_2, familyName, sizeof (replyPacket.familyName_2));
        strncpy (replyPacket.commanderName, commanderName, sizeof (replyPacket.commanderName));
    }
}

void
ZoneBuilder_normalUnk6 (
    char *commanderName,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint32_t unk1;
        uint32_t unk2;
        uint32_t unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint8_t  unk6[16*3+8];
        uint8_t  unk7;
        char commanderName [COMMANDER_NAME_SIZE];
        uint8_t unk8[80];
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF E9 00 C9 00 00 00 85 07 00 00 | 0...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  19 51 00 00 43 22 00 00 1C 08 00 00 FF FF FF FF | .Q..C...........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 01 43 42 54 32 00 00 00 | .........CBT2...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 20 04 00 00 00 00 | .......... .....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 02 33 00 00 00 00 00 00                      | ..3......\n"
            , &replyPacket, &memSize
        );

        strncpy (replyPacket.commanderName, commanderName, sizeof (replyPacket.commanderName));
    }
}

void
ZoneBuilder_faction (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  6B 0D FF FF FF FF 48 01 00 00 CE 18 27 00       | k.....H.....'."
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_enterMonster (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  BF 0B FF FF FF FF 84 00 48 01 00 00 00 40 23 C4 | ........H....@#.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  A7 69 82 43 00 40 6E C4 46 0B 51 32 00 00 80 BF | .i.C.@n.F.Q2....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  02 00 21 00 00 00 21 00 00 00 00 00 00 00 00 00 | ..!...!.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  30 4E 00 00 58 02 00 00 21 00 00 00 00 00 69 6F | 0N..X...!.....io\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  01 00 00 00 00 00 80 3F 00 00 00 00 08 00 00 00 | .......?........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  28 00 00 00 06 00 0A 00 EA B2 BD EA B3 84 EB B3 | (...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  91 00 01 00 00 11 00 53 49 41 55 5F 46 52 4F 4E | .......SIAU_FRON\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  5F 4E 50 43 5F 30 31 00 01 00 00 01 00 00 07 00 | _NPC_01.........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 A0 42                                     | ...B\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_skillAdd (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  32 0C FF FF FF FF A2 00 00 00 00 00 00 00 00 00 | 2...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 03 00 00 00 78 00 | ..............x.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  6C F5 00 00 00 00 00 00 1C F5 55 01 00 00 80 3F | l.........U....?\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  48 01 00 00 96 43 46 01 00 00 00 00 4C 01 00 00 | H....CF.....L...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  C8 42 4E 01 00 00 0C 42 4F 01 00 00 60 41 76 01 | .BN....BO...`Av.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 62 01 00 00 0C 42 75 01 00 00 00 00 | ....b....Bu.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  71 01 00 00 00 00 70 01 00 00 80 3F 9B 01 00 00 | q.....p....?....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 9D 01 00 00 00 00 A8 01 00 00 40 40 C6 01 | ............@@..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 C7 01 00 00 80 3F C8 01 00 00 00 00 | .........?......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  DF 18 00 00 00 00 D4 01 00 00 A0 42 D2 01 00 00 | ...........B....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00                                           | ..\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_loginTime (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  C0 0C FF FF FF FF 30 39 FF 17 83 7F D0 01       | ......09......\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_stamina (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  A4 0C FF FF FF FF A8 61 00 00                   | .......a..\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_buffList (
    uint32_t targetPcId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t pcId;
        uint8_t  unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  E2 0B FF FF FF FF 0D 00 5A 73 01 00 00          | ........Zs...\n"
            , &replyPacket, &memSize
        );

        replyPacket.pcId = targetPcId;
    }
}

void
ZoneBuilder_enterPc (
    CommanderInfo *commander,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId; // 5A730100
        PositionXYZ position; // 00001DC4, 00008243, 002080C4
        float unk7; // 0000803F
        uint32_t unk8; // 00000000
        uint32_t unk9; // 00270406
        uint32_t unk10; // 0000749F
        uint8_t unk11; // 01
        uint16_t unk12; // 0000
        float moveSpeed; // 0000F841
        uint32_t unk13; // 00000000
        uint16_t currentHP; // 7700
        uint16_t maxHP; // 7700
        uint16_t currentSP; // 6900
        uint16_t maxSP; // 6900
        uint32_t curUnk; // A8610000
        uint32_t maxUnk; // A8610000
        uint8_t unk14; // 00
        uint16_t unk15; // 0000
        uint32_t unk16; // FFFFFFFF
        uint32_t unk17; // FC7C0000
        uint8_t unk18; // 00
        char commanderName [COMMANDER_NAME_SIZE+1];
        char familyName [COMMANDER_FAMILY_NAME_SIZE+1];
        uint16_t unk2;
        uint32_t unk3;
        uint64_t accountId;
        uint16_t classId;
        uint16_t unk4;
        uint16_t jobId;
        uint8_t gender;
        uint8_t unk5;
        uint8_t unk6[88];
    } replyPacket;
    #pragma pack(pop)

    /*
        5A730100 00001DC4 00008243 002080C4 0000803F 00000000 00270406 0000749F 01 0000 0000F841 00000000 7700 7700 6900 6900 A8610000 A8610000 00 0000 FFFFFFFF FC7C0000 00 4342543200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 EAB5ACEC9B90EC9E9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 0000 9000000 D1A8014400000000 1527 0000 0400 0200 01 00 0000 02000000 02000000 04000000 9D1A0800 06000000 07000000 10270000 F82A0000 8D110300 7C969800 04000000 09000000 09000000 04000000 8DF30700 09000000 09000000 09000000 09000000 0A000000 33000000
        046C0100 5057E4C4 74768243 DB7832C2 1D56E733 FFFF7FBF 0223E605 0000749F 01 0000 0000F841 F6285C3F 6D00 6D00 5100 5100 935E0000 A8610000 01 0000 FFFFFFFF 36690000 00 EC958CED858CEC96B4EC9794EC8B9CEC8AA40000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 5465616D53706F740000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 0000 0000000 2A14076E00000000 1327 A100 0300 0200 01 00 0000 02000000 02000000 04000000 9D1A0800 06000000 07000000 10270000 F82A0000 4D750200 7C969800 04000000 09000000 09000000 04000000 8DF30700 09000000 09000000 09000000 09000000 0A000000 0B000000
        956A0100 A0A5B9C4 74768243 4BC90FC3 AD1802BF 957A5C3F 02289905 0000A29F 01 0000 0000F841 00000000 4E00 4E00 9A00 9A00 A8610000 A8610000 00 0000 FFFFFFFF 48820000 00 EC9584EC95BC0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 EC95BCEC95BC00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 0000 0000000 084C025A00000000 1627 A100 0200 0100 01 00 0000 02000000 02000000 04000000 9D1A0800 06000000 07000000 10270000 F82A0000 2D270200 7C969800 04000000 09000000 09000000 04000000 8DF30700 09000000 09000000 09000000 09000000 0A000000 24000000
        5A730100 0000BAC4 00008C43 00001EC3 FFFF7F3F 1D56E733 00D70406 0000749F 01 0000 0000F841 00000000 8300 8300 7200 7200 685E0000 A8610000 00 0000 FFFFFFFF 00000000 00 4342543200000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 EAB5ACEC9B90EC9E9000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 0000 9000000 D1A8014400000000 1527 0000 0400 0200 01 00 0000 02000000 02000000 04000000 9D1A0800 06000000 07000000 10270000 F82A0000 8D110300 7C969800 04000000 09000000 09000000 04000000 8DF30700 09000000 09000000 09000000 09000000 0A000000 33000000
        956A0100 A0A5B9C4 74768243 4BC90FC3 AD1802BF 957A5C3F 02269905 0000A29F 01 0000 0000F841 00000000 4E00 4E00 9A00 9A00 A8610000 A8610000 00 0000 FFFFFFFF 06640000 00 EC9584EC95BC0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 EC95BCEC95BC00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000 0000 0000000 084C025A00000000 1627 A100 0200 0100 01 00 0000 02000000 02000000 04000000 9D1A0800 06000000 07000000 10270000 F82A0000 2D270200 7C969800 04000000 09000000 09000000 04000000 8DF30700 09000000 09000000 09000000 09000000 0A000000 24000000
    */

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  BE 0B FF FF FF FF 5A 73 01 00 00 00 1D C4 00 00 | ......Zs........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  82 43 00 20 80 C4 00 00 80 3F 00 00 00 00 00 27 | .C. .....?.....'\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  04 06 00 00 74 9F 01 00 00 00 00 F8 41 00 00 00 | ....t.......A...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 77 00 77 00 69 00 69 00 A8 61 00 00 A8 61 00 | .w.w.i.i..a...a.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 FF FF FF FF FC 7C 00 00 00 43 42 54 | .........|...CBT\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  32 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | 2...............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 EA B5 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  AC EC 9B 90 EC 9E 90 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 09 00 00 00 D1 A8 01 44 00 00 00 00 15 27 00 | ........D.....'.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 04 00 02 00 01 00 00 00 02 00 00 00 02 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 04 00 00 00 9D 1A 08 00 06 00 00 00 07 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 10 27 00 00 F8 2A 00 00 8D 11 03 00 7C 96 98 | ..'...*......|..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 04 00 00 00 09 00 00 00 09 00 00 00 04 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 8D F3 07 00 09 00 00 00 09 00 00 00 09 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 09 00 00 00 0A 00 00 00 33 00 00 00          | .........3...\n"
            , &replyPacket, &memSize
        );

        replyPacket.header.type = ZC_ENTER_PC;
        strncpy (replyPacket.familyName, commander->familyName, sizeof (replyPacket.familyName));
        strncpy (replyPacket.commanderName, commander->commanderName, sizeof (replyPacket.commanderName));
        replyPacket.pcId = commander->pcId;
        replyPacket.position.x = commander->cPos.x;
        replyPacket.position.y = 260.0f;
        replyPacket.position.z = commander->cPos.z;
        replyPacket.unk7 = 1.0;
        replyPacket.accountId = commander->accountId;
        replyPacket.classId = commander->classId;
        replyPacket.jobId = commander->jobId;
        replyPacket.gender = commander->gender;
        replyPacket.moveSpeed = 31.0;
        replyPacket.currentHP = commander->currentHP;
        replyPacket.maxHP = commander->maxHP;
        replyPacket.currentSP = commander->currentSP;
        replyPacket.maxSP = commander->maxSP;
    }
}

void
ZoneBuilder_objectProperty (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:22][           ToSClient:                     dbgBuffer]  63 0C FF FF FF FF 7A 02 04 06 00 00 74 9F 01 00 | c.....z.....t...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  6C 03 00 00 80 3F AB 17 00 58 9D 45 AC 17 00 00 | l....?...X.E....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  96 43 20 04 00 50 C3 46 62 04 00 40 9C 46 71 03 | .C ..P.Fb..@.Fq.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 F8 41 48 17 00 00 40 40 49 17 00 00 00 41 | ...AH...@@I....A\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  99 04 00 00 80 40 9A 04 00 00 E0 40 9C 04 00 00 | .....@.....@....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  C0 40 E8 03 00 00 80 3F 84 03 00 00 00 00 95 04 | .@.....?........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 80 3F 9D 04 00 00 E0 40 4B 11 00 00 80 40 | ...?.....@K....@\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  4A 11 00 00 80 40 96 04 00 00 40 40 9F 04 00 00 | J....@....@@....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  40 40 AA 04 00 00 00 00 90 04 00 00 00 00 A7 04 | @@..............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 D2 42 A8 04 00 00 EE 42 BF 03 00 00 00 00 | ...B.....B......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  BE 03 00 00 00 00 B9 03 00 00 00 00 B8 03 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 B6 03 00 00 00 00 A5 04 00 00 00 41 98 04 | .............A..\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 80 40 79 04 00 00 00 00 9E 04 00 00 C0 40 | ...@y..........@\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  83 03 00 00 00 00 92 04 00 00 80 40 93 04 00 00 | ...........@....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 A6 04 00 00 E0 40 72 17 00 00 80 40 69 11 | .......@r....@i.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 B3 04 00 00 00 00 47 15 00 00 00 00 | ..........G.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  AC 04 00 00 00 00 AD 04 00 00 00 00 AE 04 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 56 04 00 00 00 00 53 04 00 00 00 00 A1 03 | ..V.....S.......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 D5 1E 00 00 00 00 8D 17 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  DD 1E 00 00 00 00 97 03 00 00 00 00 C3 03 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 A6 18 00 00 00 00 C1 03 00 00 00 00 C4 03 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 9B 03 00 00 00 00 98 03 00 00 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  96 03 00 00 00 00 A0 03 00 00 00 00 9F 03 00 00 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 9D 03 00 00 00 00 A1 18 00 00 00 00 A0 18 | ................\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 B2 04 00 00 00 00 7D 03 00 00 00 00 | ..........}.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  7E 03 00 00 80 3F 7F 03 08 00 43 68 61 72 34 5F | ~....?....Char4_\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  31 00 94 04 00 00 FA 43 CB 03 00 00 00 00 9A 03 | 1......C........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 09 04 00 00 00 00 52 17 00 00 00 00 | ..........R.....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  29 04 00 00 00 00 08 04 00 00 00 00 51 17 00 00 | )...........Q...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 26 04 00 00 00 00 FC 03 00 00 00 00 F3 03 | ..&.............\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 F1 03 00 40 1C 45 EF 03 00 00 00 00 | .......@.E......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  EE 03 00 00 7A 43 ED 03 00 40 9C 45 EC 03 00 00 | ....zC...@.E....\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  C8 42 E9 03 00 00 7A 43 EA 03 00 00 00 00 E2 03 | .B....zC........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 00 04 00 00 80 3F FE 03 00 00 00 00 | .........?......\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  E5 03 00 00 AF 43 C7 03 00 00 00 00 C9 03 00 00 | .....C..........\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 40 C6 03 45 76 F4 3F C5 03 00 00 80 3F 73 17 | .@..Ev.?.....?s.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 74 17 00 00 C0 40 70 17 00 00 C0 40 | ....t....@p....@\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  71 17 00 00 40 41 6E 17 00 00 30 41 6D 04 00 00 | q...@An...0Am...\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 68 04 00 00 20 42 66 04 00 00 02 43 65 04 | ..h... Bf....Ce.\n"
            "[11:10:22][           ToSClient:                     dbgBuffer]  00 00 00 00 92 17 00 00 7A 44                   | ........zD\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_addonMsg (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:23][           ToSClient:                     dbgBuffer]  4F 0C FF FF FF FF 1F 00 11 00 00 00 00 00 4B 45 | O.............KE\n"
            "[11:10:23][           ToSClient:                     dbgBuffer]  59 42 4F 41 52 44 5F 54 55 54 4F 52 49 41 4C    | YBOARD_TUTORIAL"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_normalUnk5 (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:20][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 1D 00 15 01 00 00 8D FA 0D 00 | 0...............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  63 61 63 60 28 99 CF C8 C0 C8 00 06 00          | cac`(........\n"
          , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_normalUnk4 (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:20][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 2C 00 DF 00 00 00 04 06 00 00 | 0.....,.........\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  74 9F 01 00 01 00 00 00 01 00 00 00 04 00 01 00 | t...............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 80 56 9E 10 83 7F D0 01             | .....V......\n"
          , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_normalUnk3 (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:20][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 10 00 D2 00 00 00 00 00 00 00 | 0...............\n"
          , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_normalUnk2 (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:20][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF 18 00 C4 00 00 00 04 06 00 00 | 0...............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  74 9F 01 00 00 00 00 00                         | t.......\n"
          , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_normalUnk1 (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:20][           ToSClient:                     dbgBuffer]  30 0D FF FF FF FF BD 00 92 00 00 00 00 01 00 00 | 0...............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 61 EA 00 00 E5 5E 00 00 79 9F 01 00 EC 28 00 | .a....^..y....(.\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 D1 91 01 00 D7 37 00 00 0A 00 EC 9A B0 EC AD | ......7.........\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  88 EC AD 88 00 00 00 00 00 7E 00 40 17 00 00 E0 | .........~.@....\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  40 4F 11 00 00 D8 41 4E 11 00 00 D8 41 A5 00 00 | @O....AN....A...\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 E0 40 B0 00 00 C0 6B 44 A1 00 00 00 D0 41 F4 | ..@....kD.....A.\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  1D 00 00 00 00 05 1C 00 00 00 00 A3 16 00 00 D8 | ................\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  41 E7 1B 00 00 20 41 64 1E 00 00 00 00 E5 1B 00 | A.... Ad........\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  80 9A 43 EC 1B 00 00 50 41 EB 1B 00 00 00 40 03 | ..C....PA.....@.\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  1C 00 00 C0 40 0A 1C 00 00 E0 40 6A 17 00 00 A0 | ....@.....@j....\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  40 65 17 00 00 A0 40 62 00 00 00 F0 43 60 00 00 | @e....@b....C`..\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 02 44 90 17 00 00 7A 44 00 00 00 00          | ..D....zD....\n"
          , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_startGame (
    float timeMultiplier,
    float serverAppTimeOffset,
    float globalAppTimeOffset,
    double serverDateTime,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        float timeMultiplier;
        float serverAppTimeOffset;
        float globalAppTimeOffset;
        double serverDateTime;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_START_GAME;
        replyPacket.timeMultiplier = timeMultiplier;
        replyPacket.serverAppTimeOffset = serverAppTimeOffset;
        replyPacket.globalAppTimeOffset = globalAppTimeOffset;
        replyPacket.serverDateTime = serverDateTime;
    }
}

void
ZoneBuilder_quickSlotList (
    zmsg_t *replyMsg
) {
    /*
    #pragma pack(push, 1)
    typedef struct {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t zlibData; // 0 or zlib deflated data
    } ZcQuickSlotListPacket;
    #pragma pack(pop)

    ZcQuickSlotListPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.variableSizeHeader.serverHeader.type = ZC_QUICK_SLOT_LIST;
    replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);
    replyPacket.zlibData = 0;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
    */

    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:36:22][           ToSClient:                     dbgBuffer]  30 0C FF FF FF FF 59 00 4D 00 00 00 63 60 72 9C | 0.....Y.M...c`r.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  C3 C0 E0 72 85 81 61 F1 7C 46 06 26 27 20 27 FF | ...r..a.|F.&' '.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  26 94 E3 0C E4 14 C0 38 2E 40 4E 21 94 C3 40 55 | &......8.@N!..@U\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  C0 1C 7D 84 93 E1 D3 65 88 A5 CC 71 40 CE 67 18 | ..}....e...q@.g.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  27 11 C8 F9 02 E5 50 D5 CE 51 C3 06 7B 08 30 22 | '.....P..Q..{.0.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  3B 90 09 99 C3 8C CC 01 00                      | ;........\n"
            , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_cooldownList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint32_t unk3;
        uint32_t unk4;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_COOLDOWN_LIST;
        replyPacket.unk1 = 0x14;
        replyPacket.unk2 = 0x604;
        replyPacket.unk3 = 0x019F74;
        replyPacket.unk4 = 0;
    }
}

void
ZoneBuilder_abilityList (
    uint32_t targetPcId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t pcId;
        uint16_t numberEntries;
        uint8_t unk [48];
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (                                                                              //   [--PcId---]
            "[11:36:22][           ToSClient:                     dbgBuffer]  33 0C FF FF FF FF 3E 00 36 F8 01 00 03 00 8D FA | 3.....>.6.......\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  1C 00 7B 7F 99 81 61 F1 7C 46 06 41 75 06 20 F8 | ..{...a.|F.Au. .\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  CF F0 01 CA 17 87 F2 3F 42 F9 92 50 3E 00 F1 D3 | .......?B..P>...\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  00 00 A3 9F 01 00 19 27 00 00 00 00 FF 00       | .......'......\n"
            , &replyPacket, &memSize
        );

        replyPacket.variableSizeHeader.serverHeader.type = ZC_ABILITY_LIST;
        replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);
        replyPacket.pcId = targetPcId;
    }
}

void
ZoneBuilder_skillList (
    uint32_t targetPcId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t pcId;
        uint16_t skillCount;
        uint8_t unk [318];
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize = sizeof (replyPacket);
        dumpToMem (                                                                              //   [--PcId---]
            "[11:45:10][           ToSClient:                     dbgBuffer]  31 0C FF FF FF FF 4C 01 36 F8 01 00 07 00 8D FA | 1.....L.6.......\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  3A 01 63 60 80 00 11 20 55 C1 A0 F4 0A C4 DB B4 | :.c`... U.......\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  D3 DB 3C 94 91 81 A1 C1 DE 03 48 32 30 B8 81 49 | ..<.......H20..I\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  1F 20 79 C2 C9 0F 48 3E 70 F4 07 92 09 8E 65 60 | . y...H>p.....e`\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  F1 24 20 F9 C1 B1 14 CC 2E 04 93 05 40 B2 C1 7E | .$ .........@..~\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  36 98 3D 17 4C AE 00 92 0E 0E C7 C0 EC E3 40 B2 | 6.=.L.........@.\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  C1 FE 04 98 7D 5F 02 64 FE 15 20 FB 84 D3 25 B0 | ....}_.d.. ...%.\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  08 88 0F 02 CC 40 5C C1 C0 B4 42 07 28 9A A7 6F | .....@....B.(..o\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  D2 8F 70 CF 34 67 74 F7 F0 38 A1 BB 87 C7 89 52 | ..p.4gt..8.....R\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  F7 2C 40 73 4F 0A D8 3D 0C 09 40 8A 41 AD FE 9F | .,@sO..=..@.A...\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  13 C2 3D 98 E1 A3 00 76 CF 01 A4 F0 39 41 B5 F0 | ..=....v....9A..\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  29 BC C9 C0 B0 78 3E 23 83 CB 1C 50 F8 2C 0F 04 | )....x>#...P.,..\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  B9 C7 28 7E 97 1D DC 3D 1F 82 DC D0 C3 87 81 01 | ..(~...=........\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  11 3E 0D F6 A0 F8 62 60 C0 1E 3E 0A 8E 88 F8 6A | .>....b`..>....j\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  00 C7 57 95 33 F6 F8 62 60 80 C4 57 01 D4 3D CE | ..W.3..b`..W..=.\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  60 F7 88 79 83 CC BE 5E CF F0 09 EE 9E 1B B6 EE | `..y...^........\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  D8 DD A3 00 0E 1F AA B9 C7 01 E2 9E 7C A8 7B 9C | ............|.{.\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  C0 EE 61 60 10 92 02 B9 A8 C1 1E E4 1E 07 07 60 | ..a`...........`\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  7A B6 38 4E 41 F8 AC 20 3A 7C 20 E9 D9 C1 D1 E5 | z.8NA.. :| .....\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  0A 24 BE 1C A1 EE 01 B9 06 C5 3D 06 7D A4 B9 C7 | .$........=.}...\n"
            "[11:45:10][           ToSClient:                     dbgBuffer]  61 8E 2B 22 7F CD 20 D1 3D 3C 4E 00             | a.+... .=<N.\n"
          , &replyPacket, &memSize
        );

        /* Uncompressed :
            00000000 00000000 14000000 780022EA 00000000 B2B94B37 55010000 803F4801 00000000 46010000 00004C01 0000C842 4E010000 E0414F01 00006041 76010000 00006201 0000F041 75010000 00007101 00000000 70010000 803F9B01 00000000 9D010000 0000A801 00004040 C6010000 0000C701 0000803F C8010000 0000DF18 00000000 D4010000 C842D201 00000000
            00000000 00000000 03000000 780002A8 2C010000 6E2F348F 55010000 803F4801 00009643 46010000 00004C01 0000C842 4E010000 0C424F01 00006041 76010000 00006201 00000C42 75010000 00007101 00000000 70010000 803F9B01 00000000 9D010000 0000A801 00004040 C6010000 0000C701 0000803F C8010000 0000DF18 00000000 D4010000 A042D201 00000000
            00000000 00000000 64000000 78000060 00000000 267FFE42 55010000 803F4801 00000000 46010000 00004C01 0000C842 4E010000 20424F01 0000C041 76010000 00006201 0000C841 75010000 00007101 00000000 70010000 803F9B01 00000000 9D010000 0000A801 00004040 C6010000 0000C701 0000803F C8010000 0000DF18 00000000 D4010000 C842D201 00000000
            71D90000 A39F0100 449C0000 7800A751 00000000 325FBA3E 55010000 803F4801 00F05246 46010000 00004C01 0000C842 4E010000 00004F01 00006041 76010000 803F6201 00000000 75010000 00007101 00000000 70010000 803F9B01 00002041 9D010000 0000A801 00008040 C6010000 7A43C701 0000803F C8010000 0000DF18 00000000 D4010000 0000D201 00000000
        */
        replyPacket.pcId = targetPcId;
    }
}

void
ZoneBuilder_itemEquipList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:10:20][           ToSClient:                     dbgBuffer]  2B 0C FF FF FF FF 12 02 02 00 00 00 00 00 00 00 | +...............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 01 00 00 03 00 00 | ................\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  02 00 00 00 00 00 00 86 00 00 00 00 00 00 00 00 | ................\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  01 4C 60 33 C1 52 0C 0C 04 00 00 00 00 00 69 0C | .L`3.R........i.\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 02 0F 18 12 D2 DC 99 19 | ................\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  9D 1A 08 00 0C 00 C5 4E 06 06 00 00 74 9F 01 00 | .......N....t...\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  03 9B E2 12 A0 50 03 00 81 10 00 00 00 40 A1 17 | .....P.......@..\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 06 00 00 00 00 00 54 00 00 00 00 00 | ..........T.....\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 04 D5 18 00 E8 1E 56 00 07 00 00 00 | ..........V.....\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 05 26 1E 09 | .............&..\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  04 F3 3B 05 10 27 00 00 00 00 D2 09 00 00 00 00 | ..;..'..........\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 06 D4 56 00 3C 78 06 23 F8 2A 00 00 | ......V.<x.#.*..\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 06 23 00 00 00 00 00 00 00 00 07 00 00 00 | ...#............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  58 D5 18 00 8D 11 03 00 12 00 3B 05 07 06 00 00 | X.........;.....\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  74 9F 01 00 08 F3 3B 05 20 88 A2 1C 8C 10 00 00 | t.....;. .......\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  A0 40 8D 10 00 00 80 40 A1 17 00 00 00 00 7C 96 | .@.....@......|.\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  98 00 00 00 98 D5 00 00 00 00 00 00 00 00 09 05 | ................\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  90 88 A2 1C 01 00 04 00 00 00 00 00 04 F3 00 00 | ................\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 0A 00 04 F3 3B 05 20 88 09 00 | ..........;. ...\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 20 D6 00 00 00 00 00 00 00 00 0B 05 | .... ...........\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  20 88 A2 1C 00 00 09 00 00 00 00 00 2C AC 00 00 |  ...........,...\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 0C 1B 84 F3 3B 05 F0 E6 04 00 | ..........;.....\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 00 58 96 00 00 00 00 00 00 00 00 0D 00 | ....X...........\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  40 B5 3B 0C FC D5 8D F3 07 00 0C 00 84 F3 08 06 | @.;.............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 74 9F 01 00 0E 00 20 88 A2 1C 88 23 81 10 | ..t..... ....#..\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 00 40 A1 17 00 00 00 00 09 00 00 00 00 00 | ...@............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 35 00 00 00 00 00 00 00 00 0F 05 01 00 00 00 | .5..............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  A7 EB 09 00 00 00 00 00 30 7D 00 00 00 00 00 00 | ........0}......\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 10 00 04 F3 3B 05 48 D6 09 00 00 00 00 00 | ......;.H.......\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  04 F3 00 00 00 00 00 00 00 00 11 00 84 D6 18 00 | ................\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  DA 70 09 00 00 00 00 00 D4 D6 00 00 00 00 00 00 | .p..............\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  00 00 12 00 04 F3 3B 05 01 00 0A 00 00 00 00 00 | ......;.........\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  8C D6 00 00 00 00 00 00 00 00 13 00 70 D7 18 00 | ............p...\n"
            "[11:10:20][           ToSClient:                     dbgBuffer]  60 05                                           | `.\n"
          , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_startInfo (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint16_t unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint16_t unk6;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_START_INFO;
        replyPacket.unk1 = 0x18;
        replyPacket.unk2 = 1;
        replyPacket.unk3 = 4;
        replyPacket.unk4 = 0x1A12;
        replyPacket.unk5 = 0;
        replyPacket.unk6 = 1;
    }
}

void
ZoneBuilder_setPos (
    uint32_t targetPcId,
    PositionXYZ *position,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        PositionXYZ position;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_SET_POS;
        replyPacket.pcId = targetPcId;
        memcpy (&replyPacket.position, position, sizeof (PositionXYZ));
    }
}

void
ZoneBuilder_guestPageMap (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint8_t  unk3;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_GUESTPAGE_MAP;
        replyPacket.unk1 = 0xD;
        replyPacket.unk2 = 1;
        replyPacket.unk3 = 0;
    }
}

void
ZoneBuilder_myPageMap (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint8_t  unk3;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_MYPAGE_MAP;
        replyPacket.unk1 = 0xD;
        replyPacket.unk2 = 1;
        replyPacket.unk3 = 0;
    }
}

void
ZoneBuilder_helpList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;

        uint32_t nbEntries;

        uint32_t unk3;
        uint8_t  unk4;

        uint32_t unk5;
        uint8_t  unk6;

        uint32_t unk7;
        uint8_t  unk8;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_HELP_LIST;
        replyPacket.unk1 = 0x1B;
        replyPacket.nbEntries = 3;

        replyPacket.unk3 = 2;
        replyPacket.unk4 = 0;

        replyPacket.unk5 = 3;
        replyPacket.unk6 = 0;

        replyPacket.unk7 = 4;
        replyPacket.unk8 = 0;
    }
}

void
ZoneBuilder_npcStateList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_NPC_STATE_LIST;
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void
ZoneBuilder_uiInfoList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        uint32_t unk3;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_UI_INFO_LIST;
        replyPacket.unk1 = 0x10;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = 0;
    }
}

void
ZoneBuilder_chatMacroList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_CHAT_MACRO_LIST;
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void
ZoneBuilder_achievePointList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_ACHIEVE_POINT_LIST;
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void
ZoneBuilder_skillmapList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_SKILLMAP_LIST;
        replyPacket.unk1 = 0xC;
        replyPacket.unk2 = 0;
    }
}

void
ZoneBuilder_optionList (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint8_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_OPTION_LIST;
        replyPacket.unk1 = 9;
        replyPacket.unk2 = 0;
    }
}

void
ZoneBuilder_MyPCEnter (
    PositionXYZ *position,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        PositionXYZ position;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_MYPC_ENTER;
        memcpy (&replyPacket.position, position, sizeof (PositionXYZ));
    }
}

void
ZoneBuilder_moveSpeed (
    uint32_t targetPcId,
    float movementSpeed,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        float movementSpeed;
        uint32_t unk1;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_MOVE_SPEED;

        replyPacket.pcId = targetPcId;
        replyPacket.movementSpeed = movementSpeed;
        replyPacket.unk1 = 0;
    }
}

void
ZoneBuilder_jump (
    uint32_t targetPcId,
    float height,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        float height;
        uint32_t unk1;
        uint8_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = ZC_JUMP;
        replyPacket.pcId = targetPcId;
        replyPacket.height = height;
        replyPacket.unk1 = 0;
        replyPacket.unk2 = 1;
    }
}

void
ZoneBuilder_connect (
    uint8_t gameMode,
    uint8_t accountPrivileges,
    uint32_t targetPcId,
    CommanderInfo *commander,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint8_t gameMode; // 0 = NormalMode, 1 = SingleMode
        uint32_t unk1;
        uint8_t accountPrivileges;
        uint8_t unk2[3];
        uint8_t unk3[7];
        uint32_t pcId;
        uint32_t unk4;
        CommanderInfo commander;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.variableSizeHeader.serverHeader.type = ZC_CONNECT_OK;
        replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);

        replyPacket.gameMode = gameMode;
        replyPacket.unk1 = 1;
        replyPacket.accountPrivileges = accountPrivileges;
        replyPacket.pcId = targetPcId;

        // Copy the current commander information
        memcpy (&replyPacket.commander, commander, sizeof (CommanderInfo));
    }
}

void
ZoneBuilder_sessionObjects (
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:15:14][           ToSClient:                     dbgBuffer]  89 0C FF FF FF FF 18 01 0A 00 D1 BF 0B 00 20 E0 | .............. .\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  02 93 05 D4 00 00 A3 9F 01 00 00 00 00 00 06 00 | ................\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  32 83 C4 0F 00 00 80 3F A0 3A 00 00 3A D7 80 17 | 2......?.:..:...\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  FE D3 00 00 A3 9F 01 00 00 00 00 00 00 00 D9 6C | ...............l\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  60 EA 00 00 C5 19 20 49 FD D3 00 00 A3 9F 01 00 | `..... I........\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 01 00 00 77 01 00 AF 03 00 00 | .........w......\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  FC D3 00 00 A3 9F 01 00 00 00 00 00 00 00 00 00 | ................\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  03 00 00 00 03 00 00 00 FB D3 00 00 A3 9F 01 00 | ................\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 A0 86 01 00 61 01 00 00 | ............a...\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  FA D3 00 00 A3 9F 01 00 00 00 00 00 00 00 18 1F | ................\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  81 38 01 00 01 00 00 00 F9 D3 00 00 A3 9F 01 00 | .8..............\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 18 73 01 00 60 D5 18 00 | .........s..`...\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  F8 D3 00 00 A3 9F 01 00 00 00 00 00 00 00 55 00 | ..............U.\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  80 38 01 00 13 D1 56 00 F7 D3 00 00 A3 9F 01 00 | .8....V.........\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 51 05 90 5F 01 00 74 D5 18 00 | ......Q.._..t...\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  F6 D3 00 00 A3 9F 01 00 00 00 00 00 18 00 3B 05 | ..............;.\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  3A 0A 00 00 96 43 41 0A 00 00 96 43 52 0A 00 00 | :....CA....CR...\n"
            "[11:15:14][           ToSClient:                     dbgBuffer]  96 43 42 0A 00 00 96 43                         | .CB....C\n"
            , NULL, &memSize
        );

        replyPacket.variableSizeHeader.serverHeader.type = ZC_SESSION_OBJECTS;
        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_itemInventoryList (
    zmsg_t *replyMsg
) {
    /*
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint32_t unk2;
        ZlibHeader zlibHeader;
    } ZcItemInventoryList;
    #pragma pack(pop)

        Packet uncompressed : (this is only *one* packet once decompressed)

        unk1     unk2     unk3
        00D80900 06006172 BEE10000

        id_unk1  id_unk2  unk4     unk5 unk6 unk7     id_unk4  unk8     id_unk5  unk9     unk10
        A39F0100 01000000 00000000 439C 0000 6C756500 03100000 000050C4 09000600 382EFEDC 0000
        A39F0100 07000000 00000000 3975 0000 352D3034 03100000 00001EC4 09000600 4279DEDC 0000
        A39F0100 02000000 00000000 3575 0000 01000800 03100000 000019C4 09000600 0008CBDB 0000
        A39F0100 05000000 00000000 3875 0000 000F0043 03100060 6A46FED7 09000600 6C00ECDA 0000
        A39F0100 01000000 00000000 429C 0000 02003100 03100000 000067C4 09000600 EC82D4D8 0000
        A39F0100 03000000 00000000 3775 0000 68616E67 03100060 EA461BC4 09000600 003442D5 0000
        A39F0100 05000000 00000000 3675 0000 4F010000 03100060 EA46FDD7 09000600 0000CBD4 0000
        A39F0100 03000000 00000000 419C 0000 0000B041 03100000 00001AC4 09000600 C601B8D4 0000
        A39F0100 06000000 00000000 3475 0000 0000D401 03100000 0000ABBB 0D000600 0000B7D4 0000
        A39F0100 30030000 00000000 F924 0100 55010000 03100000 00001CEB 09000600 0000F5D3 0000
        A39F0100 01000000 00000000 61EA 0000 0000803F 0310007C 924961C4 09000600 7101F4D3 0000
        A39F0100 01000000 00000000 3375 0000 0000A801 03100060 EA465EC4 09000600 0000F3D3 0000
        A39F0100 07000000 00000000 3275 0000 D2010000 03100060 EA465BC4 09000600 0000F2D3 0000
        A39F0100 07000000 00000000 3175 0000 00A08C47 03100060 EA468D11 03001800 4E010706 0000

        749F0100 01000000 00000000 8913 0000 00007501 FA0F00F0 53458C10 0000A040 8D100000 8040A117 00000000
    */

    /*
    uint8_t *stackBuffer = alloca (sizeof (*stackBuffer) * outPacketSize);
    ZcItemInventoryList *replyPacket = (ZoneTrafficsPacket *) stackBuffer;

    // Build the static part of the packet
    replyPacket->type = ZC_ITEM_INVENTORY_LIST;

    // Construct the packet stream
    PacketStream stream;
    PacketStream_init (&stream, (uint8_t *) replyPacket);
    // Go to the end of the static size
    PacketStream_addOffset (&stream, sizeof (ZcItemInventoryList));
    */

    #pragma pack(push, 1)
    struct {
        // Not yet implemented
    } replyPacket;
    (void) replyPacket;
    #pragma pack(pop)

    // BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        size_t memSize;
        void *memory = dumpToMem (
            "[11:36:22][           ToSClient:                     dbgBuffer]  29 0C FF FF FF FF 60 01 0F 00 00 00 8D FA 50 01 | ).....`.......P.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  63 B8 C1 C9 C0 C6 90 58 B4 EF 21 03 C3 E2 F9 8C | c......X..!.....\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  0C 8C 0C 10 E0 3C 87 81 21 A7 34 95 81 59 00 C4 | .....<..!.4..Y..\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  0F 38 02 52 65 A1 F7 EF 0E 44 15 3B 44 11 83 65 | .8.Re....D.;D..e\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  29 03 83 A9 AE 81 09 44 95 1C 58 95 53 E5 3D A8 | )......D..X.S.=.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  2A 26 A8 2A 53 A0 2A 46 06 0E A8 59 92 60 55 0C | *&.*S.*F...Y.`U.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  1C A7 6F 43 CC 62 85 AA B2 00 AA 62 E0 67 70 06 | ..oC.b.....b.gp.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  9A 95 90 E5 F6 EF 3A C8 C6 1C 86 37 B7 50 DD E5 | ......:....7.P..\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  04 74 17 13 83 21 D4 AC 74 B0 59 6F 9A AE DC 80 | .t...!..t.Yo....\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  A8 62 86 9A 65 0E 34 2B 23 31 2F 1D 64 D6 2B 37 | .b..e.4+#1/.d.+7\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  69 88 8D 26 4E 57 51 6D 34 03 AA F2 07 7A 18 A2 | i..&NWQm4....z..\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  EA 2F D8 46 06 86 D3 57 50 CD 72 04 DA C8 C0 B0 | ./.F...WP.r.....\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  C1 11 E2 47 29 B0 59 C7 18 77 40 55 B1 41 6D 34 | ...G).Y..w@U.Am4\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  01 B9 9E E1 0A 23 44 D5 EA DD BC 40 D7 33 30 6C | .....#D....@.30l\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  87 AA 32 80 3A EC A7 0A 23 43 28 C4 46 A0 AC CC | ..2.:...#C(.F...\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  6B 90 1F 19 18 BE 5E 46 F5 63 E2 2B A0 20 43 83 | k.....^F.c.+. C.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  3D D0 AC 9A 49 9E 89 60 1B 0B 19 BF A0 A9 32 06 | =...I..`......2.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  DB B8 02 64 23 D0 8F 71 10 3F 32 7C 86 AA 82 C5 | ...d#..q.?2|....\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  90 11 50 D5 25 B8 1F A3 A1 AA 3E A1 A9 32 04 99 | ..P.%.....>..2..\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  B5 A0 C7 1D 62 56 AF 20 33 83 04 83 1F 23 3B D0 | ....bV. 3....#;.\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  71 25 48 69 A2 53 18 E4 AE 52 C6 5F FC 0C 1F 82 | q%Hi.S...R._....\n"
            "[11:36:22][           ToSClient:                     dbgBuffer]  5D 7B 04 80 5A 1C 7A 81 64 83 C3 42 71 90 0C 00 | ]{..Z.z.d..Bq...\n"
          , NULL, &memSize
        );

        zmsg_add (replyMsg, zframe_new (memory, memSize));
    }
}

void
ZoneBuilder_moveDir (
    uint32_t targetPcId,
    PositionXYZ *position,
    PositionXZ *direction,
    float timestamp,
    zmsg_t *replyMsg
) {
    /*
         pcId       posX     posY     posZ     dirX     dirZ     u1 u2 unk3     time
         4E6E0100   4E1D36C4 E656A143 9DD40444 F304353F F304353F 01 00 00F84101 5341B543
         4E6E0100   402434C4 E656A143 ABCD0644 F304353F F304353F 01 00 00F84101 8D54B543
         4E6E0100   7AE81AC4 4108A543 92AB5C44 1D566733 FFFF7FBF 01 00 00F84101 8EED5A44
         4E6E0100   7AE81AC4 89C9A343 FA915944 1D566733 FFFF7FBF 01 00 00F84101 38F85A44
    */

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        PositionXYZ position;
        PositionXZ direction;
        uint8_t unk1;
        uint8_t unk2;
        uint32_t unk3;
        float timestamp;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        PacketServer_header (&replyPacket.header, ZC_MOVE_DIR);
        replyPacket.pcId = targetPcId;
        memcpy (&replyPacket.position, position, sizeof (PositionXYZ));
        memcpy (&replyPacket.direction, direction, sizeof (PositionXZ));
        replyPacket.unk1 = 1;
        replyPacket.unk2 = 0;
        replyPacket.unk3 = SWAP_UINT32 (0x00F04101);
        replyPacket.timestamp = timestamp;
    }
}

void
ZoneBuilder_pcMoveStop (
    uint32_t targetPcId,
    PositionXYZ *position,
    PositionXZ *direction,
    float timestamp,
    zmsg_t *replyMsg
) {
    /*
        header        pcId     posX     posY     posZ     u1 dirX     dirZ     time
        590D 3C395F00 32750100 00001DC4 A7698243 002080C4 01 0000803F 00000000 498AC742
        590D 6B3A5F00 5A730100 5FBA8BC4 74768243 9B4C03C4 01 F30435BF F304353F 91698943
        590D 3F435F00 5A730100 67408EC4 74768243 3FE20BC4 01 FEFF7FBF 1C56E7B3 F8439043
        590D E4435F00 5A730100 67408EC4 74768243 17060AC4 01 00000000 FEFF7F3F 56B59043
        590D 66445F00 5A730100 0BF18DC4 74768243 17060AC4 01 FEFF7F3F 1C56E733 BD179143
    */

    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t pcId;
        PositionXYZ position;
        uint8_t unk1;
        PositionXZ direction;
        float timestamp;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        PacketServer_header (&replyPacket.header, ZC_PC_MOVE_STOP);
        replyPacket.pcId = targetPcId;
        memcpy (&replyPacket.position, position, sizeof (PositionXYZ));
        memcpy (&replyPacket.direction, direction, sizeof (PositionXZ));
        replyPacket.unk1 = 1;
        replyPacket.timestamp = timestamp;
    }

}
