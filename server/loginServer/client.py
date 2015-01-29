# Dependencies
import binascii
import struct
import time
from PacketType import PacketType

# ==========================================================
class ClientHandler:
	# Configurations
	PACKETSIZE_MAX = 8192

	def loginOKHandler (self, sock):
		print "BC_LOGINOK Handler";
		data = sock.recv (self.PACKETSIZE_MAX)
		print 'CB_LOGIN_BY_PASSPORT expected. Received : ' + binascii.hexlify(data)
		# Size: 37
		reply  = struct.pack("<H", PacketType.BC_LOGINOK)
		reply += "\x00" * 35 # <-- Zéro ici pour mettre le compte en mode développeur
		sock.send (reply)
		print "Sent : " + binascii.hexlify (reply);

	def startBarrack (self, sock):
		data = sock.recv (self.PACKETSIZE_MAX)
		print 'CB_START_BARRACK expected. Received : ' + binascii.hexlify(data)

	def commanderListHandler (self, sock):
		print "BC_COMMANDER_LIST Handler";
		reply  = struct.pack("<H", PacketType.BC_COMMANDER_LIST)
		reply += "\x00" * 4; # Field 1
		# Le paquet peut contenir le nom de l'équipe si le joueur l'a déjà créée.
		reply += struct.pack("<H", 16);
		reply += "\x00" * 4 # Field 2
		reply += "\x00" * 4 # Field 3
		sock.send (reply);
		print "Sent : " + binascii.hexlify (reply);

	def currentBarrackHandler (self, sock):
		print "CURRENT_BARRACK Handler";
		data = sock.recv (self.PACKETSIZE_MAX) # CURRENT_BARRACK
		print 'Expected CURRENT_BARRACK. Received : ' + binascii.hexlify(data)

	def barrackNameChangeHandler (self, sock):
		print "BC_BARRACKNAME_CHANGE Handler";
		data = sock.recv (self.PACKETSIZE_MAX) # CB_BARRACKNAME_CHANGE
		print 'Expected CB_BARRACKNAME_CHANGE. Received : ' + binascii.hexlify(data)
		barrackName = "Spl3en"
		reply  = struct.pack("<H", PacketType.BC_BARRACKNAME_CHANGE)
		reply += "\x01" * 9;
		reply += barrackName + "\x00";
		reply += "\x00" * (63 - len(barrackName));
		sock.send (reply)
		print "Sent : " + binascii.hexlify (reply);

	def commanderCreateHandler (self, sock):
		nbCharacterBarrack = 0;
		positionCharacterList = 0;

		while True:
			print "BC_COMMANDER_CREATE Handler";
			data = sock.recv (self.PACKETSIZE_MAX) # CB_BARRACKNAME_CHANGE
			print 'Expected CB_COMMANDER_CREATE. Received : ' + binascii.hexlify(data)

			# Size: 318
			# Configs
			nbCharacterBarrack += 1;
			positionCharacterList += 1;
			charName = "Rioru";
			rightClick = "I'm displayed by a right click on the character";
			classId = 10003;
			jobId = 3;
			gender = 1;
			mapId = 0xD34;
			characterLevel = 1337;
			# 0x2710 = empty item (? or maybe it's just bugged)
			itemsId = [0x9d89d, 0x9d89e, 0x9d89f, 0x9d8a0, 0x9d8a1, 0x2710, 0x2710, 0x2710, 0x9d8a5, 0x9d8a6, 0x9d8a7, 0x9d8a8, 0x9d8a9, 0x9d8aa, 0x9d8ab, 0x9d8ac, 0x9d8ad, 0x9d8ae, 0x9d8af, 0x9d8b0];
			hairId = 3;
			spriteID = 0; # max 18

			# Packet
			reply  = struct.pack("<H", PacketType.BC_COMMANDER_CREATE);

			reply += struct.pack("<I", 0xFFFFFFFF); # UNKNOWN

			reply += charName; # Character Name
			reply += "\x00" * (65 - len(charName));

			reply += rightClick; # Right click description, not sure what it's for
			reply += "\x00" * (65 - len(rightClick));

			reply += struct.pack("<B", 0xFF) * 14; # UNKNOWN

			reply += struct.pack("<H", classId) # class id

			reply += struct.pack("<H", 0xFF) # UNKNOWN

			reply += struct.pack("<H", jobId) # job id
			reply += struct.pack("<B", gender) # Gender

			reply += struct.pack("<B", 1) # UNKNOWN

			reply += struct.pack("<I", characterLevel); # Character level
			for itemId in itemsId: # Inventory : 20 items
				reply += struct.pack("<I", itemId); # items

			reply += struct.pack("<B", hairId); # Hairstyle

			reply += struct.pack("<B", 0xFF) * 3; # UNKNOWN
			reply += struct.pack("<I", 0xFF); # UNKNOWN
			reply += struct.pack("<I", positionCharacterList) # Position in the character list
			reply += struct.pack("<B", nbCharacterBarrack) # Character position in the character list

			reply += struct.pack("<B", 0xFF) # UNKNOWN

			reply += struct.pack("<H", mapId) # mapID

			reply += struct.pack("<I", 0xFF) * 3; # UNKNOWN
			reply += struct.pack("<I", spriteID) * 1; # Apparence du sprite du corps
			reply += struct.pack("<I", 0xFF) * 1; # UNKNOWN
			reply += struct.pack("<I", 0xFF) * 2; # UNKNOWN
			reply += struct.pack("<I", 0xFF) * 8; # UNKNOWN

			sock.send (reply)
			print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def start (self, sock):
		self.loginOKHandler (sock);
		self.startBarrack (sock);
		self.commanderListHandler (sock);
		self.currentBarrackHandler (sock);
		self.barrackNameChangeHandler (sock);
		self.commanderCreateHandler (sock);

	def __init__ (self, sock):
		self.sock = sock;
		self.start (sock);
