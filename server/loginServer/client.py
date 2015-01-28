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

		while True:
			print "BC_COMMANDER_CREATE Handler";
			data = sock.recv (self.PACKETSIZE_MAX) # CB_BARRACKNAME_CHANGE
			print 'Expected CB_COMMANDER_CREATE. Received : ' + binascii.hexlify(data)
			
			# Size: 318
			# Configs
			nbCharacterBarrack += 1;
			classId = 0x2711;
			jobId = 2;
			gender = 1;
			mapId = 0xD34;
			characterLevel = 1337;
			# 0x2710 = empty item (? or maybe it's just bugged)
			itemsID = [0x9d89d, 0x9d89e, 0x9d89f, 0x9d8a0, 0x9d8a1, 0x2710, 0x2710, 0x2710, 0x9d8a5, 0x9d8a6, 0x9d8a7, 0x9d8a8, 0x9d8a9, 0x9d8aa, 0x9d8ab, 0x9d8ac, 0x9d8ad, 0x9d8ae, 0x9d8af, 0x9d8b0];
			spriteID = 2;
			
			# Packet
			reply  = struct.pack("<H", PacketType.BC_COMMANDER_CREATE);
			reply += struct.pack("<I", 0xFF) * 37; # UNKNOWN
			
			reply += struct.pack("<H", classId) # class id
			
			reply += struct.pack("<H", 0xFF) # UNKNOWN
			
			reply += struct.pack("<H", jobId) # job id
			reply += struct.pack("<B", gender) # Gender
			
			reply += struct.pack("<B", 1) # UNKNOWN
			
			reply += struct.pack("<I", characterLevel); # Character level
			
			for itemID in itemsID: # Inventory : 20 items
				reply += struct.pack("<I", itemID); # items
			
			reply += struct.pack("<Q", 0xFF); # UNKNOWN
			reply += struct.pack("<I", 0xFF) # UNKNOWN
			
			reply += struct.pack("<B", nbCharacterBarrack) # Character position in the character list
			
			reply += struct.pack("<B", 0xFF) # UNKNOWN
			
			reply += struct.pack("<H", mapId) # mapID
			
			reply += struct.pack("<I", 0xFF) * 3; # UNKNOWN
			reply += struct.pack("<I", spriteID) * 1; # Apparence du sprite du corps
			reply += struct.pack("<I", 0xFF) * 1; # UNKNOWN
			reply += struct.pack("<I", 0xFF) * 2; # UNKNOWN
			reply += struct.pack("<I", 0xFF) * 8; # UNKNOWN
			
			sock.send (reply)
			print "Sent : " + binascii.hexlify (reply);
			

	"""
	def iesModifyListHandler (self, sock):
		time.sleep(3);
		print "BC_IES_MODIFY_LIST Handler";
		reply  = struct.pack("<H", PacketType.BC_IES_MODIFY_LIST)
		reply += "AAAA"; # Unknown
		reply += struct.pack("<H", 84);
		reply += "B" * 82
		sock.send (reply);
		print "Sent : " + binascii.hexlify (reply);
		
	def AccountPropHandler (self, sock):
		time.sleep(3);
		print "BC_ACCOUNT_PROP Handler";
		reply  = struct.pack("<H", PacketType.BC_ACCOUNT_PROP)
		reply += "AAAA"; # Unknown
		reply += struct.pack("<H", 84);
		reply += "B" * 82
		sock.send (reply);
		print "Sent : " + binascii.hexlify (reply);
		
	def singleInfoHandler (self, sock):
		time.sleep(3);
		print "BC_SINGLE_INFO";
		# Size: 337
		reply  = struct.pack("<H", PacketType.BC_SINGLE_INFO)
		reply += "A" * 335
		sock.send (reply)
		print "Sent : " + binascii.hexlify (reply);
		
	def echoHandler (self, sock):
		time.sleep(3);
		print "BC_ECHO";
		# Size: 26
		reply  = struct.pack("<H", PacketType.BC_ECHO)
		reply += "A" * 24
		sock.send (reply)
		print "Sent : " + binascii.hexlify (reply);
	"""

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