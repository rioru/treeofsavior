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
static PacketHandlerState ZoneHandler_connect (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);
/** Client is ready to enter the zone */
static PacketHandlerState ZoneHandler_gameReady (ClientSession *session, unsigned char *packet, size_t packetSize, zmsg_t *reply);


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
    } ZcStartGame;
    #pragma pack(pop)

    ZcStartGame replyPacket;

    replyPacket.header.type = ZC_START_GAME;
    replyPacket.timeMultiplier = 1.0;
    replyPacket.serverAppTimeOffset = 0.0;
    replyPacket.globalAppTimeOffset = 0.0;
    replyPacket.serverDateTime = 0.0;

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}


static PacketHandlerState
ZoneHandler_connect (
    ClientSession *session,
    unsigned char *packet,
    size_t packetSize,
    zmsg_t *reply
) {
    /*
     ===== [buffer size = 44 (0x2c) ================
     23 04 00 00 29 00 00 80 00 00 00 00 00 00 00 00 | #...)...........
     02 00 00 00 43 48 41 4E 4E 45 4C 5F 53 54 52 49 | ....CHANNEL_STRI
     4E 47 00 F7 18 78 56 34 12 00 00 01             | NG...xV4....
    =================================================
    */

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

    } ZcCommanderCreatePacket;
    #pragma pack(pop)

    ZcCommanderCreatePacket replyPacket;

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

    /*
		PCID = 0xFF;

		# ZC_CONNECT_OK = 0x0BBA, // Size: 0
		reply  = struct.pack("<H", PacketType.ZC_CONNECT_OK)
		reply += struct.pack("<I", 0); # UNKNOWN

		# reply += struct.pack("<H", X); # Size of the entire packet - Added dynamically at the end of the function
		reply += struct.pack("<B", 0); # 0 = NormalMode, 1 = SingleMode
		reply += struct.pack("<I", 1); # UNKNOWN
		reply += struct.pack("<B", 0); # AccountPrivileges (if < 3, the account is GM)

		# 10 bytes = UNKNOWN
		reply += "Aa0" # Padding ?
		reply += "Aa1Aa2A"

		# Pc session
		reply += struct.pack("<I", PCID); # PCID - Define a schrageID for the current PC - must *not* be zero
		reply += "4Aa5"

		# CommanderCreatePacket
		self.nbCharacterBarrack = 1;
		self.positionCharacterList = 1;
		charName = "Rioru";
		familyName = "FamilyName";
		classId = 10005; # 10001 = Warrior, 10006 = Mage, 10003 = Archer, 10005 = Cleric
		jobId = 4;       # 1     = Warrior, 2     = Mage, 3     = Archer, 4     = Cleric
		gender = 2;
		mapId = 0x408;
		characterLevel = 1337;
		# 0x2710 = no helmet (empty slot)
		itemsId = [0x00002710, 0x00099536, 0x00002710, 0x00081E91,  # Head 1   | Head 2     |    ?                               |  Armor
				   0x0007A959, 0x0007D071, 0x00002710, 0x000933E0,  # Gloves   | Boots      |    ? (makes the head disappear xD) |  Bracelet
				   0x00027D20, 0x00036511, 0x00098208, 0x00002710,  # Bow      | Shield     |   Costume                          |   ?
				   0x0007F782, 0x00099562, 0x0007F3A1, 0x00081AAC, 	# ?        |  ?         |   Pants                            |   ?
				   0x00002710, 0x00092C1A, 0x00092C1B, 0x0008DE0B];	# ?        | Ring left  |   Right right                      |  Necklace
		hairId = 3;
		spriteID = 0; # max 18
		reply += charName; # Character Name
		reply += "\x00" * (65 - len(charName));
		reply += familyName; # Right click description, family name
		reply += "\x00" * (65 - len(familyName));
		reply += struct.pack("<B", 0) * 6; # UNKNOWN
		reply += struct.pack("<q", 1000000); # account id
		reply += struct.pack("<H", classId) # class id
		reply += struct.pack("<H", 0) # UNKNOWN
		reply += struct.pack("<H", jobId) # job id
		reply += struct.pack("<B", gender) # Gender
		reply += struct.pack("<B", 1) # UNKNOWN
		reply += struct.pack("<I", characterLevel); # Character level
		for itemId in itemsId: # Inventory : 20 items
			reply += struct.pack("<I", itemId); # items
		reply += struct.pack("<B", hairId); # Hairstyle
		reply += struct.pack("<B", 0x00); # Return Value check CommanderCreatePacketData::CopyData
		reply += struct.pack("<B", 0x00) * 2; # UNKNOWN
		reply += struct.pack("<I", PCID); # PCID - Still need to understand how it works
		reply += struct.pack("<I", self.positionCharacterList) # Position in the character list
		reply += struct.pack("<B", self.nbCharacterBarrack) # Character position in the character list
		reply += struct.pack("<B", 0) # UNKNOWN
		reply += struct.pack("<H", mapId) # mapID

		reply += struct.pack("<I", 1337); # Current XP
		reply += struct.pack("<I", 31337); # Max XP
		reply += struct.pack("<I", 0); # UNKNOWN
		reply += struct.pack("<I", spriteID); # Apparence du sprite du corps
		reply += struct.pack("<I", 2); # UNKNOWN - Something related with SpriteID (apparence related)
		reply += struct.pack("<q", 1000000); # CID
		reply += struct.pack("<I", 4000); # Current HP
		reply += struct.pack("<I", 4200); # Max HP
		reply += struct.pack("<h", 1500); # Current SP
		reply += struct.pack("<h", 1600); # Max SP
		reply += struct.pack("<I", 0x00) * 5;

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";
    */

    zmsg_add (reply, zframe_new (&replyPacket, sizeof (replyPacket)));

    return PACKET_HANDLER_OK;
}
