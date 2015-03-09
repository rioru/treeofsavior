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
#include "BarrackHandler.h"
#include "Packet/Packet.h"


// ------ Static declaration -------
/** Read the passport and accepts or refuse the authentification */
static bool BarrackHandler_loginByPassport (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Starts the barrack : call other handlers that initializes the barrack */
static bool BarrackHandler_startBarrack    (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Starts the barrack : Once the commander list has been received, request to start the barrack */
static bool BarrackHandler_currentBarrack  (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Register new servers */
static bool BarrackHandler_serverEntry     (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of commanders */
static bool BarrackHandler_commanderList   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of zone servers */
static bool BarrackHandler_zoneTraffics    (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);



// ------ Structure declaration -------
/**
 * @brief barrackHandlers is a global table containing all the barrack handlers.
 */
const BarrackHandlers barrackHandlers [BARRACK_HANDLER_ARRAY_SIZE] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CB_LOGIN_BY_PASSPORT,  BarrackHandler_loginByPassport),
    REGISTER_PACKET_HANDLER (CB_START_BARRACK,      BarrackHandler_startBarrack),
    REGISTER_PACKET_HANDLER (CB_CURRENT_BARRACK,    BarrackHandler_currentBarrack),

    #undef REGISTER_PACKET_HANDLER
};


static bool
BarrackHandler_loginByPassport (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply

) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t accountId;
        unsigned char channelString[17];
        uint32_t accountPrivileges;
    } PacketLoginOk;
    #pragma pack(pop)

    PacketLoginOk replyPacket;

    // Gives a random account
    replyPacket.header.type       = BC_LOGINOK;
    replyPacket.accountId         = R1EMU_generate_random64 ();
    replyPacket.accountPrivileges = CLIENT_SESSION_PRIVILEGES_ADMIN;
    strncpy (replyPacket.channelString, "CHANNEL_STRING", sizeof (replyPacket.channelString));

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return false;
}

static bool
BarrackHandler_startBarrack (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    BarrackHandler_serverEntry   (session, packet, packetSize, reply);
    BarrackHandler_commanderList (session, packet, packetSize, reply);

    return false;
}

static bool
BarrackHandler_currentBarrack (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    BarrackHandler_zoneTraffics (session, packet, packetSize, reply);

    return false;
}

static bool
BarrackHandler_zoneTraffics (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct PacketZoneTraffic {
        uint8_t zoneListId;
        uint16_t currentPlayersCount;
    } PacketZoneTraffic;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct PacketMapTraffic {
        uint16_t mapId;
        uint16_t zoneServerCount;
        PacketZoneTraffic zones[0]; // variable length array
    }   PacketMapTraffic;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct PacketZoneTraffics {
        BarrackPacketNormalHeader normalHeader;
        uint16_t zlibHeader;
        uint16_t maxPcCount;
        uint16_t mapAvailableCount;
        PacketMapTraffic maps[0]; // variable length array
    } PacketZoneTraffics;
    #pragma pack(pop)


    // === Data from the DB ===
    // Number of maps playable
    int mapAvailableCount = 1;
    // Array of zone server availables for each map
    int zoneServerCounts [mapAvailableCount];
    // Array of mapId for each map
    int mapsId [mapAvailableCount];
    // Fill the arrays here
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        zoneServerCounts [mapIndex] = 5;
        mapsId [mapIndex] = 0x551;
    }

    // Count the total memory space needed for the reply packet
    int outPacketSize = sizeof (PacketZoneTraffics) + (sizeof (PacketMapTraffic) * mapAvailableCount);
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        outPacketSize += sizeof (PacketZoneTraffic) * zoneServerCounts[mapIndex];
    }

    // Allocate on the stack the memory for the packet
    unsigned char stackBuffer [outPacketSize];
    PacketZoneTraffics *replyPacket = (PacketZoneTraffics *) stackBuffer;

    // Construct the packet
    replyPacket->zlibHeader = 0;
    replyPacket->maxPcCount = 100;
    replyPacket->mapAvailableCount = mapAvailableCount;
    BarrackPacket_normalHeader (&replyPacket->normalHeader, BC_NORMAL_ZONE_TRAFFIC, packetSize);

    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        PacketMapTraffic *map = &replyPacket->maps[mapIndex];
        map->mapId = mapsId [mapIndex];
        map->zoneServerCount = zoneServerCounts [mapIndex];

        for (int zoneServerIndex = 0; zoneServerIndex < zoneServerCounts[mapIndex]; zoneServerIndex++) {
            PacketZoneTraffic *zone = &map->zones [zoneServerIndex];
            zone->zoneListId = zoneServerIndex;
            zone->currentPlayersCount = 10;
        }
    }

    zmsg_add (reply, zframe_new (replyPacket, outPacketSize));

    return false;
}

static bool
BarrackHandler_serverEntry (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t ipClientNet;
        uint32_t ipVirtualClientNet;
        uint16_t channelPort1;
        uint16_t channelPort2;
    } PacketServerEntry;
    #pragma pack(pop)

    PacketServerEntry replyPacket;

    // Gives a random account
    replyPacket.header.type        = BC_SERVER_ENTRY;
    replyPacket.ipClientNet        = *((char []) {127, 0, 0, 1});
    replyPacket.ipVirtualClientNet = *((char []) {127, 0, 0, 1});
    replyPacket.channelPort1       = 1337;
    replyPacket.channelPort2       = 1338;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return false;
}

static bool
BarrackHandler_commanderList (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t packetLen;
        uint32_t field2;
        uint32_t field3;
    } PacketCommanderList;
    #pragma pack(pop)

    PacketCommanderList replyPacket;

    // Gives a random account
    replyPacket.header.type = BC_COMMANDER_LIST;
    replyPacket.field2 = 0;
    replyPacket.field3 = 0;

	// Add dynamically the size of the packet
    replyPacket.packetLen = sizeof (PacketCommanderList);

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return false;
}


