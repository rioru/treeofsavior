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

	def commanderListHandler (self, sock):
		print "BC_COMMANDER_LIST Handler";
		data = sock.recv (self.PACKETSIZE_MAX)
		print 'CB_START_BARRACK expected. Received : ' + binascii.hexlify(data)
		reply  = struct.pack("<H", PacketType.BC_COMMANDER_LIST)
		reply += "B" * 4; # Unknown
		# Le paquet peut contenir le nom de l'équipe si le joueur l'a déjà créée.
		reply += struct.pack("<H", 16);
		reply += "\x00" * 4 # Field 1
		reply += "\x00" * 4 # Field 2
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
		reply += "B" * 9;
		reply += barrackName + "\x00";
		reply += "C" * (63 - len(barrackName));
		sock.send (reply)
		print "Sent : " + binascii.hexlify (reply);
	
	def commanderCreateHandler (self, sock):
		while True:
			print "BC_COMMANDER_CREATE Handler";
			data = sock.recv (self.PACKETSIZE_MAX) # CB_BARRACKNAME_CHANGE
			print 'Expected CB_COMMANDER_CREATE. Received : ' + binascii.hexlify(data)
			# Size: 318
			reply  = struct.pack("<H", PacketType.BC_COMMANDER_CREATE);
			reply += "\x01"*316
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
		self.commanderListHandler (sock);
		self.currentBarrackHandler (sock);
		self.barrackNameChangeHandler (sock);
		self.commanderCreateHandler (sock);
		
	def __init__ (self, sock):
		self.sock = sock;
		self.start (sock);