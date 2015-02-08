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

		# ZC_CONNECT_OK = 0x0BBA, // Size: 0
		reply  = struct.pack("<H", PacketType.ZC_CONNECT_OK)
		reply += struct.pack("<I", 0); # UNKNOWN

		# reply += struct.pack("<H", X); # Size of the entire packet - Added dynamically at the end of the function
		reply += struct.pack("<B", 1); # Exit Barrack ?
		reply += struct.pack("<I", 0); # UNKNOWN
		reply += struct.pack("<B", 1); # CF sub_6AE590 - set dword_165DCC0

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
		reply += struct.pack("<I", 0); # UNKNOWN
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
