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

// ------ Static declaration -------
/** Connect to the zone server */
static PacketHandlerState ZoneHandler_connect   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Client is ready to enter the zone */
static PacketHandlerState ZoneHandler_gameReady (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
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


// ------ Structure declaration -------
/**
 * @brief zoneHandlers is a global table containing all the zone handlers.
 */
const PacketHandler zoneHandlers [ZONE_HANDLER_ARRAY_SIZE] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CZ_CONNECT, ZoneHandler_connect),
    REGISTER_PACKET_HANDLER (CZ_GAME_READY, ZoneHandler_gameReady),

    #undef REGISTER_PACKET_HANDLER
};



static PacketHandlerState
ZoneHandler_gameReady (
    ClientSession *session,
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

    replyPacket.header.type = ZC_START_GAME;
    replyPacket.timeMultiplier = 1.0;
    replyPacket.serverAppTimeOffset = 0.0;
    replyPacket.globalAppTimeOffset = 0.0;
    replyPacket.serverDateTime = 0.0;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    // ===== The game starts from this point =====
    // Add additional information
    ZoneHandler_quickSlotListHandler (session, reply);
    ZoneHandler_uiInfoList (session, reply);
    ZoneHandler_startInfo (session, reply);
    ZoneHandler_moveSpeed (session, reply);
    ZoneHandler_MyPCEnter (session, reply);
    ZoneHandler_setPos (session, reply, session->currentPcId, 1142.29, 1000, -32.42);

    return PACKET_HANDLER_OK;
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

    replyPacket.header.type = ZC_MOVE_SPEED;

    replyPacket.pcId = session->currentPcId;
    replyPacket.movementSpeed = 100.0;
    replyPacket.unk1 = 0.1;

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

    replyPacket.variableSizeHeader.serverHeader.type = ZC_QUICK_SLOT_LIST;
    replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);
    replyPacket.zlibData = 0;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));
}


static PacketHandlerState
ZoneHandler_connect (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    buffer_print (packet, packetSize, "CONNECt : ");
    /*
     ===== [buffer size = 44 (0x2c) ================
     23 04 00 00 29 00 00 80 00 00 00 00 00 00 00 00 | #...)...........
     02 00 00 00 43 48 41 4E 4E 45 4C 5F 53 54 52 49 | ....CHANNEL_STRI
     4E 47 00 F7 18 78 56 34 12 00 00 01             | NG...xV4....
    =================================================
    */
    #pragma pack(push, 1)
    typedef struct {

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

    ZcConnectPacket replyPacket;

    replyPacket.variableSizeHeader.serverHeader.type = ZC_CONNECT_OK;
    replyPacket.variableSizeHeader.packetSize = sizeof (replyPacket);

    replyPacket.gameMode = 0;
    replyPacket.accountPrivileges = 1;
    replyPacket.pcId = session->currentPcId;

    replyPacket.commander = Commander_CreateBasicCommander ();

    // CharName
    strncpy (replyPacket.commander.charName, session->currentCommanderName, sizeof (replyPacket.commander.charName));

    // FamilyName
    strncpy (replyPacket.commander.familyName, session->familyName, sizeof (replyPacket.commander.familyName));

    // AccountID
    replyPacket.commander.accountId = session->accountId;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}
