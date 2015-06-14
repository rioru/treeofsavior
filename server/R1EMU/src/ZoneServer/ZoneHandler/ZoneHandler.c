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
#include "ZoneBuilder.h"
#include "Common/Redis/Fields/RedisGameSession.h"

// ------ Static declaration -------
/** Connect to the zone server */
static PacketHandlerState ZoneHandler_connect         (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Client is ready to enter the zone */
static PacketHandlerState ZoneHandler_gameReady       (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Jump handler */
static PacketHandlerState ZoneHandler_jump            (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On air handler */
static PacketHandlerState ZoneHandler_onAir           (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On ground handler */
static PacketHandlerState ZoneHandler_onGround        (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On move with keyboard handler */
static PacketHandlerState ZoneHandler_keyboardMove    (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On stop commander movement */
static PacketHandlerState ZoneHandler_moveStop        (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On movement information */
static PacketHandlerState ZoneHandler_movementInfo    (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On commander rotation */
static PacketHandlerState ZoneHandler_rotate          (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On commander head rotation */
static PacketHandlerState ZoneHandler_headRotate      (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** @unknown */
static PacketHandlerState ZoneHandler_campInfo        (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On log out */
static PacketHandlerState ZoneHandler_czQuickSlotList (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On log out */
static PacketHandlerState ZoneHandler_itemUse         (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On log out */
static PacketHandlerState ZoneHandler_iNeedParty      (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On log out */
static PacketHandlerState ZoneHandler_logout          (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On cast a spell on the ground */
static PacketHandlerState ZoneHandler_skillGround     (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** On commander sit */
static PacketHandlerState ZoneHandler_restSit         (Worker *self, Session *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);

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
    REGISTER_PACKET_HANDLER (CZ_QUICKSLOT_LIST, ZoneHandler_czQuickSlotList),
    REGISTER_PACKET_HANDLER (CZ_LOGOUT, ZoneHandler_logout),
    REGISTER_PACKET_HANDLER (CZ_ITEM_USE, ZoneHandler_itemUse),
    REGISTER_PACKET_HANDLER (CZ_I_NEED_PARTY, ZoneHandler_iNeedParty),
    REGISTER_PACKET_HANDLER (CZ_SKILL_GROUND, ZoneHandler_skillGround),
    REGISTER_PACKET_HANDLER (CZ_REST_SIT, ZoneHandler_restSit),

    #undef REGISTER_PACKET_HANDLER
};

static PacketHandlerState
ZoneHandler_restSit (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        // This packet is actually empty
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Make sit the current commander
    ZoneBuilder_restSit (session->game.currentCommander.pcId, reply);

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_skillGround (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
        uint32_t skillId;
        uint32_t unk2;
        Position3D pos1;
        Position3D pos2;
        float unk3;
        uint32_t unk4;
        uint32_t unk5;
        uint8_t unk6;
        uint8_t unk7;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    /*   CzSkillGroundPacket :
         u1 skillId  unk2     x        y        z        x2       y2       z2       u3       u4       u5       u6 u7
         00 419C0000 00000000 DAFD24C4 74768243 1B77F1C3 DAFD24C4 74768243 1B77F1C3 FFFF7FBF 0020B539 00000000 00 01
         00 419C0000 00000000 720344C4 74768243 2178F6C3 720344C4 74768243 2178F6C3 F30435BF F40435BF 00000000 00 01
         00 419C0000 00000000 C9EC91C4 74768243 17060AC4 C9EC91C4 74768243 17060AC4 FFFF7F3F 0020B5B9 00000000 00 00
         00 429C0000 00000000 5A00FAC3 1F7CA143 B1D3E843 5A00FAC3 1F7CA143 B1D3E843 EE04353F F60435BF 00000000 00 00
         00 439C0000 00000000 1E43FFC3 1F7CA143 E130D443 1E43FFC3 1F7CA143 E130D443 EF04353F F80435BF 00000000 00 00
    */

    // Not sure of the actual order
    ZoneBuilder_skillCast (
        session->game.currentCommander.pcId,
        clientPacket->skillId,
        &clientPacket->pos1,
        &clientPacket->pos2,
        reply
    );

    ZoneBuilder_playSkillCastAni (
        session->game.currentCommander.pcId,
        &clientPacket->pos1,
        reply
    );

    // This should be elsewhere
    ZoneBuilder_skillReady (
        session->game.currentCommander.pcId,
        clientPacket->skillId,
        &clientPacket->pos1,
        &clientPacket->pos2,
        reply
    );

    ZoneBuilder_playAni (reply);

    ZoneBuilder_normalUnk8 (
        session->game.currentCommander.pcId,
        reply
    );

    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_campInfo (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_CAMPINFO not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_itemUse (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_ITEM_USE not implemented yet.");
    return PACKET_HANDLER_OK;
}

static PacketHandlerState
ZoneHandler_iNeedParty (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_I_NEED_PARTY not implemented yet.");
    // ZoneBuilder_partyInfo (reply);
    // ZoneBuilder_partyList (reply);

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
ZoneHandler_czQuickSlotList (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    warning ("CZ_QUICKSLOT_LIST not implemented yet.");
    // Answer PacketType : ZC_QUICKSLOT_REGISTER
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
    ZoneBuilder_itemInventoryList (reply);
    ZoneBuilder_sessionObjects (reply);
    ZoneBuilder_optionList (reply);
    ZoneBuilder_skillmapList (reply);
    ZoneBuilder_achievePointList (reply);
    ZoneBuilder_chatMacroList (reply);
    ZoneBuilder_uiInfoList (reply);
    ZoneBuilder_npcStateList (reply);
    ZoneBuilder_helpList (reply);
    ZoneBuilder_myPageMap (reply);
    ZoneBuilder_guestPageMap (reply);
    ZoneBuilder_startInfo (reply);
    ZoneBuilder_itemEquipList (reply);
    ZoneBuilder_skillList (session->game.currentCommander.pcId, reply);
    ZoneBuilder_abilityList (session->game.currentCommander.pcId, reply);
    ZoneBuilder_cooldownList (reply);
    ZoneBuilder_quickSlotList (reply);
    ZoneBuilder_normalUnk1 (reply);
    ZoneBuilder_normalUnk2 (reply);
    ZoneBuilder_normalUnk3 (reply);
    ZoneBuilder_normalUnk4 (reply);
    ZoneBuilder_normalUnk5 (reply);
    ZoneBuilder_startGame (1.0, 0.0, 0.0, 0.0, reply);
    ZoneBuilder_objectProperty (reply);
    ZoneBuilder_stamina (reply);
    ZoneBuilder_loginTime (reply);

    Position3D enterPosition = {
        .x = session->game.currentCommander.cPosX,
        .y = session->game.currentCommander.cPosY,
        .z = -1025.0f
    };
    ZoneBuilder_MyPCEnter (&enterPosition, reply);
    ZoneBuilder_skillAdd (reply);
    ZoneBuilder_enterPc (&session->game.currentCommander, reply);
    ZoneBuilder_buffList (session->game.currentCommander.pcId, reply);

    // Add NPC at the start screen
    ZoneBuilder_enterMonster (reply);
    ZoneBuilder_faction (reply);

    ZoneBuilder_normalUnk6 (
        session->game.currentCommander.commanderName,
        reply
    );

    ZoneBuilder_normalUnk7 (
        session->socket.accountId,
        session->game.currentCommander.pcId,
        session->game.currentCommander.familyName,
        session->game.currentCommander.commanderName,
        reply
    );

    ZoneBuilder_jobPts (reply);
    ZoneBuilder_moveSpeed (session->game.currentCommander.pcId, 31.0, reply);
    ZoneBuilder_normalUnk9 (session->game.currentCommander.pcId, reply);
    ZoneBuilder_addonMsg (reply);

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
    #pragma pack(push, 1)
    struct {
        uint32_t unk1;
        uint64_t accountId;
        uint32_t spriteId;
        uint32_t spriteIdRelated;
        unsigned char accountLogin [GAME_SESSION_ACCOUNT_LOGIN_MAXSIZE];
        uint8_t unk4;
        uint32_t zoneServerId;
        uint16_t unk3;
        uint8_t channelListId;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    // Get the Game Session that the Barrack Server moved
    RedisGameSessionKey gameKey = {
        .routerId = self->info.routerId,
        .mapId = -1,
        .accountId = clientPacket->accountId
    };
    if (!(Redis_getGameSession (self->redis, &gameKey, &session->game))) {
        error ("Cannot retrieve the game session.");
        return PACKET_HANDLER_ERROR;
    }

    // Check the client packet here (authentication)
    if (strncmp (session->game.accountLogin, clientPacket->accountLogin, sizeof (session->game.accountLogin)) != 0) {
        error ("Wrong account authentication. (clientPacket account = <%s>, Session account = <%s>",
            clientPacket->accountLogin, session->game.accountLogin);
        return PACKET_HANDLER_ERROR;
    }

    // Authentication OK !
    // Update the Socket Session
    SocketSession_init (&session->socket,
        clientPacket->accountId,
        self->info.routerId,
        session->game.currentCommander.mapId,
        session->socket.socketId,
        true
    );

    // Move the game Session to the current mapId
    RedisGameSessionKey fromKey = {
        .routerId = session->socket.routerId,
        .mapId = -1,
        .accountId = session->socket.accountId
    };
    RedisGameSessionKey toKey = {
        .routerId = session->socket.routerId,
        .mapId = session->socket.mapId,
        .accountId = session->socket.accountId
    };

    if (!(Redis_moveGameSession (self->redis, &fromKey, &toKey))) {
        error ("Cannot move the game session to the current mapId.");
        return PACKET_HANDLER_ERROR;
    }

    // Position : Official starting point position (tutorial)
    session->game.currentCommander.cPosX = -628.0f;
    session->game.currentCommander.cPosY = 260.0f;

    ZoneBuilder_connect (
        0, // GameMode
        0, // accountPrivileges
        session->game.currentCommander.pcId, // targetPcId
        &session->game.currentCommander, // Current commander
        reply
    );

    return PACKET_HANDLER_UPDATE_SESSION;
}

static PacketHandlerState
ZoneHandler_jump (
    Worker *self,
    Session *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    #pragma pack(push, 1)
    struct {
        uint8_t unk1;
    } *clientPacket = (void *) packet;
    #pragma pack(pop)

    if (sizeof (*clientPacket) != packetSize) {
        error ("The packet size received isn't correct. (packet size = %d, correct size = %d)",
            packetSize, sizeof (*clientPacket));

        return PACKET_HANDLER_ERROR;
    }

    ZoneBuilder_jump (
        session->game.currentCommander.pcId,
        300.0,
        reply
    );

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
