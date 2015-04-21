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
#include "BarrackServer/BarrackWorker/BarrackWorker.h"

// ------ Static declaration -------
/** Read the passport and accepts or refuse the authentification */
static PacketHandlerState BarrackHandler_loginByPassport   (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Read the login / password and accepts or refuse the authentification */
static PacketHandlerState BarrackHandler_login             (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Start the barrack : call other handlers that initializes the barrack */
static PacketHandlerState BarrackHandler_startBarrack      (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Once the commander list has been received, request to start the barrack */
static PacketHandlerState BarrackHandler_currentBarrack    (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Change a barrack name */
static PacketHandlerState BarrackHandler_barracknameChange (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Create a commander */
static PacketHandlerState BarrackHandler_commanderCreate   (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Send a list of zone servers */
static PacketHandlerState BarrackHandler_commanderDestroy  (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Change the commander position in the barrack */
static PacketHandlerState BarrackHandler_commanderMove     (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Makes the commander jumps in the barrack */
static PacketHandlerState BarrackHandler_jump              (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Request for the player to enter in game */
static PacketHandlerState BarrackHandler_startGame         (Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);

/** Register new servers */
static PacketHandlerState BarrackHandler_serverEntry       (zmsg_t *reply);
/** Send a list of commanders */
static PacketHandlerState BarrackHandler_commanderList     (zmsg_t *reply);
/** Send a list of zone servers */
static PacketHandlerState BarrackHandler_zoneTraffics      (zmsg_t *reply);

// ------ Structure declaration -------
/**
 * @brief barrackHandlers is a global table containing all the barrack handlers.
 */
const PacketHandler barrackHandlers [BARRACK_HANDLER_ARRAY_SIZE] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CB_LOGIN,              BarrackHandler_login),
    REGISTER_PACKET_HANDLER (CB_LOGIN_BY_PASSPORT,  BarrackHandler_loginByPassport),
    REGISTER_PACKET_HANDLER (CB_START_BARRACK,      BarrackHandler_startBarrack),
    REGISTER_PACKET_HANDLER (CB_CURRENT_BARRACK,    BarrackHandler_currentBarrack),
    REGISTER_PACKET_HANDLER (CB_BARRACKNAME_CHANGE, BarrackHandler_barracknameChange),
    REGISTER_PACKET_HANDLER (CB_COMMANDER_CREATE,   BarrackHandler_commanderCreate),
    REGISTER_PACKET_HANDLER (CB_COMMANDER_DESTROY,  BarrackHandler_commanderDestroy),
    REGISTER_PACKET_HANDLER (CB_COMMANDER_MOVE,     BarrackHandler_commanderMove),
    REGISTER_PACKET_HANDLER (CB_JUMP,               BarrackHandler_jump),
    REGISTER_PACKET_HANDLER (CB_START_GAME,         BarrackHandler_startGame),

    #undef REGISTER_PACKET_HANDLER
};



static PacketHandlerState
BarrackHandler_login (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    SocketSession *socketSession = &session->socket;

    BarrackWorker * self = (BarrackWorker *) arg;

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

    // Authenticate here
    // TODO

    // Authentication OK!
    socketSession->authenticated = true;

    // ===== Gives a random account =====
    replyPacket.header.type = BC_LOGINOK;
    // Let's use a random accountId until we have a login mechanism
    replyPacket.accountId = R1EMU_generate_random64 (&self->seed);
    info ("AccountID %llx generated !", replyPacket.accountId);
    replyPacket.accountPrivileges = CLIENT_SESSION_PRIVILEGES_ADMIN;
    // TODO: Generate a challenge in channelString
    strncpy (replyPacket.channelString, "CHANNEL_STRING", sizeof (replyPacket.channelString));
    // ==================================

    // Update the session
    socketSession->accountId = replyPacket.accountId;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_startGame (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    #pragma pack(push, 1)
    typedef struct {
        uint16_t channelListId;
        uint8_t commanderListId;
    } CbStartGamePacket;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t zoneServerId;
        unsigned char zoneServerIp[32];
        uint32_t zoneServerPort;
        uint32_t mapId;
        uint8_t commanderListId;
        uint32_t spriteId;
        uint32_t spriteIdRelated;
        uint8_t isSingleMap;
    } BcStartGamePacket;
    #pragma pack(pop)


    if (sizeof (CbStartGamePacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbStartGamePacket));

        return PACKET_HANDLER_ERROR;
    }

    CbStartGamePacket *clientPacket = (CbStartGamePacket *) packet;
    BcStartGamePacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Retrieve zone servers IPs from global server
    char *zoneServerIps[] = {
        "127.0.0.1",
        "46.105.97.46",
        "192.168.33.10"
    };
    //! TODO : Check BOF
    char *zoneServerIp = zoneServerIps [clientPacket->channelListId];

    replyPacket.header.type = BC_START_GAMEOK;
    replyPacket.zoneServerId = 0x12345678;
    strncpy (replyPacket.zoneServerIp, zoneServerIp, sizeof (replyPacket.zoneServerIp));
    replyPacket.zoneServerPort = 2004;
    replyPacket.mapId = 0x551;
    replyPacket.commanderListId = clientPacket->commanderListId;
    replyPacket.spriteId = 0;
    replyPacket.spriteIdRelated = 2;
    replyPacket.isSingleMap = false;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}


static PacketHandlerState
BarrackHandler_jump (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    SocketSession *socketSession = &session->socket;

    #pragma pack(push, 1)
    typedef struct {
        uint32_t unk1;
        uint32_t unk2;
        uint8_t commanderListId;
    } CbJumpPacket;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        float jumpHeight;
        uint64_t accountId;
        uint8_t commanderListId;
    } BcJumpPacket;
    #pragma pack(pop)

    if (sizeof (CbJumpPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbJumpPacket));

        return PACKET_HANDLER_ERROR;
    }

    CbJumpPacket *clientPacket = (CbJumpPacket *) packet;

    BcJumpPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = BC_JUMP;
    replyPacket.commanderListId = clientPacket->commanderListId;
    replyPacket.jumpHeight = 1000.0;
    replyPacket.accountId = socketSession->accountId;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_commanderMove (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    #pragma pack(push, 1)
    typedef struct {
        uint8_t commanderListId;
        float x, y, z;
        float angleDestX, angleDestY;
    } CbCommanderMovePacket;
    #pragma pack(pop)

    if (sizeof (CbCommanderMovePacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbCommanderMovePacket));

        return PACKET_HANDLER_ERROR;
    }

    // CbCommanderMovePacket *clientPacket = (CbCommanderMovePacket *) packet;
    // Nothing to reply

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_loginByPassport (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    SocketSession *socketSession = &session->socket;

    BarrackWorker * self = (BarrackWorker *) arg;

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

    // Authenticate here
    // TODO

    // Authentication OK!
    socketSession->authenticated = true;

    // ===== Gives a random account =====
    replyPacket.header.type = BC_LOGINOK;
    // Let's use a random accountId until we have a login mechanism
    replyPacket.accountId = R1EMU_generate_random64 (&self->seed);
    info ("AccountID %llx generated !", replyPacket.accountId);
    replyPacket.accountPrivileges = CLIENT_SESSION_PRIVILEGES_ADMIN;
    // TODO: Generate a challenge in channelString
    strncpy (replyPacket.channelString, "CHANNEL_STRING", sizeof (replyPacket.channelString));
    // ==================================

    // Update the session
    socketSession->accountId = replyPacket.accountId;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_startBarrack (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    BarrackHandler_serverEntry   (reply);
    BarrackHandler_commanderList (reply);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_currentBarrack (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    BarrackHandler_zoneTraffics (reply);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_barracknameChange (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    GameSession *gameSession = &session->game;

    #pragma pack(push, 1)
    typedef struct {
        unsigned char barrackName [64];
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

    if (sizeof (CbBarrackNameChangePacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbBarrackNameChangePacket));

        return PACKET_HANDLER_ERROR;
    }

    BcBarrackNameChangePacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = BC_BARRACKNAME_CHANGE;
    memcpy (replyPacket.unk1, "\x01\x01\x01\x01\x01", sizeof (replyPacket.unk1));

    // Check if the barrack name is not empty and contains only ASCII characters
    size_t barrackNameLen = strlen (clientPacket->barrackName);
    if (barrackNameLen == 0) {
        error ("Empty barrack name");
        return PACKET_HANDLER_ERROR;
    }
    for (size_t i = 0; i < barrackNameLen; i++) {
         if (!isprint (clientPacket->barrackName[i])) {
            dbg ("Wrong barrack name size in BC_BARRACKNAME_CHANGE");
            return PACKET_HANDLER_ERROR;
         }
    }
    strncpy (replyPacket.barrackName, clientPacket->barrackName, sizeof (replyPacket.barrackName));

    // Update the session
    strncpy (gameSession->currentCommander.familyName, clientPacket->barrackName, sizeof (gameSession->currentCommander.familyName));

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_commanderDestroy (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    GameSession *gameSession = &session->game;

    typedef struct {
        ServerPacketHeader header;
        uint8_t action;
    } BcCommanderDestroyPacket;

    BcCommanderDestroyPacket replyPacket = {
        .header.type   = BC_COMMANDER_DESTROY,
        .action = 0xFF // Destroy all characters!
    };

    // Update session
    gameSession->charactersBarrackCount = 0;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}


static PacketHandlerState
BarrackHandler_commanderCreate (
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    GameSession *gameSession = &session->game;
    SocketSession *socketSession = &session->socket;

    BarrackWorker *self = (BarrackWorker *) arg;

    #pragma pack(push, 1)
    typedef struct {
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
    } BcCommanderCreatePacket;
    #pragma pack(pop)

    if (sizeof (CbCommanderCreatePacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbCommanderCreatePacket));

        return PACKET_HANDLER_ERROR;
    }

    BcCommanderCreatePacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = BC_COMMANDER_CREATE;
    CommanderInfo_createBasicCommander(&replyPacket.commander);

    // CharName
    strncpy (replyPacket.commander.charName, clientPacket->charName, sizeof (replyPacket.commander.charName));

    // FamilyName
    strncpy (replyPacket.commander.familyName, gameSession->currentCommander.familyName, sizeof (replyPacket.commander.familyName));

    // AccountID
    replyPacket.commander.accountId = socketSession->accountId;

    // JobID
    switch (clientPacket->jobId)
    {
        default:
            error ("Invalid commander Job ID (%d)", replyPacket.commander.jobId);
            return PACKET_HANDLER_ERROR;
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
            return PACKET_HANDLER_ERROR;
            break;
    }

    // Character position
    replyPacket.commander.listPosition = gameSession->charactersBarrackCount + 1;
    replyPacket.commander.charPosition = gameSession->charactersBarrackCount + 1;

    // Hair type
    switch (clientPacket->hairType) {
        // TODO : Check the hairType
        /*
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
            return PACKET_HANDLER_ERROR;
        break;
        */
    }

    // PCID
    replyPacket.commander.pcId = R1EMU_generate_random (&self->seed);

    // CommanderID
    replyPacket.commander.commanderId = R1EMU_generate_random64 (&self->seed);

    // Update the session
    gameSession->charactersBarrackCount++;
    gameSession->currentPcId = replyPacket.commander.pcId;
    gameSession->currentCommanderId = replyPacket.commander.commanderId;
    gameSession->currentCommander = replyPacket.commander;

    // Send the message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_zoneTraffics (
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

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_serverEntry (
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

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_commanderList (
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

    return PACKET_HANDLER_OK;
}
