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
/** Start the barrack : call other handlers that initializes the barrack */
static BarrackHandlerState BarrackHandler_startBarrack      (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Once the commander list has been received, request to start the barrack */
static BarrackHandlerState BarrackHandler_currentBarrack    (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Change a barrack name */
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
static BarrackHandlerState BarrackHandler_commanderDestroy  (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of zone servers */
static BarrackHandlerState BarrackHandler_commanderMove     (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);


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
    REGISTER_PACKET_HANDLER (CB_COMMANDER_MOVE,     BarrackHandler_commanderMove),

    #undef REGISTER_PACKET_HANDLER
};


static BarrackHandlerState
BarrackHandler_commanderMove (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    buffer_print (packet, packetSize, "MOVE : ");

    #pragma pack(push, 1)
    typedef struct {
        uint16_t unk;
        uint8_t commanderListId;
        float x, y, z;
        float angleDestX, angleDestY;
    } CbCommanderMovePacket;
    #pragma pack(pop)

    if (sizeof (CbCommanderMovePacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbCommanderMovePacket));

        return BARRACK_HANDLER_ERROR;
    }

    CbCommanderMovePacket *clientPacket = (CbCommanderMovePacket *) packet;

    dbg ("commanderListId = %d", clientPacket->commanderListId);
    dbg ("x / y / z = %f / %f / %f", clientPacket->x, clientPacket->y, clientPacket->z);
    dbg ("angleDestX / angleDestY = %f / %f", clientPacket->angleDestX, clientPacket->angleDestY);

    // Nothing to reply

    return BARRACK_HANDLER_OK;
}

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
    } BcLoginOkPacket;
    #pragma pack(pop)

    BcLoginOkPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Gives a random account
    replyPacket.header.type       = BC_LOGINOK;
    replyPacket.accountId         = R1EMU_generate_random64 ();
    replyPacket.accountPrivileges = CLIENT_SESSION_PRIVILEGES_ADMIN;
    strncpy (replyPacket.channelString, "CHANNEL_STRING", sizeof (replyPacket.channelString));

    // Update the session
    session->accountId = replyPacket.accountId;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_UPDATE_SESSION;
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
    typedef struct {
        uint16_t unk1;
        unsigned char barrackName [62];
    }  CbBarrackNameChangePacket;
    #pragma pack(pop)
    CbBarrackNameChangePacket *clientPacket = (CbBarrackNameChangePacket *) packet;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        unsigned char unk1[5];
        unsigned char barrackName[64];
    } BcBarrackNameChangePacket;
    #pragma pack(pop)

    BcBarrackNameChangePacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = BC_BARRACKNAME_CHANGE;
    memcpy (replyPacket.unk1, "\x01\x01\x01\x01\x01", sizeof (replyPacket.unk1));

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

    // Update the session
    strncpy (session->familyName, clientPacket->barrackName, sizeof (session->familyName));

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_UPDATE_SESSION;
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
    } BcCommanderDestroyPacket;

    BcCommanderDestroyPacket replyPacket = {
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
    typedef struct {
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
    }  CbCommanderCreatePacket;
    #pragma pack(pop)
    CbCommanderCreatePacket *clientPacket = (CbCommanderCreatePacket *) packet;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        CommanderInfo commander;
    } BcPacketCommanderCreate;
    #pragma pack(pop)

    BcPacketCommanderCreate replyPacket;

    replyPacket.header.type = BC_COMMANDER_CREATE;
    replyPacket.commander = Commander_CreateBasicCommander();

    // CharName
    strncpy (replyPacket.commander.charName, clientPacket->charName, sizeof (replyPacket.commander.charName));

    // FamilyName
    strncpy (replyPacket.commander.familyName, session->familyName, sizeof (replyPacket.commander.familyName));

    // AccountID
    replyPacket.commander.accountId = session->accountId;

    // JobID
    switch (clientPacket->jobId)
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

    // Gender
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

    // Character position
    replyPacket.commander.listPosition = session->charactersBarrackCount + 1;
    replyPacket.commander.charPosition = session->charactersBarrackCount + 1;

    // Hair type
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

    // PCID
    replyPacket.commander.PCId = R1EMU_generate_random ();

    // CommanderID
    replyPacket.commander.commanderId = R1EMU_generate_random64 ();

    // Update the session
    session->charactersBarrackCount++;
    session->currentPCId = replyPacket.commander.PCId;
    session->currentCommanderId = replyPacket.commander.commanderId;

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
        BarrackPacketNormalHeader normalHeader;
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
        mapsId [mapIndex] = 0x551;
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

    BarrackPacket_normalHeader (&replyPacket->normalHeader, BC_NORMAL_ZONE_TRAFFIC, outPacketSize);
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
    } BcServerEntryPacket;
    #pragma pack(pop)

    BcServerEntryPacket replyPacket;
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
    } BcCommanderListPacket;
    #pragma pack(pop)

    BcCommanderListPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Empty commander list
    replyPacket.variableSizeHeader.serverHeader.type = BC_COMMANDER_LIST;
    replyPacket.field2 = 0;
    replyPacket.field3 = 0;

	// Add dynamically the size of the packet
    replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return BARRACK_HANDLER_OK;
}


