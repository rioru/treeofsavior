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
#include "BarrackBuilder.h"
#include "Common/Server/Worker.h"
#include "Common/Packet/Packet.h"
#include "Common/Packet/PacketStream.h"
#include "Common/Packet/PacketType.h"


// ------ Structure declaration -------


// ------ Static declaration -------


// ------ Extern function implementation -------

void
BarrackBuilder_loginOk (
    uint64_t accountId,
    uint8_t *accountLogin,
    GameSessionPrivileges accountPrivileges,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t accountId;
        uint8_t accountLogin [GAME_SESSION_ACCOUNT_LOGIN_MAXSIZE];
        uint32_t accountPrivileges;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = BC_LOGINOK;
        replyPacket.accountId = accountId;
        replyPacket.accountPrivileges = accountPrivileges;
        strncpy (replyPacket.accountLogin, accountLogin, sizeof (replyPacket.accountLogin));
    }
}

void
BarrackBuilder_startGameOk (
    uint32_t zoneServerId,
    uint8_t *zoneServerIp,
    uint32_t zoneServerPort,
    uint32_t mapId,
    uint8_t commanderListId,
    uint32_t spriteId,
    uint32_t spriteIdRelated,
    uint8_t isSingleMap,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t zoneServerId;
        uint8_t zoneServerIp[32];
        uint32_t zoneServerPort;
        uint32_t mapId;
        uint8_t commanderListId;
        uint32_t spriteId;
        uint32_t spriteIdRelated;
        uint8_t isSingleMap;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = BC_START_GAMEOK;
        replyPacket.zoneServerId = zoneServerId;
        strncpy (replyPacket.zoneServerIp, zoneServerIp, sizeof (replyPacket.zoneServerIp));
        replyPacket.zoneServerPort = zoneServerPort;
        replyPacket.mapId = mapId;
        replyPacket.commanderListId = commanderListId;
        replyPacket.spriteId = spriteId;
        replyPacket.spriteIdRelated = spriteIdRelated;
        replyPacket.isSingleMap = isSingleMap;
    }
}

void
BarrackBuilder_commanderMoveOk (
    uint64_t accountId,
    uint16_t commanderListId,
    PositionXZ *position,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        PacketNormalHeader normalHeader;
        uint64_t accountId;
        uint16_t commanderListId;
        uint16_t unk1;
        PositionXZ position;
        uint8_t unk2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        PacketNormal_header (&replyPacket.normalHeader, BC_NORMAL_COMMANDER_MOVE_OK, sizeof (replyPacket));

        replyPacket.accountId = accountId;
        replyPacket.commanderListId = commanderListId;
        memcpy (&replyPacket.position, position, sizeof (PositionXZ));
    }
}

void
BarrackBuilder_serverEntry (
    uint32_t ipClientNet,
    uint32_t ipVirtualClientNet,
    uint16_t channelPort1,
    uint16_t channelPort2,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint32_t ipClientNet;
        uint32_t ipVirtualClientNet;
        uint16_t channelPort1;
        uint16_t channelPort2;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type        = BC_SERVER_ENTRY;
        replyPacket.ipClientNet        = ipClientNet;
        replyPacket.ipVirtualClientNet = ipVirtualClientNet;
        replyPacket.channelPort1       = channelPort1;
        replyPacket.channelPort2       = channelPort2;
    }
}

void
BarrackBuilder_commanderList (
    uint64_t accountId,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        VariableSizePacketHeader variableSizeHeader;
        uint64_t accountId;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        PacketVariableSize_header (&replyPacket.variableSizeHeader, BC_COMMANDER_LIST, sizeof (replyPacket));
        replyPacket.accountId = accountId;
    }
}

