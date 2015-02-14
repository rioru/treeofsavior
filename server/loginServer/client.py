# Dependencies
import binascii
import struct
import time
import socket
import io
from PacketType import PacketType

def stream_unpack (fmt, stream):
    size = struct.calcsize(fmt)
    buf = stream.read(size)
    return struct.unpack(fmt, buf)[0];

# ==========================================================
class ClientHandler:

	# Configurations
	PACKETSIZE_MAX = 8192

	nbCharacterBarrack = 0;
	positionCharacterList = 0;

	def loginByPasswordHandler (self, packet):
		# CB_LOGIN_BY_PASSPORT = 0x0004, // Size: 531
		print 'CB_LOGIN_BY_PASSPORT expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		reply  = struct.pack("<H", PacketType.BC_LOGINOK)
		reply += "\x00" * 8
		reply += "\x01" * 1 # Send movements to server in barrack + allow mouse movements
		reply += "\x00" * 24
		reply += "\x00" * 1 # 0 = developper mode, otherwise normal mode
		reply += "\x00" * 1

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def startBarrackHandler (self, packet):
		# CB_START_BARRACK = 0x0006, // Size: 11
		print 'CB_START_BARRACK expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		self.serverEntryHandler (packet);
		self.commanderListHandler (packet);
		self.singleInfoHandler(packet);


	def serverEntryHandler (self, packet):
		# BC_SERVER_ENTRY = 0x0057, // Size: 18
		channelIP = "127.0.0.1";
		channelPort1 = 1337;
		channelPort2 = 1338;

		reply  = struct.pack("<H", PacketType.BC_SERVER_ENTRY)
		reply += struct.pack("<I", 1); # UNKNOWN - 0 throws an error (IESError004) in log

		channelIP = socket.inet_aton (channelIP);
		reply += channelIP; # ClientNet
		reply += channelIP; # VirtualClientNet

		reply += struct.pack("<H", channelPort1);
		reply += struct.pack("<H", channelPort2);

		self.sock.send (reply);
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def commanderListHandler (self, packet):
		# BC_COMMANDER_LIST = 0x000F, // Size: 0
		createNewAccount = True;
		nbCharacterListed = 1;

		reply  = struct.pack("<H", PacketType.BC_COMMANDER_LIST)
		reply += struct.pack("<I", 0); # UNKNOWN

		# reply += struct.pack("<H", X); # Size of the entire packet - Added dynamically at the end of the function
		reply += struct.pack("<I", 0); # Field 2 -
		reply += struct.pack("<I", 0); # Field 3 - Field 2 and 3 are used for visiting other barracks.

		if not createNewAccount:
			barrackName = "Spl3en"
			#barrackName = packet[10:72]
			reply += struct.pack("<B", 0xFF); # UNKNOWN (seems not to be used)
			reply += struct.pack("<B", nbCharacterListed); # Number of characters contained in the packet
			reply += barrackName + "\x00" * (64 - len(barrackName));

			data = struct.pack("<H", 0x1e8); # Barrack class ID
			reply += struct.pack("<H", len(data)); # Size of data following this field
			reply += data;

			# CommanderCreatePacket-like
			self.nbCharacterBarrack += 1;
			self.positionCharacterList += 1;
			charName = "Rioru";
			familyName = "I'm displayed by a right click on the character";
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

			# Packet
			reply += charName; # Character Name
			reply += "\x00" * (65 - len(charName));
			reply += familyName; # Right click description, family name
			reply += "\x00" * (65 - len(familyName));
			reply += struct.pack("<B", 0) * 6; # UNKNOWN
			reply += struct.pack("<I", 0); # UNKNOWN
			reply += struct.pack("<I", 0); # UNKNOWN
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

			reply += struct.pack("<I", 0x000000FF); # PCID - Still need to understand how it works

			reply += struct.pack("<I", self.positionCharacterList) # Position in the character list
			reply += struct.pack("<B", self.nbCharacterBarrack) # Character position in the character list
			reply += struct.pack("<B", 0) # UNKNOWN
			reply += struct.pack("<H", mapId) # mapID
			reply += struct.pack("<I", 1337); # Current XP
			reply += struct.pack("<I", 31337); # Max XP
			reply += struct.pack("<I", 0); # UNKNOWN
			reply += struct.pack("<I", spriteID) * 1; # Apparence du sprite du corps
			reply += struct.pack("<I", 0) * 1; # UNKNOWN
			reply += struct.pack("<I", 0) * 2; # UNKNOWN
			reply += struct.pack("<I", 0) * 8; # UNKNOWN

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];

		self.sock.send (reply);
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def currentBarrackHandler (self, packet):
		# CB_CURRENT_BARRACK = 0x004E, // Size: 39
		print 'CB_CURRENT_BARRACK expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";


	def barrackNameChangeHandler (self, packet):
		# CB_BARRACKNAME_CHANGE = 0x000A, // Size: 74
		print 'CB_BARRACKNAME_CHANGE expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		# BC_BARRACKNAME_CHANGE = 0x0017, // Size: 75
		#barrackName = "Spl3en"
		barrackName = packet[10:72]
		reply  = struct.pack("<H", PacketType.BC_BARRACKNAME_CHANGE)
		reply += "\x01" * 9;
		reply += barrackName + "\x00" * (64 - len(barrackName));
		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def commanderCreateHandler (self, packet):
		# CB_COMMANDER_CREATE = 0x0007, // Size: 91
		print 'Expected CB_COMMANDER_CREATE. Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";

		# BC_COMMANDER_CREATE = 0x0010, // Size: 318
		self.nbCharacterBarrack += 1;
		self.positionCharacterList += 1;
		#charName = "Rioru";
		charName = packet[11:76]
		familyName = "I'm displayed by a right click on the character";
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

		reply  = struct.pack("<H", PacketType.BC_COMMANDER_CREATE);
		reply += struct.pack("<I", 0xFFFFFFFF); # UNKNOWN
		reply += charName; # Character Name
		reply += "\x00" * (65 - len(charName));
		reply += familyName; # Right click description, family name
		reply += "\x00" * (65 - len(familyName));
		reply += struct.pack("<B", 0) * 6; # UNKNOWN
		reply += struct.pack("<I", 0); # UNKNOWN
		reply += struct.pack("<I", 0); # UNKNOWN
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

		reply += struct.pack("<I", 0x000000FF); # PCID - Still need to understand how it works

		reply += struct.pack("<I", self.positionCharacterList) # Position in the character list
		reply += struct.pack("<B", self.nbCharacterBarrack) # Character position in the character list
		reply += struct.pack("<B", 0) # UNKNOWN
		reply += struct.pack("<H", mapId) # mapID
		reply += struct.pack("<I", 1337); # Current XP
		reply += struct.pack("<I", 31337); # Max XP
		reply += struct.pack("<I", 0); # UNKNOWN
		reply += struct.pack("<I", spriteID) * 1; # Apparence du sprite du corps
		reply += struct.pack("<I", 0) * 1; # UNKNOWN
		reply += struct.pack("<I", 0) * 2; # UNKNOWN
		reply += struct.pack("<I", 0) * 8; # UNKNOWN

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def jumpHandler (self, packet):
		# CB_JUMP = 0x0055, // Size: 19
		print 'Expected CB_JUMP. Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";
		packetStream = io.BytesIO (packet);
		packetType = stream_unpack ("<H", packetStream);
		packetNum = stream_unpack ("<I", packetStream);
		packetChecksum = stream_unpack ("<H", packetStream);
		unk1 = stream_unpack ("<H", packetStream);
		unk2 = stream_unpack ("<H", packetStream);
		bIsJumping = stream_unpack ("<B", packetStream);
		unk3 = stream_unpack ("<I", packetStream);

		if packetStream.read (1):
			print "WARNING : The packet still contains data sent from the client that hasn't been read.";

		# BC_JUMP = 0x0056, // Size: 19
		reply  = struct.pack("<H", PacketType.BC_JUMP);
		reply += struct.pack("<I", 0xAAAAAAAA); # UNKNOWN
		reply += struct.pack("<I", 0xBBBBBBBB); # UNKNOWN
		reply += struct.pack("<I", 0x00010000); # UNKNOWN - This value is checked in a conditionnal jump
		reply += struct.pack("<I", 0); # UNKNOWN -This value is checked in a conditionnal jump
		reply += struct.pack("<B", 1); # Character Id Jumping

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def moveHandler (self, packet):
		print 'CB_COMMANDER_MOVE expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		sequenceNumber, checksum, unk, x, y, z, g, h, i = struct.unpack("<iI?fffhhh", packet[2:29])
		print "nbPacket : " + str(sequenceNumber)
		print "unk : %d" % unk
		print "Position : %f %f %f" % (x, y, z)
		print "onClick g: %f %f %f" % (g, h, i)

	def commanderDestroyHandler (self, packet):
		# CB_COMMANDER_DESTROY = 0x0008, // Size: 11
		print 'Expected CB_COMMANDER_DESTROY. Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";

		# BC_COMMANDER_DESTROY = 0x0011, // Size: 7
		reply  = struct.pack("<H", PacketType.BC_COMMANDER_DESTROY);
		reply += struct.pack("<I", 0xFFFFFFFF);
		reply += struct.pack("<B", 0xFF); # 0xFF = Clear all characters

		self.nbCharacterBarrack = 0;
		self.positionCharacterList = 0;

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def singleInfoHandler (self, packet):
		# BC_SINGLE_INFO = 0x0013 // Size: 337

		charName = "Rioru";

		reply  = struct.pack("<H", PacketType.BC_SINGLE_INFO)
		reply += struct.pack("<I", 0); # UNKNOWN

		reply += charName + "\x00" * (65 - len(charName)); # Character Name

		reply += "1Ac2Ac3Ac4Ac5Ac6"
		reply += "Ac7A"
		reply += struct.pack("<H", 0x1); # jobClassId

		reply += "Ac9Ad0"
		itemsId = [0x00002710, 0x00099536, 0x00002710, 0x00081E91,  # Head 1   | Head 2     |    ?                               |  Armor
				   0x0007A959, 0x0007D071, 0x00002710, 0x000933E0,  # Gloves   | Boots      |    ? (makes the head disappear xD) |  Bracelet
				   0x00027D20, 0x00036511, 0x00098208, 0x00002710,  # Bow      | Shield     |   Costume                          |   ?
				   0x0007F782, 0x00099562, 0x0007F3A1, 0x00081AAC, 	# ?        |  ?         |   Pants                            |   ?
				   0x00002710, 0x00092C1A, 0x00092C1B, 0x0008DE0B];	# ?        | Ring left  |   Right right                      |  Necklace

		for itemId in itemsId: # Inventory : 20 items
			reply += struct.pack("<I", itemId);

		reply += "7Af8" + "Af9A";
		reply += "g0Ag" # startPosX - buffer
		reply += "1Ag2" # startPosY - buffer
		reply += "Ag3A" # startPosZ - buffer
		reply += "g4Ag5Ag6Ag7Ag8Ag9Ah0Ah1A"

		reply += "h2Ah3Ah4Ah5Ah6Ah7Ah8Ah9Ai0Ai1Ai2";

		reply += "F" * 4; # dword

		reply += "6Ag7Ag8Ag9Ah0Ah1Ah2Ah3Ah4Ah5Ah6" + "\x00"; # string 31B + zero
		reply += "C" * 4; # dword

		reply += "n3An4An5An6An7An8An9Ao0Ao1Ao2Ao3Ao4Ao5Ao" + "\x00"; # string 40B + zero
		reply += "E" # Byte

		self.sock.send (reply);
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def startGameHandler (self, packet):
		spriteID = 0;
		zoneServerDomainName = "127.0.0.1";
		zoneServerPort = 4919;

		# CB_START_GAME = 0x0009 // Size: 13
		print 'Expected CB_START_GAME. Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";

		# BC_START_GAMEOK = 0x0012 // Size: 60
		reply  = struct.pack("<H", PacketType.BC_START_GAMEOK);
		reply += struct.pack("<I", 0xFFFFFFFF); # UNKNOWN
		reply += struct.pack("<I", 0x12345678); # zone ID
		reply += zoneServerDomainName + "\x00" * (32 - len(zoneServerDomainName));
		reply += struct.pack("<I", zoneServerPort); # zoneServerPort
		reply += struct.pack("<I", 0x408); # mapId
		reply += struct.pack("<B", 1); # Channel ID
		reply += struct.pack("<I", spriteID); # Apparence du sprite du corps
		reply += struct.pack("<I", 2); # UNKNOWN - Something related with SpriteID (apparence related)
		reply += "\x00" # Boolean : StartSingleMap

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def logoutHandler (self, packet):
		# CB_LOGOUT = 0x0005 // Size: 10
		print 'Expected CB_LOGOUT. Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";
		reply  = struct.pack("<H", PacketType.BC_LOGOUTOK);
		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def netDecrypt (self, data):
		packetSize = struct.unpack("<H", data[:2]);
		# print "PacketSize received : %d" % packetSize;
		return data[2:];


	def extractPacketType (self, data):
		return struct.unpack("<H", data[:2])[0];


	def start (self):
		while True:
			data = self.sock.recv (self.PACKETSIZE_MAX);
			if (len(data) == 0):
				break; # Client exited

			packet = self.netDecrypt (data);
			packetType = self.extractPacketType (packet);

			# Packet handler
			if (packetType == PacketType.CB_LOGIN_BY_PASSPORT):
				self.loginByPasswordHandler (packet);

			elif (packetType == PacketType.CB_START_BARRACK):
				self.startBarrackHandler (packet);

			elif (packetType == PacketType.CB_CURRENT_BARRACK):
				self.currentBarrackHandler (packet);

			elif (packetType == PacketType.CB_BARRACKNAME_CHANGE):
				self.barrackNameChangeHandler (packet);

			elif (packetType == PacketType.CB_COMMANDER_CREATE):
				self.commanderCreateHandler (packet);

			elif (packetType == PacketType.CB_COMMANDER_DESTROY):
				self.commanderDestroyHandler (packet);

			elif (packetType == PacketType.CB_JUMP):
				self.jumpHandler (packet);

			elif (packetType == PacketType.CB_COMMANDER_MOVE):
				self.moveHandler (packet);

			elif (packetType == PacketType.CB_START_GAME):
				self.startGameHandler (packet);

			elif (packetType == PacketType.CB_LOGOUT):
				self.logoutHandler (packet);

			else:
				print "[WARNING] Unhandled packet type = 0x%x" % packetType;
				print 'Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";


	def __init__ (self, sock):
		self.sock = sock;
		self.nbCharacterBarrack = 0;
		self.positionCharacterList = 0;
		self.start ();
