# Dependencies
import binascii
import struct
import time
import socket
from PacketType import PacketType

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
		self.commanderListHandler (packet);


	def serverEntryHandler (self, packet):
		# BC_SERVER_ENTRY = 0x0057, // Size: 18
		channelIP = "127.0.0.1";
		channelPort1 = 1337;
		channelPort2 = 1338;

		reply  = struct.pack("<H", PacketType.BC_SERVER_ENTRY)
		reply += struct.pack("<I", 0); # UNKNOWN

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

			# Packet
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
			
			reply += struct.pack("<I",0x000000FF); # PCID - Still need to understand how it works

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

		self.sock.send (reply);
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def currentBarrackHandler (self, packet):
		# CB_CURRENT_BARRACK = 0x004E, // Size: 39
		print 'CB_CURRENT_BARRACK expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

	def barrackNameChangeHandler (self, packet):
		# CB_BARRACKNAME_CHANGE = 0x000A, // Size: 74
		print 'CB_BARRACKNAME_CHANGE expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";
		
		# BC_BARRACKNAME_CHANGE = 0x0017, // Size: 75
		barrackName = "Spl3en"
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

		# Packet
		reply  = struct.pack("<H", PacketType.BC_COMMANDER_CREATE);
		reply += struct.pack("<I", 0xFFFFFFFF); # UNKNOWN
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
		
		reply += struct.pack("<I",0x000000FF); # PCID - Still need to understand how it works

		reply += struct.pack("<I", self.positionCharacterList) # Position in the character list
		reply += struct.pack("<B", self.nbCharacterBarrack) # Character position in the character list
		reply += struct.pack("<B", 0) # UNKNOWN
		reply += struct.pack("<H", mapId) # mapID
		reply += struct.pack("<I", 0) * 3; # UNKNOWN
		reply += struct.pack("<I", spriteID) * 1; # Apparence du sprite du corps
		reply += struct.pack("<I", 0) * 1; # UNKNOWN
		reply += struct.pack("<I", 0) * 2; # UNKNOWN
		reply += struct.pack("<I", 0) * 8; # UNKNOWN
		
		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

			
	def netDecrypt (self, data):
		# TODO
		packetSize = struct.unpack("<H", data[:2])[0];
		print "PacketSize received : %d" % packetSize;
		packet = data[2:];
		return packet;

		
	def extractPacketType (self, data):
		return struct.unpack("<H", data[:2])[0];

		
	def start (self):
		while True:
			data = self.sock.recv (self.PACKETSIZE_MAX)
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
				
			else:
				print "[WARNING] Unhandled packet type = 0x%x" % packetType;
			

	def __init__ (self, sock):
		self.sock = sock;
		self.start ();