void
BarrackBuilder_zoneTraffics (
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        uint8_t zoneListId;
        uint16_t currentPlayersCount;
    } SingleZoneTraffic;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct {
        uint16_t mapId;
        uint16_t zoneServerCount;
        // SingleZoneTraffic zones[]; // variable length array
    }   SingleMapTraffic;
    #pragma pack(pop)

	#pragma pack(push, 1)
    typedef struct {
        PacketNormalHeader normalHeader;
        uint16_t zlibHeader;
        uint16_t zoneMaxPcCount;
        uint16_t mapAvailableCount;
        // SingleMapTraffic maps[]; // variable length array
    } ZoneTrafficsPacket;
    #pragma pack(pop)

    // === Data from the DB ===
    int zlibHeader = 0;
    int zoneMaxPcCount = 100;
    // Number of maps playable
    int mapAvailableCount = 1;
    // Array of zone server availables for each map
	int *zoneServerCounts = alloca (sizeof (*zoneServerCounts) * mapAvailableCount);
    // Array of mapId for each map
    int *mapsId = alloca (sizeof (*mapsId) * mapAvailableCount);
    // Fill the arrays here
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        zoneServerCounts [mapIndex] = 5;
        mapsId [mapIndex] = 0x3fd;
    }
    // Number of players per zone
    int currentPlayersCount = 10;

    // Count the total memory space needed for the reply packet
    int outPacketSize = sizeof (ZoneTrafficsPacket) + (sizeof (SingleMapTraffic) * mapAvailableCount);
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        outPacketSize += sizeof (SingleZoneTraffic) * zoneServerCounts[mapIndex];
    }

    // Allocate on the stack the memory for the packet
    unsigned char *stackBuffer = alloca (sizeof (*stackBuffer) * outPacketSize);
    ZoneTrafficsPacket *replyPacket = (ZoneTrafficsPacket *) stackBuffer;
    memset (replyPacket, 0, outPacketSize);

    // Construct the packet
    PacketStream stream;
    PacketStream_init (&stream, (unsigned char *) replyPacket);

    PacketNormal_header (&replyPacket->normalHeader, BC_NORMAL_ZONE_TRAFFIC, outPacketSize);
    PacketStream_addOffset (&stream, sizeof (replyPacket->normalHeader));

    PacketStream_append (&stream, &zlibHeader, sizeof_struct_member (ZoneTrafficsPacket, zlibHeader));
    PacketStream_append (&stream, &zoneMaxPcCount, sizeof_struct_member (ZoneTrafficsPacket, zoneMaxPcCount));
    PacketStream_append (&stream, &mapAvailableCount, sizeof_struct_member (ZoneTrafficsPacket, mapAvailableCount));

    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++)
    {
        PacketStream_append (&stream, &mapsId [mapIndex], sizeof_struct_member (SingleMapTraffic, mapId));
        PacketStream_append (&stream, &zoneServerCounts[mapIndex], sizeof_struct_member (SingleMapTraffic, zoneServerCount));

        for (int zoneServerIndex = 0; zoneServerIndex < zoneServerCounts[mapIndex]; zoneServerIndex++) {
            PacketStream_append (&stream, &zoneServerIndex, sizeof_struct_member (SingleZoneTraffic, zoneListId));
            PacketStream_append (&stream, &currentPlayersCount, sizeof_struct_member (SingleZoneTraffic, currentPlayersCount));
        }
    }

    zmsg_add (reply, zframe_new (replyPacket, outPacketSize));
}

void
BarrackBuilder_barrackNameChange (
    uint8_t *barrackName,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint8_t unk1[5];
        uint8_t barrackName[64];
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = BC_BARRACKNAME_CHANGE;
        memcpy (replyPacket.unk1, "\x01\x01\x01\x01\x01", sizeof (replyPacket.unk1));
        strncpy (replyPacket.barrackName, barrackName, sizeof (replyPacket.barrackName));
    }
}

void
BarrackBuilder_commanderDestroy (
    uint8_t commanderDestroyMask,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint8_t commanderDestroyMask;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = BC_COMMANDER_DESTROY;
        replyPacket.commanderDestroyMask = commanderDestroyMask;
    }
}

void
BarrackBuilder_commanderCreate (
    CommanderInfo *commander,
    zmsg_t *replyMsg
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        CommanderInfo commander;
    } replyPacket;
    #pragma pack(pop)

    BUILD_REPLY_PACKET (replyPacket, replyMsg)
    {
        replyPacket.header.type = BC_COMMANDER_CREATE;
        memcpy (&replyPacket.commander, commander, sizeof (CommanderInfo));
    }
}
