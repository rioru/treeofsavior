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
#include "Common/Packet/Packet.h"
#include "Common/Commander/Commander.h"
#include "Common/Packet/PacketStream.h"

// ------ Static declaration -------
/** Read the passport and accepts or refuse the authentification */
static BarrackHandlerState BarrackHandler_loginByPassport   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Starts the barrack : call other handlers that initializes the barrack */
static BarrackHandlerState BarrackHandler_startBarrack      (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Starts the barrack : Once the commander list has been received, request to start the barrack */
static BarrackHandlerState BarrackHandler_currentBarrack    (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Changed a barrack name */
static BarrackHandlerState BarrackHandler_barracknameChange (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Create a commander */
static BarrackHandlerState BarrackHandler_commanderCreate   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Register new servers */
static BarrackHandlerState BarrackHandler_serverEntry       (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of commanders */
static BarrackHandlerState BarrackHandler_commanderList     (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of zone servers */
static BarrackHandlerState BarrackHandler_zoneTraffics      (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of zone servers */
static BarrackHandlerState BarrackHandler_commanderDestroy   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);


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
    REGISTER_PACKET_HANDLER (CB_BARRACKNAME_CHANGE, BarrackHandler_barracknameChange),
    REGISTER_PACKET_HANDLER (CB_COMMANDER_CREATE,   BarrackHandler_commanderCreate),
    REGISTER_PACKET_HANDLER (CB_COMMANDER_DESTROY,  BarrackHandler_commanderDestroy),

    #undef REGISTER_PACKET_HANDLER
};


static BarrackHandlerState
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
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Gives a random account
    replyPacket.header.type       = BC_LOGINOK;
    replyPacket.accountId         = R1EMU_generate_random64 ();
    replyPacket.accountPrivileges = CLIENT_SESSION_PRIVILEGES_ADMIN;
    strncpy (replyPacket.channelString, "CHANNEL_STRING", sizeof (replyPacket.channelString));

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_OK;
}

static BarrackHandlerState
BarrackHandler_startBarrack (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    BarrackHandler_serverEntry   (session, packet, packetSize, reply);
    BarrackHandler_commanderList (session, packet, packetSize, reply);

    return BARRACK_HANDLER_OK;
}

static BarrackHandlerState
BarrackHandler_currentBarrack (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    BarrackHandler_zoneTraffics (session, packet, packetSize, reply);

    return BARRACK_HANDLER_OK;
}

static BarrackHandlerState
BarrackHandler_barracknameChange (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct PacketBarrackNameChange {
        uint16_t unk1;
        unsigned char barrackName [62];
    }  PacketBarrackNameChange;
    #pragma pack(pop)
    PacketBarrackNameChange *clientPacket = (PacketBarrackNameChange *) packet;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        unsigned char unk1[5];
        unsigned char barrackName[64];
    } PacketBarracknameChange;
    #pragma pack(pop)

    PacketBarracknameChange replyPacket;

    replyPacket.header.type = BC_BARRACKNAME_CHANGE;
    strncpy (replyPacket.unk1, "\x01\x01\x01\x01\x01", sizeof (replyPacket.unk1));

    // Check if the barrack name is not empty and contains only ASCII characters
    size_t barrackNameLen = strlen (clientPacket->barrackName);
    if (barrackNameLen == 0) {
        error ("Empty barrack name");
        return BARRACK_HANDLER_ERROR;
    }
    for (int i = 0; i < barrackNameLen; i++) {
         if (!isprint (clientPacket->barrackName[i])) {
            dbg ("Wrong barrack name size in BC_BARRACKNAME_CHANGE");
            return BARRACK_HANDLER_ERROR;
         }
    }
    strncpy (replyPacket.barrackName, clientPacket->barrackName, sizeof (replyPacket.barrackName));

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_OK;
}

static BarrackHandlerState
BarrackHandler_commanderDestroy (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    typedef struct {
        ServerPacketHeader header;
        uint8_t action;
    } PacketCommanderDestroy;

    PacketCommanderDestroy replyPacket = {
        .header.type   = BC_COMMANDER_DESTROY,
        .action = 0xFF // Destroy all characters!
    };

    // Update session
    session->charactersBarrackCount = 0;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_UPDATE_SESSION;
}


static BarrackHandlerState
BarrackHandler_commanderCreate (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct PacketCbCommanderCreate {
        uint16_t unk1;
        uint8_t unk2;
        unsigned char charName[64];
        uint8_t unk3;
        uint8_t jobId;
        uint8_t gender;
        uint32_t unk4;
        uint32_t unk5;
        uint32_t unk6;
        uint8_t hairType;
    }  PacketCbCommanderCreate;
    #pragma pack(pop)
    PacketCbCommanderCreate *clientPacket = (PacketCbCommanderCreate *) packet;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        CommanderInfo commander;
    } PacketCommanderCreate;
    #pragma pack(pop)

    PacketCommanderCreate replyPacket;

    replyPacket.header.type = BC_COMMANDER_CREATE;
    replyPacket.commander = Commander_CreateBasicCommander();
    strncpy (replyPacket.commander.charName, clientPacket->charName, sizeof (replyPacket.commander.charName));

    switch (clientPacket->gender) {
        case COMMANDER_GENDER_MALE:
        case COMMANDER_GENDER_FEMALE:
            replyPacket.commander.gender = clientPacket->gender;
            break;

        case COMMANDER_GENDER_BOTH:
        default:
            error ("Invalid gender (%d)", clientPacket->gender);
            return BARRACK_HANDLER_ERROR;
            break;
    }

    switch (replyPacket.commander.jobId)
    {
        default:
            error ("Invalid commander Job ID (%d)", replyPacket.commander.jobId);
            return BARRACK_HANDLER_ERROR;
        break;
        case COMMANDER_JOB_WARRIOR:
            replyPacket.commander.classId = COMMANDER_CLASS_WARRIOR;
            break ;
        case COMMANDER_JOB_ARCHER:
            replyPacket.commander.classId = COMMANDER_CLASS_ARCHER;
            break ;
        case COMMANDER_JOB_MAGE:
            replyPacket.commander.classId = COMMANDER_CLASS_MAGE;
            break ;
        case COMMANDER_JOB_CLERIC:
            replyPacket.commander.classId = COMMANDER_CLASS_CLERIC;
            break ;
    }
    replyPacket.commander.jobId = clientPacket->jobId;

    replyPacket.commander.listPosition = session->charactersBarrackCount + 1;
    replyPacket.commander.charPosition = session->charactersBarrackCount + 1;

    switch (clientPacket->hairType) {
        case COMMANDER_HAIR_ID1:
        case COMMANDER_HAIR_ID2:
        case COMMANDER_HAIR_ID3:
        case COMMANDER_HAIR_ID4:
        case COMMANDER_HAIR_ID5:
        case COMMANDER_HAIR_ID6:
        case COMMANDER_HAIR_ID7:
        case COMMANDER_HAIR_ID8:
        case COMMANDER_HAIR_ID9:
        case COMMANDER_HAIR_ID10:
            replyPacket.commander.hairType = clientPacket->hairType;
        break;

        default:
            dbg ("Invalid hairType (%d)", clientPacket->hairType);
            return BARRACK_HANDLER_ERROR;
        break;
    }

    // Update the session
    session->charactersBarrackCount++;

    // Send the message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_UPDATE_SESSION;
}

static BarrackHandlerState
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
        // PacketZoneTraffic zones[]; // variable length array
    }   PacketMapTraffic;
    #pragma pack(pop)

	#pragma pack(push, 1)
    typedef struct PacketZoneTraffics {
        BarrackPacketNormalHeader normalHeader;
        uint16_t zlibHeader;
        uint16_t zoneMaxPcCount;
        uint16_t mapAvailableCount;
        // PacketMapTraffic maps[]; // variable length array
    } PacketZoneTraffics;
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
        mapsId [mapIndex] = 0x551;
    }
    // Number of players per zone
    int currentPlayersCount = 10;

    // Count the total memory space needed for the reply packet
    int outPacketSize = sizeof (PacketZoneTraffics) + (sizeof (PacketMapTraffic) * mapAvailableCount);
    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++) {
        outPacketSize += sizeof (PacketZoneTraffic) * zoneServerCounts[mapIndex];
    }

    // Allocate on the stack the memory for the packet
    unsigned char *stackBuffer = alloca (sizeof (*stackBuffer) * outPacketSize);
    PacketZoneTraffics *replyPacket = (PacketZoneTraffics *) stackBuffer;
    memset (replyPacket, 0, outPacketSize);

    // Construct the packet
    PacketStream stream;
    PacketStream_init (&stream, (unsigned char *) replyPacket);

    BarrackPacket_normalHeader (&replyPacket->normalHeader, BC_NORMAL_ZONE_TRAFFIC, outPacketSize);
    PacketStream_addOffset (&stream, sizeof (replyPacket->normalHeader));

    PacketStream_append (&stream, &zlibHeader, sizeof_struct_member (PacketZoneTraffics, zlibHeader));
    PacketStream_append (&stream, &zoneMaxPcCount, sizeof_struct_member (PacketZoneTraffics, zoneMaxPcCount));
    PacketStream_append (&stream, &mapAvailableCount, sizeof_struct_member (PacketZoneTraffics, mapAvailableCount));

    for (int mapIndex = 0; mapIndex < mapAvailableCount; mapIndex++)
    {
        PacketStream_append (&stream, &mapsId [mapIndex], sizeof_struct_member (PacketMapTraffic, mapId));
        PacketStream_append (&stream, &zoneServerCounts[mapIndex], sizeof_struct_member (PacketMapTraffic, zoneServerCount));

        for (int zoneServerIndex = 0; zoneServerIndex < zoneServerCounts[mapIndex]; zoneServerIndex++) {
            PacketStream_append (&stream, &zoneServerIndex, sizeof_struct_member (PacketZoneTraffic, zoneListId));
            PacketStream_append (&stream, &currentPlayersCount, sizeof_struct_member (PacketZoneTraffic, currentPlayersCount));
        }
    }

    zmsg_add (reply, zframe_new (replyPacket, outPacketSize));

    return BARRACK_HANDLER_OK;
}

static BarrackHandlerState
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
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Connect to localhost:1337 and localhost:1338
    replyPacket.header.type        = BC_SERVER_ENTRY;
    replyPacket.ipClientNet        = *(uint32_t *) ((char []) {127, 0, 0, 1});
    replyPacket.ipVirtualClientNet = *(uint32_t *) ((char []) {127, 0, 0, 1});
    replyPacket.channelPort1       = 1337;
    replyPacket.channelPort2       = 1338;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_OK;
}

static BarrackHandlerState
BarrackHandler_commanderList (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        VariableSizePacketHeader variableSizeHeader;
        uint32_t field2;
        uint32_t field3;
    } PacketCommanderList;
    #pragma pack(pop)

    PacketCommanderList replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Empty commander list
    replyPacket.variableSizeHeader.serverHeader.type = BC_COMMANDER_LIST;
    replyPacket.field2 = 0;
    replyPacket.field3 = 0;

	// Add dynamically the size of the packet
    replyPacket.variableSizeHeader.packetSize = sizeof (PacketCommanderList);

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_OK;
}


