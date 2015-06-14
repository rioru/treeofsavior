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
#include "BarrackBuilder.h"
#include "Common/utils/random.h"
#include "Common/Packet/Packet.h"
#include "Common/Server/Worker.h"
#include "Common/Commander/Commander.h"
#include "Common/Packet/PacketStream.h"
#include "Common/Redis/Fields/RedisGameSession.h"
#include "Common/Redis/Fields/RedisSocketSession.h"

// ------ Static declaration -------
/** Read the passport and accepts or refuse the authentification */
static PacketHandlerState BarrackHandler_loginByPassport   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Read the login / password and accepts or refuse the authentification */
static PacketHandlerState BarrackHandler_login             (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Start the barrack : call other handlers that initializes the barrack */
static PacketHandlerState BarrackHandler_startBarrack      (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Once the commander list has been received, request to start the barrack */
static PacketHandlerState BarrackHandler_currentBarrack    (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Change a barrack name */
static PacketHandlerState BarrackHandler_barracknameChange (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Create a commander */
static PacketHandlerState BarrackHandler_commanderCreate   (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Send a list of zone servers */
static PacketHandlerState BarrackHandler_commanderDestroy  (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Change the commander position in the barrack */
static PacketHandlerState BarrackHandler_commanderMove     (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);
/** Request for the player to enter in game */
static PacketHandlerState BarrackHandler_startGame         (Worker *self, Session *session, uint8_t *packet, size_t packetSize, zmsg_t *reply);

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

static PacketHandlerState
BarrackHandler_login (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        uint8_t accountLogin [GAME_SESSION_ACCOUNT_LOGIN_MAXSIZE];
        uint8_t md5Password[17];
        uint8_t unk1[5];
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Authenticate here
    // TODO

    // Authentication OK!
    session->socket.authenticated = true;

    // Update the session
    // ===== Gives a fake admin account =====
    session->socket.accountId = R1EMU_generate_random64 (&self->seed);
    strncpy (session->game.accountLogin, clientPacket->accountLogin, sizeof (session->game.accountLogin));
    session->game.accountPrivilege = GAME_SESSION_PRIVILEGES_ADMIN;
    // ==================================
    info ("AccountID %llx generated !", session->socket.accountId);

    BarrackBuilder_loginOk (
        session->socket.accountId,
        session->game.accountLogin,
        session->game.accountPrivilege,
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_loginByPassport (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        ServerPacketHeader header;
        uint16_t unk1;
        uint64_t accountId;
        uint8_t accountLogin[GAME_SESSION_ACCOUNT_LOGIN_MAXSIZE];
        uint32_t accountPrivileges;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Authenticate here
    // TODO

    // Authentication OK!
    session->socket.authenticated = true;

    // Update the session
    // ===== Gives a fake account =====
    session->socket.accountId = R1EMU_generate_random64 (&self->seed);
    strncpy (session->game.accountLogin, clientPacket->accountLogin, sizeof (session->game.accountLogin));
    // ==================================
    info ("AccountID %llx generated !", session->socket.accountId);

    BarrackBuilder_loginOk (
        session->socket.accountId,
        session->game.accountLogin,
        GAME_SESSION_PRIVILEGES_ADMIN,
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_startGame (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        uint16_t routerId;
        uint8_t commanderListId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Retrieve zone servers IPs from Redis
    // Fake IPs here until we can retrieve the IPs database
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

    // Retrieve zone servers ports from Redis
    // Fake ports here until we can retrieve the ports database
    int zoneServerPorts [] = {
        2004, 2005, 2006
    };

    char *zoneServerIp = zoneServerIps [clientPacket->routerId];
    int zoneServerPort = zoneServerPorts [clientPacket->routerId];

    // Move the GameSession to the target Zone
    RedisGameSessionKey fromKey = {
        .routerId = session->socket.routerId,
        .mapId = session->socket.mapId,
        .accountId = session->socket.accountId
    };
    RedisGameSessionKey toKey = {
        .routerId = clientPacket->routerId, // target zoneId
        .mapId = -1,
        .accountId = session->socket.accountId
    };
    if (!(Redis_moveGameSession (self->redis, &fromKey, &toKey))) {
        error ("Cannot move the Game session %s.", session->socket.socketId);
        return PACKET_HANDLER_ERROR;
    }

    // Build the answer packet
    BarrackBuilder_startGameOk (
        clientPacket->routerId,
        zoneServerIp,
        zoneServerPort,
        session->game.currentCommander.mapId,
        clientPacket->commanderListId,
        session->game.currentCommander.spriteId,
        session->game.currentCommander.spriteIdRelated,
        false,
        reply
    );

    // We don't need the barrack session anymore
    return PACKET_HANDLER_DELETE_SESSION;
}

static PacketHandlerState
BarrackHandler_commanderMove (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        uint8_t commanderListId;
        PositionXYZ position;
        float angleDestX, angleDestY;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // TODO : Check position of the client

    // Update session
    session->game.currentCommander.cPosX = clientPacket->position.x;
    session->game.currentCommander.cPosZ = clientPacket->position.z;

    // Build packet
    PositionXZ okPosition = {
        .x = session->game.currentCommander.cPosX,
        .z = session->game.currentCommander.cPosZ
    };
    BarrackBuilder_commanderMoveOk (
        session->socket.accountId,
        clientPacket->commanderListId,
        &okPosition,
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_startBarrack (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    // Connect to S Server at localhost:1337 and localhost:1338
    BarrackBuilder_serverEntry (
        *(uint32_t *) ((char []) {127, 0, 0, 1}),
        *(uint32_t *) ((char []) {127, 0, 0, 1}),
        1337,
        1338,
        reply
    );

    // Send the commander list
    BarrackBuilder_commanderList (
        session->socket.accountId,
        reply
    );

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_currentBarrack (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    //   [CLIENT SEND] Packet type : <CB_CURRENT_BARRACK>
    //   =================================================
    //    4E00 03000000 F7030000 D1A8014400000000 03000068 42F0968F 41000070 4111E334 3FCF2635 BF
    //    size pktType  checksum     accountId               float    float    float    float

    BarrackBuilder_zoneTraffics (reply);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
BarrackHandler_barracknameChange (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    typedef struct {
        uint8_t barrackName [64];
    }  CbBarrackNameChangePacket;
    #pragma pack(pop)
    CbBarrackNameChangePacket *clientPacket = (CbBarrackNameChangePacket *) packet;

    if (sizeof (CbBarrackNameChangePacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (CbBarrackNameChangePacket));

        return PACKET_HANDLER_ERROR;
    }

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

    // Update the session
    strncpy (session->game.currentCommander.familyName, clientPacket->barrackName, sizeof (session->game.currentCommander.familyName));

    // Build the reply packet
    BarrackBuilder_barrackNameChange (
        session->game.currentCommander.familyName,
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
BarrackHandler_commanderDestroy (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    uint8_t commanderDestroyMask = 0xFF; // Destroy all characters!

    // Update session
    session->game.charactersBarrackCount = 0;

    // Build the reply packet
    BarrackBuilder_commanderDestroy (commanderDestroyMask, reply);

    return PACKET_HANDLER_UPDATE_SESSION;
}


static PacketHandlerState
BarrackHandler_commanderCreate (
    Worker *self,
    Session *session,
    uint8_t *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        uint8_t charPosition;
        uint8_t commanderName[64];
        uint8_t unk3;
        uint8_t jobId;
        uint8_t gender;
        uint32_t unk4;
        uint32_t unk5;
        uint32_t unk6;
        uint8_t hairType;
    }  *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // CharName
    strncpy (session->game.currentCommander.commanderName, clientPacket->commanderName, sizeof (session->game.currentCommander.commanderName));

    // AccountID
    session->game.currentCommander.accountId = session->socket.accountId;

    // JobID
    switch (clientPacket->jobId)
    {
        default:
            error ("Invalid commander Job ID (%d)", session->game.currentCommander.jobId);
            return PACKET_HANDLER_ERROR;
        break;
        case COMMANDER_JOB_WARRIOR:
            session->game.currentCommander.classId = COMMANDER_CLASS_WARRIOR;
            break ;
        case COMMANDER_JOB_ARCHER:
            session->game.currentCommander.classId = COMMANDER_CLASS_ARCHER;
            break ;
        case COMMANDER_JOB_MAGE:
            session->game.currentCommander.classId = COMMANDER_CLASS_MAGE;
            break ;
        case COMMANDER_JOB_CLERIC:
            session->game.currentCommander.classId = COMMANDER_CLASS_CLERIC;
            break ;
    }
    session->game.currentCommander.jobId = clientPacket->jobId;

    // Gender
    switch (clientPacket->gender) {
        case COMMANDER_GENDER_MALE:
        case COMMANDER_GENDER_FEMALE:
            session->game.currentCommander.gender = clientPacket->gender;
            break;

        case COMMANDER_GENDER_BOTH:
        default:
            error ("Invalid gender (%d)", clientPacket->gender);
            return PACKET_HANDLER_ERROR;
            break;
    }

    // Character position
    if (clientPacket->charPosition != session->game.charactersBarrackCount + 1) {
        warning ("Client sent a malformed charPosition.");
    }

    session->game.currentCommander.charPosition = session->game.charactersBarrackCount + 1;

    // Hair type
    session->game.currentCommander.hairType = clientPacket->hairType;

    // PCID
    session->game.currentCommander.pcId = R1EMU_generate_random (&self->seed);
    info ("pcId generated : %x", session->game.currentCommander.pcId);

    // CommanderID
    session->game.currentCommander.commanderId = R1EMU_generate_random64 (&self->seed);
    info ("CommanderId generated : %llx", session->game.currentCommander.commanderId);

    // Position : Center of the barrack
    session->game.currentCommander.cPosX = 27.0f;
    session->game.currentCommander.cPosZ = 29.0f;

    // Default MapId
    session->game.currentCommander.mapId = 0x3FD;

    // Add the character to the account
    session->game.charactersBarrackCount++;

    // Build the reply packet
    BarrackBuilder_commanderCreate (&session->game.currentCommander, reply);

    return PACKET_HANDLER_UPDATE_SESSION;
}
