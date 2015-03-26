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
#include "ZoneServer/ZoneWorker/ZoneWorker.h"

// ------ Static declaration -------
/** Connect to the zone server */
static PacketHandlerState ZoneHandler_connect   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Client is ready to enter the zone */
static PacketHandlerState ZoneHandler_gameReady (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Send information about quickslots */
static void ZoneHandler_quickSlotListHandler    (ClientSession *session, zmsg_t *reply);
/** Send information about the UI */
static void ZoneHandler_uiInfoList              (ClientSession *session, zmsg_t *reply);
/** Send information about Jobs */
static void ZoneHandler_startInfo               (ClientSession *session, zmsg_t *reply);
/** Send a commander movement speed */
static void ZoneHandler_moveSpeed               (ClientSession *session, zmsg_t *reply);
/** Alert the client that a new PC has entered */
static void ZoneHandler_MyPCEnter               (ClientSession *session, zmsg_t *reply);
/** Set the position of a commander */
static void ZoneHandler_setPos                  (ClientSession *session, zmsg_t *reply, uint32_t pcId, float x, float y, float z);
/** Jump handler */
static PacketHandlerState ZoneHandler_jump      (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Jump handler */
static PacketHandlerState ZoneHandler_onAir     (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);
/** Jump handler */
static PacketHandlerState ZoneHandler_onGround  (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply, void *arg);

// ------ Structure declaration -------
/**
 * @brief zoneHandlers is a global table containing all the zone handlers.
 */
const PacketHandler zoneHandlers [ZONE_HANDLER_ARRAY_SIZE] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CZ_CONNECT, ZoneHandler_connect),
    REGISTER_PACKET_HANDLER (CZ_GAME_READY, ZoneHandler_gameReady),
    REGISTER_PACKET_HANDLER (CZ_JUMP, ZoneHandler_jump),
    REGISTER_PACKET_HANDLER (CZ_ON_AIR, ZoneHandler_onAir),
    REGISTER_PACKET_HANDLER (CZ_ON_GROUND, ZoneHandler_onGround),

    #undef REGISTER_PACKET_HANDLER
};



static PacketHandlerState
ZoneHandler_gameReady (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
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
    ZoneHandler_quickSlotListHandler (session, reply);
    ZoneHandler_uiInfoList (session, reply);
    ZoneHandler_startInfo (session, reply);
    ZoneHandler_moveSpeed (session, reply);
    ZoneHandler_MyPCEnter (session, reply);
    ZoneHandler_setPos (session, reply, session->currentPcId, 1142.29f, 1000, -32.42f);

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}


static PacketHandlerState
ZoneHandler_connect (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    // zframe_t *barrackSessionFrame;
    // ClientSession *barrackSession;
    // ZoneWorker *self = (ZoneWorker *) arg;

    #pragma pack(push, 1)
    typedef struct {
        uint32_t unk1;
        uint64_t accountId;
        uint32_t unk2;
        uint32_t commanderListId;
        unsigned char channelString[17];
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

    // CzConnectPacket *clientPacket = (CzConnectPacket *) packet;
    ZcConnectPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    // A new user just connected to the zone server
    // Its session is empty and must be updated from the barrack server.
    // Ask for the session to the barrack server
    /*
    if (!(barrackSessionFrame = ZoneWorker_getBarrackSession (self, clientPacket->accountId))) {
        error ("Cannot retrieve the session from the barrack server.");
        return PACKET_HANDLER_ERROR;
    }

    barrackSession = (ClientSession *) zframe_data (barrackSessionFrame);
    (void) barrackSession;
    */

    // === Fake a correct session ===
    session->currentPcId = 0xFF;
    session->currentCommanderId = 0xBADBADBADBADBAD;
    session->charactersBarrackCount = 1;
    session->accountId = 0xDEADDEADDEADDEAD;
    strcpy (session->familyName, "FamilyName");
    strcpy (session->currentCommanderName, "Rioru");
    // ===============================

    replyPacket.variableSizeHeader.serverHeader.type = ZC_CONNECT_OK;
    replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);

    replyPacket.gameMode = 0;
    replyPacket.unk1 = 1;
    replyPacket.accountPrivileges = 0;
    replyPacket.pcId = session->currentPcId;

    replyPacket.commander = Commander_CreateBasicCommander ();

    // CharName
    strncpy (replyPacket.commander.charName, session->currentCommanderName, sizeof (replyPacket.commander.charName));

    // FamilyName
    strncpy (replyPacket.commander.familyName, session->familyName, sizeof (replyPacket.commander.familyName));

    // AccountID
    replyPacket.commander.accountId = session->accountId;

    // PCID
    replyPacket.commander.pcId = session->currentPcId;

    // CommanderID
    replyPacket.commander.commanderId = session->currentCommanderId;

    // Character position
    replyPacket.commander.listPosition = session->charactersBarrackCount;
    replyPacket.commander.charPosition = session->charactersBarrackCount;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_UPDATE_SESSION;
}


static void
ZoneHandler_setPos (
    ClientSession *session,
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

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
}

static void
ZoneHandler_MyPCEnter (
    ClientSession *session,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        uint32_t pcId;
        float unk1; // FSMActor.field_87
        float unk2; // FSMActor.field_88
    } ZcMyPcEnterPacket;
    #pragma pack(pop)

    ZcMyPcEnterPacket replyPacket;
    memset (&replyPacket, 0, sizeof (replyPacket));

    replyPacket.header.type = ZC_MYPC_ENTER;

    replyPacket.pcId = session->currentPcId;
    replyPacket.unk1 = 0;
    replyPacket.unk2 = 0;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
}

static void
ZoneHandler_moveSpeed (
    ClientSession *session,
    zmsg_t *reply
) {
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

    replyPacket.pcId = session->currentPcId;
    replyPacket.movementSpeed = 100.0f;
    replyPacket.unk1 = 0.1f;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
}

static void
ZoneHandler_startInfo (
    ClientSession *session,
    zmsg_t *reply
) {

}

static void
ZoneHandler_uiInfoList (
    ClientSession *session,
    zmsg_t *reply
) {

}

static void
ZoneHandler_quickSlotListHandler (
    ClientSession *session,
    zmsg_t *reply
) {
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
}

static PacketHandlerState
ZoneHandler_jump (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
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
    replyPacket.pcId = session->currentPcId;
    replyPacket.height = 300.0;
    replyPacket.unk1 = 1;
    replyPacket.charPosition = 1;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_onAir (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_onGround (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply,
    void *arg
) {
    return PACKET_HANDLER_OK;
}
