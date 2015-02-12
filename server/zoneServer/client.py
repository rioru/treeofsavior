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

	def connectHandler (self, packet):
		# CZ_CONNECT = 0x0BB9, // Size: 54
		print 'CZ_CONNECT expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		PCID = 0xFF;
		
		# ZC_CONNECT_OK = 0x0BBA, // Size: 0
		reply  = struct.pack("<H", PacketType.ZC_CONNECT_OK)
		reply += struct.pack("<I", 0); # UNKNOWN

		# reply += struct.pack("<H", X); # Size of the entire packet - Added dynamically at the end of the function
		reply += struct.pack("<B", 1); # Exit Barrack ?
		reply += struct.pack("<I", 1); # UNKNOWN
		reply += struct.pack("<B", 1); # UNKNOWN - CF sub_6AE590 - set dword_165DCC0

		# 10 bytes = UNKNOWN
		reply += "Aa0Aa1Aa2A"

		# Pc session
		reply += struct.pack("<I", PCID); # PCID - Define a schrageID for the current PC - must *not* be zero
		reply += "4Aa5"
		
		# CommanderCreatePacket
		self.nbCharacterBarrack = 1;
		self.positionCharacterList = 1;
		charName = "Rioru";
		rightClick = "I'm displayed by a right click on the character";
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
		reply += rightClick; # Right click description, not sure what it's for
		reply += "\x00" * (65 - len(rightClick));
		reply += struct.pack("<B", 0) * 14; # UNKNOWN
		reply += struct.pack("<H", classId) # class id
		reply += struct.pack("<H", 0) # UNKNOWN
		reply += struct.pack("<H", jobId) # job id
		reply += struct.pack("<B", gender) # Gender
		reply += struct.pack("<B", 1) # UNKNOWN
		reply += struct.pack("<I", characterLevel); # Character level
		for itemId in itemsId: # Inventory : 20 items
			reply += struct.pack("<I", itemId); # items
		reply += struct.pack("<B", hairId); # Hairstyle
		reply += struct.pack("<B",0x00) * 3; # UNKNOWN
		reply += struct.pack("<I", PCID); # PCID - Still need to understand how it works
		reply += struct.pack("<I", self.positionCharacterList) # Position in the character list
		reply += struct.pack("<B", self.nbCharacterBarrack) # Character position in the character list
		reply += struct.pack("<B", 0) # UNKNOWN
		reply += struct.pack("<H", mapId) # mapID
		reply += struct.pack("<I", 0) * 3; # UNKNOWN
		reply += struct.pack("<I", spriteID) * 1; # Apparence du sprite du corps
		reply += struct.pack("<I", 0) * 1; # UNKNOWN
		reply += struct.pack("<I", 0) * 2; # UNKNOWN
		reply += struct.pack("<I", 0) * 8; # UNKNOWN
		

		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def gameReadyHandler (self, packet):
		# CZ_GAME_READY = 0x0BFD, // Size: 10
		print 'CZ_GAME_READY expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		# ZC_START_GAME = 0x0C05, // Size: 26
		reply  = struct.pack("<H", PacketType.ZC_START_GAME)
		reply += struct.pack("<I", 1); # UNKNOWN
		reply += struct.pack("<B", 1) * 20; # UNKNOWN
		
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
			if (packetType == PacketType.CZ_CONNECT):
				self.connectHandler (packet);

			elif (packetType == PacketType.CZ_GAME_READY):
				self.gameReadyHandler (packet);

			else:
				print "[WARNING] Unhandled packet type = 0x%x" % packetType;
				print 'Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";


	def __init__ (self, sock):
		self.sock = sock;
		self.start ();
