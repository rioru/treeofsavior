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
#include "Common/Commander.h"

// ------ Static declaration -------
/** Read the passport and accepts or refuse the authentication */
static bool BarrackHandler_loginByPassport   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Starts the barrack : call other handlers to initialize the barrack */
static bool BarrackHandler_startBarrack      (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Changes the barrack name */
static bool BarrackHandler_BarracknameChange (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Create a commander */
static bool BarrackHandler_commanderCreate   (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Register new servers */
static bool BarrackHandler_serverEntry       (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of commanders */
static bool BarrackHandler_commanderList     (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);


// ------ Structure declaration -------
/**
 * @brief barrackHandlers is a global table containing all the barrack handlers.
 */
const BarrackHandlers barrackHandlers [BARRACK_HANDLER_ARRAY_SIZE] = {
    #define REGISTER_PACKET_HANDLER(packetName, handler) \
        [packetName] = {handler, STRINGIFY (packetName)}

    REGISTER_PACKET_HANDLER (CB_LOGIN_BY_PASSPORT,  BarrackHandler_loginByPassport),
    REGISTER_PACKET_HANDLER (CB_START_BARRACK,      BarrackHandler_startBarrack),
    REGISTER_PACKET_HANDLER (CB_BARRACKNAME_CHANGE, BarrackHandler_BarracknameChange),
    REGISTER_PACKET_HANDLER (CB_COMMANDER_CREATE,   BarrackHandler_commanderCreate),

    #undef REGISTER_PACKET_HANDLER
};

// ------ Extern declaration -------
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
BarrackHandler_BarracknameChange (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
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
    strncpy (replyPacket.barrackName, packet + 2, sizeof (replyPacket.barrackName));
    dbg ("Current barrack name : %s", replyPacket.barrackName);

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return false;
}

static bool
BarrackHandler_commanderCreate (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        ServerPacketHeader header;
        CommanderInfo commander;
    } PacketCommanderCreate;
    #pragma pack(pop)

    PacketCommanderCreate replyPacket;

    replyPacket.header.type = BC_COMMANDER_CREATE;
    replyPacket.commander = Commander_CreateBasicCommander();
    strncpy (replyPacket.commander.charName, packet + 3, sizeof (replyPacket.commander.charName));
    replyPacket.commander.jobId = *(packet + 68);
    replyPacket.commander.gender = *(packet + 69);
    switch (replyPacket.commander.jobId)
    {
    default:
    case 1:
        replyPacket.commander.classId = 10001;
        break ;
    case 2:
        replyPacket.commander.classId = 10006;
        break ;
    case 3:
        replyPacket.commander.classId = 10003;
        break ;
    case 4:
        replyPacket.commander.classId = 10005;
        break ;
    }
    replyPacket.commander.listPosition = 1;
    replyPacket.commander.charPosition = 1;
    // Need to add hairType
    buffer_print (packet, packetSize, NULL);
    dbg ("Size of reply : %d", sizeof(replyPacket));

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

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


