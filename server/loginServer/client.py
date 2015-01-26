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
			classId = 4;
			jobId = 4;
			gender = 1;
			
			# Packet
			reply  = struct.pack("<H", PacketType.BC_COMMANDER_CREATE);
			reply += "\xFF"*4 # Field 1
			reply += "\xFF"*144
			reply += struct.pack("<H", classId) # class id
			reply += "\x01"*2
			reply += struct.pack("<H", jobId) # job id
			reply += struct.pack("<B", gender) # Gender
			reply += "\x04"*1
			reply += "\x01"*4
			reply += struct.pack("<I", 2) * 20; # items
			reply += struct.pack("<Q", 1); # quad sprintf (00452885)
			reply += "\x01"*4
			reply += struct.pack("<B", nbCharacterBarrack) # Character position in the character list
			reply += "\x01"
			reply += struct.pack("<H", 4) #sysbarrack related
			reply += "\x01"*60
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