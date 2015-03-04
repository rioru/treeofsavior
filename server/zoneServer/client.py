# Dependencies
import binascii
import struct
import time
import socket
import io
import filetime;
from PacketType import PacketType

def stream_unpack (fmt, stream):
    size = struct.calcsize(fmt)
    buf = stream.read(size)
    return struct.unpack(fmt, buf)[0];

# ==========================================================
class ClientHandler:

	# Configurations
	PACKETSIZE_MAX = 8192

	# Globals
	X = 0
	Y = 0
	Z = 0

	def connectHandler (self, packet):
		# CZ_CONNECT = 0x0BB9, // Size: 54
		print 'CZ_CONNECT expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

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


	def quickSlotListHandler (self, packet):
		# ZC_QUICK_SLOT_LIST = 0x0C31, // Size: 0
		reply  = struct.pack("<H", PacketType.ZC_QUICK_SLOT_LIST)
		reply += struct.pack("<I", 0); # UNKNOWN

		reply += struct.pack("<I", 0); # 0 or zlib deflated data

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def uiInfoList (self, packet):
		# ZC_UI_INFO_LIST = 0x0CCC, // Size: 0
		uiInfoListSize = 1
		unkUiListSize  = 1
		reply  = struct.pack("<H", PacketType.ZC_UI_INFO_LIST)
		reply += struct.pack("<I", 0); # UNKNOWN

		reply += struct.pack("<I", uiInfoListSize); # nb list
		reply += struct.pack("<I", unkUiListSize); # UNKNOWN for loop after

		# UI_FRAME_INFO, Size: 48
		frameTitle = "chat";
		for i in range(0, uiInfoListSize):
			reply += struct.pack("<H", (i*2)); # groupID
			reply += struct.pack("<H", (i*2)+1); # uiIndex
			reply += struct.pack("<H", 100); # x
			reply += struct.pack("<H", 100); # y
			reply += struct.pack("<H", 500); # width
			reply += struct.pack("<H", 500); # height
			reply += frameTitle + "\x00" * (32 - len(frameTitle));
			reply += struct.pack("<H", 0xAAAA); # numArg1
			reply += struct.pack("<H", 0xBBBB); # numArg2

		# Unknown vector 2
		for i in range(0, unkUiListSize):
			reply += struct.pack("<H", (i*2));   # id_1
			reply += struct.pack("<H", (i*2)+1); # id_2

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];

		self.sock.send (reply)
		print "[ZC_UI_INFO_LIST] Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";


	def MyPCEnter (self, packet):
		# ZC_MYPC_ENTER = 0x0CA4, // Size: 18
		reply  = struct.pack("<H", PacketType.ZC_MYPC_ENTER)
		reply += struct.pack("<I", 0); # UNKNOWN
		
		reply += struct.pack("<I", 0xFF); # PCID
		reply += struct.pack("<I", 0); # UNKNOWN - FSMActor.field_87
		reply += struct.pack("<I", 0); # UNKNOWN - FSMActor.field_88

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def startInfo (self, packet):
		# This packet contains only information about JOB_INFO
		count_JOB_INFO = 10;

		# ZC_START_INFO = 0x0D0E                       # Size: 0
		reply  = struct.pack("<H", PacketType.ZC_START_INFO)
		reply += struct.pack("<I", 0); # UNKNOWN

		reply += struct.pack("<I", count_JOB_INFO); # count_JOB_INFO

		for i in range (0, count_JOB_INFO): # 12 bytes each - information about JOB_INFO
			reply += struct.pack("<H", 0xAAAA - i); # field_0
			reply += struct.pack("<H", 0xEEEE - i); # unk2
			reply += struct.pack("<I", 0xDDDDDDDD - i); # field_4
			reply += struct.pack("<I", 0xCCCC - i); # field_8
			reply += struct.pack("<I", 0xBBBB - i); # field_A

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];

		self.sock.send (reply)
		print "[ZC_START_INFO] Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def message (self, packet):
		messageCode = 0x42; # CF packet::ServerMsg (0x4DA330) for the message codes list

		# ZC_MESSAGE = 0x0C03                          # Size: 30
		reply  = struct.pack("<H", PacketType.ZC_MESSAGE)
		reply += struct.pack("<I", 0); # UNKNOWN

		reply += struct.pack("<B", messageCode);

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];

		self.sock.send (reply)
		print "[ZC_MESSAGE] Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def gameReadyHandler (self, packet):
		# CZ_GAME_READY = 0x0BFD, // Size: 10
		print 'CZ_GAME_READY expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		# Add additional information
		self.quickSlotListHandler (packet);
		self.uiInfoList (packet);
		self.startInfo (packet);

		# ZC_START_GAME = 0x0C05, // Size: 26
		reply  = struct.pack("<H", PacketType.ZC_START_GAME)
		reply += struct.pack("<I", 1); # UNKNOWN

		reply += struct.pack("<f", 1.0); # Time multiplier - Not sure what it does
		reply += struct.pack("<f", 0.0); # serverAppTimeOffset
		reply += struct.pack("<f", 0.0); # globalAppTimeOffset
		reply += struct.pack("<d", filetime.filetime_now()); # serverDateTime

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

		# ===== The game starts from this point =====
		self.moveSpeed(packet);
		self.MyPCEnter(packet);
		self.setPos(packet, 1142.29, 1000, -32.42);

	def restSitHandler (self, packet):
		# CZ_REST_SIT = 0x0C0E, Size: 10
		print 'CZ_REST_SIT expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		# ZC_REST_SIT = 0x0BCB, Size: 11
		reply  = struct.pack("<H", PacketType.ZC_REST_SIT);
		reply += struct.pack("<I", 0);
		reply += struct.pack("<I", 0xFF); # PCID
		reply += struct.pack("<B", 1);
		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";
		# For fun, remove it when you want
		self.uiOpen(packet, "chat", 1); # Chat input
		self.uiOpen(packet, "chatframe", 1); # Chat output

	def movementInfoHandler (self, packet):
		# CZ_MOVEMENT_INFO = 0x0C11                           # Size: 23
		print 'CZ_MOVEMENT_INFO expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		packetType, sequenceNumber, checksum, unk1, unk2, x, y, zlowWord = struct.unpack("<HIHH?ffH", packet)
		# There is only 2 bytes received for Z axis instead of 4. Really weird.
		print "Position : x:%.2f | y:%.2f" % (x, y)

	def logoutHandler (self, packet):
		# CZ_LOGOUT = 0x0BFF                           # Size: 10
		print 'CZ_LOGOUT expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		# ZC_LOGOUT_OK = 0x0C02                        # Size: 6
		reply  = struct.pack("<H", PacketType.ZC_LOGOUT_OK);
		reply += struct.pack("<I", 0xFFFFFFFF); # UNKNOWN

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def keyboardMoveHandler (self, packet):
		# CZ_KEYBOARD_MOVE = 0x0C08                    # Size: 41
		print 'CZ_KEYBOARD_MOVE expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		packetType, sequenceNumber, checksum, unk1, unk2, x, y, z, dirX, dirZ, unk3, unk4 = struct.unpack("<HIHH?fffffII", packet)
		print "Position : x:%.2f | y:%.2f | z:%.2f | dirX:%.2f | dirZ:%.2f" % (x, y, z, dirX, dirZ)

	def moveStopHandler (self, packet):
		# CZ_MOVE_STOP = 0x0C0D                        # Size: 35
		# 0d0c 2c000000 8c04 0000 00 c73a70c2 6a1f0243 14fb33c1 ffff7fbf 00000000 331d
		# 0d0c 2f000000 9a04 0000 00 a10b18c3 6a1f0243 0d4db6c1 ffff7fbf 00000000 987f
		# 0d0c 33000000 7c06 0000 00 31a1a6c3 6a1f0243 6ce43442 ffff7fbf 00000000 2504
		# 0d0c 33000000 7c06 0000 00 31a1a6c3 6a1f0243 6ce43442 ffff7fbf 00000000 2504

		"""
		The *first* CZ_MOVE_STOP sent to the server is 78 or 80 bytes long :
		CZ_MOVE_STOP expected. Received :
		  0d0c 03000000 1000 0000 00 12000000 00000000 00000000 0000803f 00000000 56f8
		                          0a 00100c04 00000018 0023000d 0c050000 00260000 0000
								     12000000 00000000 00000000 0000803f 00000000 56f8 (78)
		"""
		print 'CZ_MOVE_STOP expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		if (len(packet) == 33):
			packetType, sequenceNumber, checksum, unk1, unk2, x, y, z, dirX, dirZ, unk3 = struct.unpack("<HIHH?fffffH", packet)
			print "[CZ_MOVE_STOP] x:%.2f | y:%.2f | z:%.2f | dirX:%.2f | dirZ:%.2f | unk=%x" % (x, y, z, dirX, dirZ, unk3)
			self.X = float("%.2f" % x)
			self.Y = float("%.2f" % y)
			self.Z = float("%.2f" % z)

			# ZC_MOVE_STOP = 0x0BCA # Size: 23
			# If the PCID in this packet is the same than the PCID of the player, this packet is ignored by the client.
			reply  = struct.pack("<H", PacketType.ZC_MOVE_STOP);
			reply += struct.pack("<I", 0xFFFFFFFF); # UNKNOWN
			reply += struct.pack("<I", 0x000000FF); # PCID
			reply += struct.pack("<f", x); # x
			reply += struct.pack("<f", y); # y
			reply += struct.pack("<f", z); # z
			reply += struct.pack("<B", 1); # UNKNOWN

			self.sock.send (reply)
			print "[ZC_MOVE_STOP] Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def moveSpeed (self, packet):
		# ZC_MOVE_SPEED = 0x0BC9, // Size: 18
		reply  = struct.pack("<H", PacketType.ZC_MOVE_SPEED);
		reply += struct.pack("<I", 0);

		reply += struct.pack("<I", 0xFF); # PCID
		reply += struct.pack("<f", 100); # Movement Speed
		reply += struct.pack("<f", 0.1); # UNKNOWN

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def poseHandler (self, packet):
		# CZ_POSE = 0x0C1B, // Size: 34
		print 'CZ_POSE expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		packetType, sequenceNumber, checksum, unk1, pose = struct.unpack("<HIHHI", packet[:14])
		position = packet[14:26] # 12
		direction = packet[26:] # 8
		dir1, dir2 = struct.unpack("<Hf", direction)

		# ZC_POSE = 0x0BEE, // Size: 34
		reply  = struct.pack("<H", PacketType.ZC_POSE);
		reply += struct.pack("<I", 0);

		reply += struct.pack("<I", 0xFF); # PCID
		reply += struct.pack("<I", pose); # Pose
		reply += position; # Position (x, y, z)
		# Direction needs to be fixed
		reply += struct.pack("<f", dir1);
		reply += struct.pack("<f", dir2);

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def jumpHandler (self,packet):
		# CZ_JUMP = 0x0C0A, // Size: 11
		print 'CZ_JUMP expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		# ZC_JUMP = 0x0BCC, // Size: 19
		reply  = struct.pack("<H", PacketType.ZC_JUMP);
		reply += struct.pack("<I", 0);

		reply += struct.pack("<I", 0xFF); # PCID
		reply += struct.pack("<f", 300); # Height
		reply += struct.pack("<I", 1); # UNKNOWN
		reply += struct.pack("<B", 1); # Character Id Jumping

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def onAirHandler (self, packet):
		# CZ_ON_AIR = 0x0C0F                           # Size: 10
		""" <packetType> <packetSequence> <checksum>
		0f0c 3b000000 2000
		0f0c 3f000000 2400
		0f0c 43000000 5800
		"""
		# print 'CZ_ON_AIR expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		return;

	def onGroundHandler (self, packet):
		# CZ_ON_GROUND = 0x0C10                        # Size: 10
		""" <packetType> <packetSequence> <checksum>
		100c 3c000000 2000
		100c 40000000 5c00
		100c 44000000 5800
		"""
		# print 'CZ_ON_GROUND expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		return;

	def setPos(self, packet, x, y, z):
		# ZC_SET_POS = 0x0BD0, // Size: 22
		reply  = struct.pack("<H", PacketType.ZC_SET_POS);
		reply += struct.pack("<I", 0);
		reply += struct.pack("<I", 0xFF); # PCID

		reply += struct.pack("<f", x);
		reply += struct.pack("<f", y);
		reply += struct.pack("<f", z);
		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def moveBarrackHandler(self, packet):
		# CZ_MOVE_BARRACK = 0x0C00, // Size: 10
		print 'CZ_MOVE_BARRACK expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		# ZC_MOVE_BARRACK = 0x0C01, // Size: 6
		reply  = struct.pack("<H", PacketType.ZC_MOVE_BARRACK);
		reply += struct.pack("<I", 0);

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def rotateHandler(self, packet):
		# CZ_ROTATE = 0x0C18, // Size: 18
		print 'CZ_ROTATE expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		if len(packet) == 16:
			packetType, sequenceNumber, checksum, x, z = struct.unpack("<HIHII", packet)
		else:
			return
		print "Rotate : dirX:%f | dirZ:%f" % (x, z)
		#180c b1000000 8602 0000f104 35bff204

		# ZC_ROTATE = 0x0BE9, // Size: 19
		reply  = struct.pack("<H", PacketType.ZC_ROTATE);
		reply += struct.pack("<I", 0);

		reply += struct.pack("<I", 0xFF); # PCID
		reply += struct.pack("<f", x); # x
		reply += struct.pack("<f", z); # z
		reply += struct.pack("<B", 0); # UNKNOWN

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def changeCamera(self, packet, option, x = 0, y = 0, z = 0, fspd = 0, ispd = 0):
		# ZC_CHANGE_CAMERA = 0x0C97, // Size: 31
		# changeCameraSub(*(_BYTE *)(a1 + 6), a1 + 7, a1 + 11, *(float *)(a1 + 23), *(float *)(a1 + 27));
		reply  = struct.pack("<H", PacketType.ZC_CHANGE_CAMERA);
		reply += struct.pack("<I", 0);

		reply += struct.pack("<B", option); # option 0, 1 or 2 | 0 = reset, 1, 2 = set then update ?

		reply += struct.pack("<I", 0); # unk only used when option == 2
		reply += struct.pack("<f", x); # x - Positions are only used with option 1
		reply += struct.pack("<f", y); # y
		reply += struct.pack("<f", z); # z
		reply += struct.pack("<f", fspd); # Final camera speed
		reply += struct.pack("<f", ispd); # Initial camera speed
		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def uiOpen(self, packet, ui, open):
		# ZC_UI_OPEN = 0x0C95, // Size: 39
		reply  = struct.pack("<H", PacketType.ZC_UI_OPEN);
		reply += struct.pack("<I", 0);

		reply += ui; # Max 32
		reply += "\x00" * (32 - len(ui));
		reply += struct.pack("<B", open); # 1 or 0, enable or disable
		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

		
	def normalHeader (self, type):
		# Handler at 0x702FA0
		# ZC_NORMAL = 0x0D27, // Size: 0
		reply  = struct.pack("<H", PacketType.ZC_NORMAL)
		reply += struct.pack("<I", 0) # seqnum
		reply += struct.pack("<I", type) # type
		
		# data after that
		return reply;

	def reqChannelTraffics (self, packet):
		# CZ_REQ_CHANNEL_TRAFFICS = 0x0C95, // Size: 10
		print 'CZ_REQ_CHANNEL_TRAFFICS expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		
		mapId = 0x551;
		nbZoneServers = 10;
		
		reply = self.normalHeader (0xF8);
		reply += struct.pack("<H", 0); # zlib header (if egal to 0xFA8D, it means the following packet is compressed)
		reply += struct.pack("<H", mapId); # mapID
		reply += struct.pack("<H", nbZoneServers); # Count of zone servers for this map
		
		# Zone traffic structure
		for j in range(0, nbZoneServers):
			reply += struct.pack("<B", j); # List ID the zone server
			reply += struct.pack("<H", j * 10); # Zone server current players count

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];
		
		self.sock.send (reply)
		print "[CZ_REQ_CHANNEL_TRAFFICS] Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

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
			if (packetType == PacketType.CZ_CONNECT):
				self.connectHandler (packet);

			elif (packetType == PacketType.CZ_LOGOUT):
				self.logoutHandler (packet);

			elif (packetType == PacketType.CZ_GAME_READY):
				self.gameReadyHandler (packet);

			elif (packetType == PacketType.CZ_KEYBOARD_MOVE):
				self.keyboardMoveHandler (packet);

			elif (packetType == PacketType.CZ_MOVE_STOP):
				self.moveStopHandler (packet);

			elif (packetType == PacketType.CZ_REST_SIT):
				self.restSitHandler (packet);

			elif (packetType == PacketType.CZ_MOVEMENT_INFO):
				self.movementInfoHandler (packet);

			elif (packetType == PacketType.CZ_POSE):
				self.poseHandler (packet);

			elif (packetType == PacketType.CZ_JUMP):
					self.jumpHandler (packet);

			elif (packetType == PacketType.CZ_ON_AIR):
				self.onAirHandler (packet);

			elif (packetType == PacketType.CZ_ON_GROUND):
				self.onGroundHandler (packet);

			elif (packetType == PacketType.CZ_MOVE_BARRACK):
				self.moveBarrackHandler (packet);

			elif (packetType == PacketType.CZ_ROTATE):
				self.rotateHandler (packet);
				
			elif (packetType == PacketType.CZ_REQ_CHANNEL_TRAFFICS):
				self.reqChannelTraffics (packet);

			else:
				print "[WARNING] Unhandled packet type = 0x%x (size=%d)" % (packetType, len(data));
				# print 'Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";


	def __init__ (self, sock):
		self.sock = sock;
		self.start ();
