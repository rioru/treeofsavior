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
#include "ZoneHandler.h"
#include "Common/Packet/Packet.h"
#include "Common/Commander/Commander.h"
#include "Common/Packet/PacketStream.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Server/Worker.h"
#include "BarrackServer/BarrackServer.h"


// ------ Static declaration -------
/** Connect to the zone server */
static PacketHandlerState ZoneHandler_connect       (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Client is ready to enter the zone */
static PacketHandlerState ZoneHandler_gameReady     (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send information about quickslots */
static void ZoneHandler_quickSlotListHandler        (Worker *self, Session *session, zmsg_t *reply);
/** Send information about the UI */
static void ZoneHandler_uiInfoList                  (Worker *self, Session *session, zmsg_t *reply);
/** Send information about Jobs */
static void ZoneHandler_startInfo                   (Worker *self, Session *session, zmsg_t *reply);
/** Send a commander movement speed */
static void ZoneHandler_moveSpeed                   (Worker *self, Session *session, zmsg_t *reply);
/** Alert the client that a new PC has entered */
static void ZoneHandler_MyPCEnter                   (Worker *self, Session *session, zmsg_t *reply);
/** Set the position of a commander */
// static void ZoneHandler_setPos                   (Worker *self, Session *session, zmsg_t *reply, uint32_t pcId, float x, float y, float z);
/** Jump handler */
static PacketHandlerState ZoneHandler_jump          (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On air handler */
static PacketHandlerState ZoneHandler_onAir         (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On ground handler */
static PacketHandlerState ZoneHandler_onGround      (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On move with keyboard handler */
static PacketHandlerState ZoneHandler_keyboardMove  (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On stop commander movement */
static PacketHandlerState ZoneHandler_moveStop      (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On movement information */
static PacketHandlerState ZoneHandler_movementInfo  (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On commander rotation */
static PacketHandlerState ZoneHandler_rotate        (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On commander head rotation */
static PacketHandlerState ZoneHandler_headRotate    (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** @unknown */
static PacketHandlerState ZoneHandler_campInfo      (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On log out */
static PacketHandlerState ZoneHandler_logout        (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);

// ------ Structure declaration -------
/**
 * @brief zoneHandlers is a global table containing all the zone handlers.
 */
const PacketHandler zoneHandlers [PACKET_TYPE_COUNT] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CZ_CONNECT, ZoneHandler_connect),
    REGISTER_PACKET_HANDLER (CZ_GAME_READY, ZoneHandler_gameReady),
    REGISTER_PACKET_HANDLER (CZ_JUMP, ZoneHandler_jump),
    REGISTER_PACKET_HANDLER (CZ_ON_AIR, ZoneHandler_onAir),
    REGISTER_PACKET_HANDLER (CZ_ON_GROUND, ZoneHandler_onGround),
    REGISTER_PACKET_HANDLER (CZ_KEYBOARD_MOVE, ZoneHandler_keyboardMove),
    REGISTER_PACKET_HANDLER (CZ_MOVE_STOP, ZoneHandler_moveStop),
    REGISTER_PACKET_HANDLER (CZ_MOVEMENT_INFO, ZoneHandler_movementInfo),
    REGISTER_PACKET_HANDLER (CZ_ROTATE, ZoneHandler_rotate),
    REGISTER_PACKET_HANDLER (CZ_HEAD_ROTATE, ZoneHandler_headRotate),
    REGISTER_PACKET_HANDLER (CZ_CAMPINFO, ZoneHandler_campInfo),
    REGISTER_PACKET_HANDLER (CZ_LOGOUT, ZoneHandler_logout),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState
ZoneHandler_campInfo (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    // Play the keyboard tutorial on login
    // ZC_ADDON_MSG is send after CZ_CAMPINFO :
    char *dump =
        "[11:10:23][           ToSClient:                     dbgBuffer]  4F 0C FF FF FF FF 1F 00 11 00 00 00 00 00 4B 45 | O.............KE\n"
        "[11:10:23][           ToSClient:                     dbgBuffer]  59 42 4F 41 52 44 5F 54 55 54 4F 52 49 41 4C    | YBOARD_TUTORIAL";

    size_t memSize;
    void *memory = dumpToMem (dump, NULL, &memSize);

    zmsg_add (reply, zframe_new (memory, memSize));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_logout (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_LOGOUT not implemented yet.");
    return PACKET_HANDLER_OK;
}


static PacketHandlerState
ZoneHandler_headRotate (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_HEAD_ROTATE not implemented yet.");
    return PACKET_HANDLER_OK;
}


static PacketHandlerState
ZoneHandler_rotate (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_ROTATE not implemented yet.");
    return PACKET_HANDLER_OK;
}


static PacketHandlerState
ZoneHandler_movementInfo (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_MOVEMENT_INFO not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_moveStop (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_MOVEMENT_INFO not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_keyboardMove (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_KEYBOARD_MOVE not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_gameReady (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        float timeMultiplier;
        float serverAppTimeOffset;
        float globalAppTimeOffset;
        double serverDateTime;
    } ZcStartGamePacket;
    #pragma pack(pop)

    ZcStartGamePacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = ZC_START_GAME;
    replyPacket.timeMultiplier = 1.0;
    replyPacket.serverAppTimeOffset = 0.0;
    replyPacket.globalAppTimeOffset = 0.0;
    replyPacket.serverDateTime = 0.0;

    // ===== The game starts from this point =====
    // Add additional information
    ZoneHandler_quickSlotListHandler (self, session, reply);
    ZoneHandler_uiInfoList (self, session, reply);
    ZoneHandler_startInfo (self, session, reply);
    ZoneHandler_moveSpeed (self, session, reply);
    ZoneHandler_MyPCEnter (self, session, reply);

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}


static PacketHandlerState
ZoneHandler_connect (
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
        uint32_t unk1;
        uint64_t accountId;
        uint32_t spriteId;
        uint32_t spriteIdRelated;
        unsigned char accountName[18];
        uint32_t zoneServerId;
        uint16_t unk3;
        uint8_t channelListId;
    } CzConnectPacket;
    #pragma pack(pop)

    #pragma pack(push, 1)
    typedef struct {
        VariableSizePacketHeader variableSizeHeader;
        uint8_t gameMode; // 0 = NormalMode, 1 = SingleMode
        uint32_t unk1;
        uint8_t accountPrivileges;
        uint8_t unk2[3];
        uint8_t unk3[7];
        uint32_t pcId;
        uint32_t unk4;
        CommanderInfo commander;

    } ZcConnectPacket;
    #pragma pack(pop)

    if (sizeof (CzConnectPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CzConnectPacket));

        return PACKET_HANDLER_ERROR;
    }

    CzConnectPacket *clientPacket = (CzConnectPacket *) packet;
    ZcConnectPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // Check the client packet here :
    if ((clientPacket->accountId != socketSession->accountId)
    // TODO : Complete the check
    ) {
        error ("Wrong account authentication.");
        return PACKET_HANDLER_ERROR;
    }

    // Build the reply packet
    replyPacket.variableSizeHeader.serverHeader.type = ZC_CONNECT_OK;
    replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);

    replyPacket.gameMode = 0;
    replyPacket.unk1 = 1;
    replyPacket.accountPrivileges = 0;
    replyPacket.pcId = gameSession->currentCommander.pcId;

    // Copy the commander Information
    memcpy (&replyPacket.commander, &gameSession->currentCommander, sizeof (CommanderInfo));

    // AccountID
    replyPacket.commander.accountId = socketSession->accountId;

    // PCID
    replyPacket.commander.pcId = gameSession->currentCommander.pcId;

    // CommanderID
    replyPacket.commander.commanderId = gameSession->currentCommander.commanderId;

    // Character position
    replyPacket.commander.charPosition = gameSession->charactersBarrackCount;

    // Position : Official starting point position (tutorial)
    gameSession->currentCommander.cPosX = -628.0f;
    gameSession->currentCommander.cPosY = 260.0f;
    replyPacket.commander.cPosX = gameSession->currentCommander.cPosX;
    replyPacket.commander.cPosY = gameSession->currentCommander.cPosY;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}

/*
static void
ZoneHandler_setPos (
    Worker *self,
    Session *session,
    zmsg_t *reply,
    uint32_t pcId,
    float x, float y, float z
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        float x, y, z;
    } ZcSetPosPacket;
    #pragma pack(pop)

    ZcSetPosPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = ZC_SET_POS;
    replyPacket.pcId = pcId;
    replyPacket.x = x;
    replyPacket.y = y;
    replyPacket.z = z;

    // Update session

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
}
*/

static void
ZoneHandler_MyPCEnter (
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        float x, y, z;
    } ZcMyPcEnterPacket;
    #pragma pack(pop)

    ZcMyPcEnterPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = ZC_MYPC_ENTER;

    replyPacket.x = session->game.currentCommander.cPosX;
    replyPacket.y = session->game.currentCommander.cPosY;
    replyPacket.z = -1025.0f;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
}

static void
ZoneHandler_moveSpeed (
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
    GameSession *gameSession = &session->game;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        float movementSpeed;
        float unk1;
    } ZcMoveSpeedPacket;
    #pragma pack(pop)

    ZcMoveSpeedPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = ZC_MOVE_SPEED;

    replyPacket.pcId = gameSession->currentCommander.pcId;
    replyPacket.movementSpeed = 100.0f;
    replyPacket.unk1 = 0.1f;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
}

static void
ZoneHandler_startInfo (
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
    warning ("CZ_START_INFO not implemented yet.");
}

static void
ZoneHandler_uiInfoList (
    Worker *self,
    Session *session,
    zmsg_t *reply
) {
    warning ("CZ_UI_INFO_LIST not implemented yet.");
}

static void
ZoneHandler_quickSlotListHandler (
    Worker *self,
    Session *session,
    zmsg_t *reply
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

    size_t memSize;
    void *packet = dumpToMem (
        "[11:36:22][           ToSClient:                     dbgBuffer]  30 0C FF FF FF FF 59 00 4D 00 00 00 63 60 72 9C | 0.....Y.M...c`r.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  C3 C0 E0 72 85 81 61 F1 7C 46 06 26 27 20 27 FF | ...r..a.|F.&' '.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  26 94 E3 0C E4 14 C0 38 2E 40 4E 21 94 C3 40 55 | &......8.@N!..@U\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  C0 1C 7D 84 93 E1 D3 65 88 A5 CC 71 40 CE 67 18 | ..}....e...q@.g.\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  27 11 C8 F9 02 E5 50 D5 CE 51 C3 06 7B 08 30 22 | '.....P..Q..{.0'\n"
        "[11:36:22][           ToSClient:                     dbgBuffer]  3B 90 09 99 C3 8C CC 01 00                      | ;........\n"
        , NULL, &memSize
    );

    zmsg_add (reply, zframe_new (packet, memSize));
}

static PacketHandlerState
ZoneHandler_jump (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    GameSession *gameSession = &session->game;

    #pragma pack(push, 1)
    typedef struct {
        uint8_t unk1;
    } CzJumpPacket;
    #pragma pack(pop)

    if (sizeof (CzJumpPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CzJumpPacket));

        return PACKET_HANDLER_ERROR;
    }

    // CzJumpPacket *clientPacket = (CzJumpPacket *) packet;

    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        float height;
        uint32_t unk1;
        uint8_t charPosition;
    } ZcJumpPacket;
    #pragma pack(pop)

    ZcJumpPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = ZC_JUMP;
    replyPacket.pcId = gameSession->currentCommander.pcId;
    replyPacket.height = 300.0;
    replyPacket.unk1 = 1;
    replyPacket.charPosition = 1;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_onAir (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_ON_AIR not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_onGround (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_ON_GROUND not implemented yet.");
    return PACKET_HANDLER_OK;
}
