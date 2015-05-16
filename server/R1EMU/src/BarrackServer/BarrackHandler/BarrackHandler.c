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
#include "Common/utils/random.h"
#include "Common/Packet/Packet.h"
#include "Common/Server/Worker.h"
#include "Common/Commander/Commander.h"
#include "Common/Packet/PacketStream.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Redis/Fields/RedisSocketSession.h"

// ------ Static declaration -------
/** Read the passport and accepts or refuse the authentification */
static PacketHandlerState BarrackHandler_loginByPassport   (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Read the login / password and accepts or refuse the authentification */
static PacketHandlerState BarrackHandler_login             (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Start the barrack : call other handlers that initializes the barrack */
static PacketHandlerState BarrackHandler_startBarrack      (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Once the commander list has been received, request to start the barrack */
static PacketHandlerState BarrackHandler_currentBarrack    (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Change a barrack name */
static PacketHandlerState BarrackHandler_barracknameChange (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Create a commander */
static PacketHandlerState BarrackHandler_commanderCreate   (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of zone servers */
static PacketHandlerState BarrackHandler_commanderDestroy  (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Change the commander position in the barrack */
static PacketHandlerState BarrackHandler_commanderMove     (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Makes the commander jumps in the barrack */
// static PacketHandlerState BarrackHandler_jump              (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Request for the player to enter in game */
static PacketHandlerState BarrackHandler_startGame         (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);

/** @UNKNOWN */
static PacketHandlerState BarrackHandler_iesModifyList     (Worker *self, zmsg_t *reply);
/** Register new servers */
static PacketHandlerState BarrackHandler_serverEntry       (Worker *self, zmsg_t *reply);
/** Send a list of commanders */
static PacketHandlerState BarrackHandler_commanderList     (Worker *self, Session *session, zmsg_t *reply);
/** Send a list of zone servers */
static PacketHandlerState BarrackHandler_zoneTraffics      (Worker *self, zmsg_t *reply);
/** @UNKNOWN. When this packet is sent to the client, it doesn't ask for the family name even when the account is new */
// static PacketHandlerState BarrackHandler_unkHandler1       (Worker *self, Session *session, zmsg_t *reply);
/** @UNKNOWN. Gives information to a specific commander. */
// static PacketHandlerState BarrackHandler_commanderInfo      (Worker *self, Session *session, zmsg_t *reply);
/** Send information about pets. */
// static PacketHandlerState BarrackHandler_petInfo           (Worker *self, Session *session, zmsg_t *reply);

// ------ Structure declaration -------
/**
 * @brief barrackHandlers is a global table containing all the barrack handlers.
 */
const PacketHandler barrackHandlers [PACKET_TYPE_COUNT] = {
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
    // REGISTER_PACKET_HANDLER (CB_JUMP,               BarrackHandler_jump),
    REGISTER_PACKET_HANDLER (CB_START_GAME,         BarrackHandler_startGame),

    #undef REGISTER_PACKET_HANDLER
};



PacketHandlerState
BarrackHandler_login (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    SocketSession *socketSession = &session->socket;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t accountId;
        unsigned char accountNameId[17];
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
    strncpy (replyPacket.accountNameId, "accountNameId", sizeof (replyPacket.accountNameId));
    // ==================================

    // Update the session
    socketSession->accountId = replyPacket.accountId;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}


static PacketHandlerState
BarrackHandler_loginByPassport (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    SocketSession *socketSession = &session->socket;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t accountId;
        unsigned char accountNameId[17];
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
    strncpy (replyPacket.accountNameId, "accountNameId", sizeof (replyPacket.accountNameId));
    // ==================================

    // Update the session
    socketSession->accountId = replyPacket.accountId;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_startGame (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        uint16_t routerId;
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

    // Retrieve zone servers IPs from Redis
    // Fake IPs here until we can retrieve the IP server database
    char *zoneServerIps[] = {
        "127.0.0.1",
        "46.105.97.46",
        "192.168.33.10"
    };
    int maxServerCount = sizeof_array (zoneServerIps);
    if (clientPacket->routerId >= maxServerCount) {
        error ("Invalid RouterId.");
        return PACKET_HANDLER_ERROR;
    }

    char *zoneServerIp = zoneServerIps [clientPacket->routerId];

    // Load Zone Server options
    // Until we can do it, fake server options
    replyPacket.header.type = BC_START_GAMEOK;
    replyPacket.zoneServerId = 0x12345678;
    strncpy (replyPacket.zoneServerIp, zoneServerIp, sizeof (replyPacket.zoneServerIp));
    replyPacket.zoneServerPort = 2004;
    replyPacket.mapId = 0x3fd;
    replyPacket.commanderListId = clientPacket->commanderListId;
    replyPacket.spriteId = 0;
    replyPacket.spriteIdRelated = 2;
    replyPacket.isSingleMap = false;

    // Update the session to the concerned Zone Server
    session->socket.routerId = clientPacket->routerId;
    session->socket.mapId = replyPacket.mapId;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

/*
static PacketHandlerState
BarrackHandler_jump (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
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
*/

static PacketHandlerState
BarrackHandler_commanderMove (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        uint8_t commanderListId;
        float x, y, z;
        float angleDestX, angleDestY;
    } CbCommanderMovePacket;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct {
        BarrackPacketNormalHeader normalHeader;
        uint64_t accountId;
        uint16_t commanderListId;
        uint16_t unk1;
        float x, y;
        uint8_t unk2;
    } BcNormalCommanderMoveOkPacket;
    #pragma pack(pop)

    if (sizeof (CbCommanderMovePacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbCommanderMovePacket));

        return PACKET_HANDLER_ERROR;
    }

    CbCommanderMovePacket *clientPacket = (CbCommanderMovePacket *) packet;

    BcNormalCommanderMoveOkPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    BarrackPacket_normalHeader (&replyPacket.normalHeader, BC_NORMAL_COMMANDER_MOVE_OK, sizeof (BcNormalCommanderMoveOkPacket));

    // TODO : Check position of the client
    replyPacket.accountId = session->socket.accountId;
    replyPacket.x = clientPacket->x;
    replyPacket.y = clientPacket->y;
    replyPacket.commanderListId = clientPacket->commanderListId;

    // zlist_t * clients = Redis_getClientsWithinDistance (self->redis, session->socket.serverId, session->socket.mapId, replyPacket.x, replyPacket.y, 0.0, 100.0);

    // Update session
    session->game.currentCommander.cPosX = replyPacket.x;
    session->game.currentCommander.cPosY = replyPacket.y;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

/*
static PacketHandlerState
BarrackHandler_commanderInfo (
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
    // BC_NORMAL_COMMANDER_INFO

	#pragma pack(push, 1)
    typedef struct {
        BarrackPacketNormalHeader normalHeader;
        uint64_t accountId;
    } BcNormalUnkHandler2Packet;
    #pragma pack(pop)

    size_t memSize;
    void *memory = dumpToMem (
        "[11:09:37][           ToSClient:                     dbgBuffer]  4F 00 FF FF FF FF 17 02 00 00 00 00 D1 A8 01 44 | O..............D\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 D1 A8 01 44 00 00 00 00 43 42 54 EC | .......D....CBT.\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  9A A9 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 00 00 00 00 00 00 00 00 1C 27 00 00 | .............'..\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  1F 00 01 00 4A 00 00 00 45 95 09 00 02 00 00 00 | ....J...E.......\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  04 00 00 00 97 1E 08 00 69 A9 07 00 83 CC 07 00 | ........i.......\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  10 27 00 00 F8 2A 00 00 77 C3 02 00 B1 5F 03 00 | .'...*..w...._..\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  15 7E 09 00 09 00 00 00 09 00 00 00 04 00 00 00 | .~..............\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  9C F3 07 00 09 00 00 00 09 00 00 00 00 30 09 00 | .............0..\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  F5 2F 09 00 E2 E1 08 00 2E 00 00 00 EC 28 00 00 | ./...........(..\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  D1 91 01 00 03 00 0B 05 04 00 00 00 49 02 00 00 | ............I...\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  EC 00 01 00 00 00 00 00 00 00 68 42 BD 63 90 41 | ..........hB.c.A\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 70 41 00 00 00 00 00 00 00 00 00 00 68 42 | ..pA..........hB\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  BD 63 90 41 00 00 70 41 00 00 00 00 00 00 00 00 | .c.A..pA........\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 06 00 A1 17 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  0C 00 FA 0F 00 00 11 45 A1 17 00 00 00 00 0C 00 | .......E........\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  FA 0F 00 80 36 45 A1 17 00 00 00 00 0C 00 FA 0F | ....6E..........\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 C0 80 45 A1 17 00 00 00 00 00 00 00 00 24 00 | ...E..........$.\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  70 10 00 00 A0 40 92 16 00 00 80 40 FA 0F 00 60 | p....@.....@...`\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  48 45 A1 17 00 00 00 00 0D 10 D0 1A 1D 49 12 10 | HE...........I..\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 A0 40 0C 00 FA 0F 00 C0 82 45 A1 17 00 00 | ...@.......E....\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 06 00 A1 17 00 00 00 00 00 00 00 00 00 00 | ................\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  0C 00 FA 0F 00 C0 80 45 A1 17 00 00 00 00 00 00 | .......E........\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 00 0C 00 FA 0F 00 00 75 45 A1 17 00 00 00 00 | ........uE......\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  17 00 6F 10 07 00 43 42 54 EC 9A A9 00 FA 0F 00 | ..o...CBT.......\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  00 75 45 A1 17 00 00 00 00 0C 00 FA 0F 00 00 75 | .uE............u\n"
        "[11:09:37][           ToSClient:                     dbgBuffer]  45 A1 17 00 00 00 00                            | E......\n"
        , NULL, &memSize
    );

    BcNormalUnkHandler2Packet *packet = memory;
    packet->accountId = session->socket.accountId;

    zmsg_add (reply, zframe_new (memory, memSize));

    return PACKET_HANDLER_OK;
}
*/

/*
static PacketHandlerState
BarrackHandler_unkHandler1 (
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        BarrackPacketNormalHeader normalHeader;
        uint64_t accountId;
        uint32_t unk1;
        uint32_t unk2;
        uint8_t unk3;
    } BcNormalUnknown1Packet;
    #pragma pack(pop)

    BcNormalUnknown1Packet replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Build BC_NORMAL header
    BarrackPacket_normalHeader (&replyPacket.normalHeader, BC_NORMAL_UNKNOWN_1, sizeof (BcNormalUnknown1Packet));

    replyPacket.accountId = session->socket.accountId;
    replyPacket.unk1 = 0xB;
    replyPacket.unk2 = 0;
    replyPacket.unk3 = 0;

    // Send message
    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}
*/

static PacketHandlerState
BarrackHandler_startBarrack (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    BarrackHandler_iesModifyList (self, reply);
    BarrackHandler_serverEntry   (self, reply);
    // BarrackHandler_unkHandler1   (self, session, reply);
    BarrackHandler_commanderList (self, session, reply);

    return PACKET_HANDLER_OK;
}

/*
static PacketHandlerState
BarrackHandler_petInfo (
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
	#pragma pack(push, 1)
    typedef struct {
        BarrackPacketNormalHeader normalHeader;
        uint64_t accountId;
        // TODO
        byte unk[180];
    } BcNormalPetInformationPacket;
    #pragma pack(pop)

    BcNormalPetInformationPacket replyPacket;
    memset (&replyPacket, 0, sizeof (BcNormalPetInformationPacket));

    size_t memSize = sizeof (BcNormalPetInformationPacket);
    dumpToMem (
		"[11:09:37][           ToSClient:                     dbgBuffer]  4F 00 FF FF FF FF C4 00 08 00 00 00 D1 A8 01 44 | O..............D\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 01 00 00 00 61 EA 00 00 E5 5E 00 00 | ........a....^..\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  79 9F 01 00 EC 28 00 00 D1 91 01 00 D7 37 00 00 | y....(.......7..\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  0A 00 EC 9A B0 EC AD 88 EC AD 88 00 00 00 00 00 | ................\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  7E 00 4E 11 00 00 00 00 B0 00 00 00 00 00 4F 11 | ~.N...........O.\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 A5 00 00 00 00 00 40 17 00 00 00 00 | ..........@.....\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  A1 00 00 00 00 00 F4 1D 00 00 00 00 05 1C 00 00 | ................\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  00 00 6A 17 00 00 00 00 65 17 00 00 00 00 62 00 | ..j.....e.....b.\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00 60 00 00 00 00 00 90 17 00 00 00 00 | ....`...........\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  A3 16 00 00 E0 41 64 1E 00 00 00 00 E5 1B 00 00 | .....Ad.........\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  00 00 EC 1B 00 00 00 00 EB 1B 00 00 00 00 03 1C | ................\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  00 00 C0 40 0A 1C 00 00 E0 40 E7 1B 00 00 20 41 | ...@.....@.... A\n"
		"[11:09:37][           ToSClient:                     dbgBuffer]  00 00 00 00                                     | ....\n"
        , &replyPacket, &memSize
    );

    replyPacket.accountId = session->socket.accountId;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (BcNormalPetInformationPacket)));

    return PACKET_HANDLER_OK;
}
*/

static PacketHandlerState
BarrackHandler_currentBarrack (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    //   [CLIENT SEND] Packet type : <CB_CURRENT_BARRACK>
    //   =================================================
    //    4E00 03000000 F7030000 D1A8014400000000 03000068 42F0968F 41000070 4111E334 3FCF2635 BF
    //    size pktType  checksum     accountId               float    float    float    float

    // BarrackHandler_petInfo (session, reply);
    // BarrackHandler_commanderInfo (self, session, reply);
    BarrackHandler_zoneTraffics (self, reply);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_barracknameChange (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
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
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
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
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    GameSession *gameSession = &session->game;
    SocketSession *socketSession = &session->socket;

    #pragma pack(push, 1)
    typedef struct {
        uint8_t charPosition;
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
    CommanderInfo_createBasicCommander (&replyPacket.commander);

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
    if (clientPacket->charPosition != gameSession->charactersBarrackCount + 1) {
        warning ("Client sent a malformed charPosition.");
    }

    replyPacket.commander.charPosition = gameSession->charactersBarrackCount + 1;

    // Hair type
    // TODO : Check the hairType
    replyPacket.commander.hairType = clientPacket->hairType;
    switch (clientPacket->hairType) {
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
    Worker *self,
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
BarrackHandler_iesModifyList (
    Worker *self,
    zmsg_t *reply
) {
    size_t memSize;
    void *memory = dumpToMem (
        "[11:09:35][           ToSClient:                     dbgBuffer]  48 00 FF FF FF FF F0 00 01 00 0C 00 53 68 61 72 | H...........Shar\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  65 64 43 6F 6E 73 74 00 03 00 FA 00 00 00 01 00 | edConst.........\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  06 00 56 61 6C 75 65 00 01 00 09 00 00 00 02 00 | ..Value.........\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  31 00 05 00 38 2E 30 30 00 0A 00 EC 9C 84 EC A0 | 1...8.00........\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  80 EB 93 9C 00 0A 00 32 30 31 35 2D 30 34 2D 32 | .......2015-04-2\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  00 0F 00 43 68 61 6E 67 65 20 42 79 20 54 6F 6F | ...Change By Too\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  6C 00 FB 00 00 00 01 00 06 00 56 61 6C 75 65 00 | l.........Value.\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  01 00 08 00 00 00 02 00 33 00 05 00 33 2E 30 30 | ........3...3.00\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  00 08 00 61 73 64 61 73 64 61 00 0A 00 32 30 31 | ...asdasda...201\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  35 2D 30 34 2D 32 00 0F 00 43 68 61 6E 67 65 20 | 5-04-2...Change \n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  42 79 20 54 6F 6F 6C 00 FF 00 00 00 01 00 06 00 | By Tool.........\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  56 61 6C 75 65 00 01 00 04 00 00 00 02 00 31 00 | Value.........1.\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  05 00 31 2E 30 30 00 0A 00 EB B2 95 EC 82 AC EB | ..1.00..........\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  82 A8 00 0A 00 32 30 31 35 2D 30 34 2D 32 00 0F | .....2015-04-2..\n"
        "[11:09:35][           ToSClient:                     dbgBuffer]  00 43 68 61 6E 67 65 20 42 79 20 54 6F 6F 6C 00 | .Change By Tool.\n"
        , NULL, &memSize
    );

    zmsg_add (reply, zframe_new (memory, memSize));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_serverEntry (
    Worker *self,
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
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        VariableSizePacketHeader variableSizeHeader;
        uint64_t accountId;
    } BcCommanderListPacket;
    #pragma pack(pop)

    BcCommanderListPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Empty commander list
    replyPacket.variableSizeHeader.serverHeader.type = BC_COMMANDER_LIST;
    replyPacket.accountId = session->socket.accountId;

	// Add dynamically the size of the packet
    replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}
