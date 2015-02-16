# Dependencies
import binascii
import struct
import time
import socket
import io
from PacketType import PacketType

# ==========================================================
class ClientHandler:
	# Configurations
	PACKETSIZE_MAX = 8192

	def LoginHandler (self, packet):
		# CS_LOGIN = 0x0B55, Size: 48
		# 550b 00000000 4368616e6e656c4c6f67696e0000000000 d41d8cd98f00b204e9800998ecf8427e00 8877665544332211
		print 'CS_LOGIN expected. Received : ' + binascii.hexlify (packet) + " (" + str(len(packet)) + ")";

		packetType, sequenceNumber, plaintext, md5, CID = struct.unpack("<HI17s17sq", packet)
		
		print "plaintext = " + plaintext;
		print "md5(plain) = " + binascii.hexlify (md5[:16]);
		print "CID = 0x%x" % CID;

		# Don't know what to answer
		self.unkFunc(packet)
		self.groupChat(packet)

	def normalHeader(self, type):
		# Handler at 0087B5F0
		# SC_NORMAL = 0x0B56, // Size: 0
		reply  = struct.pack("<H", PacketType.SC_NORMAL)
		reply += struct.pack("<I", 0) # seqnum
		reply += struct.pack("<H", 0) # checksum?
		reply += struct.pack("<I", type) # type
		# data after that
		return reply;

	def unkFunc(self, packet):
		reply  = self.normalHeader(2);
		reply += struct.pack("<I", 1)
		reply += struct.pack("<I", 1)
		reply += struct.pack("<I", 1)
		reply += struct.pack("<I", 1)

		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def groupChat(self, packet):
		reply  = self.normalHeader(5);

		# v1
		reply += struct.pack("<B", 1)
		reply += struct.pack("<I", 1)
		reply += struct.pack("<B", 1) * 3

		# v2
		reply += struct.pack("<B", 1)
		reply += struct.pack("<I", 1)
		reply += struct.pack("<B", 1) * 3
		reply += struct.pack("<I", 1)

		# unk
		reply += struct.pack("<B", 1) * 124

		reply += "testhello"


		self.sock.send (reply)
		print "Sent : " + binascii.hexlify (reply) + " (" + str(len(reply)) + ")";

	def extractPacketType (self, data):
		return struct.unpack("<H", data[:2])[0];

	def start (self):
		while True:
			packet = self.sock.recv (self.PACKETSIZE_MAX)
			if (len(packet) == 0):
				break; # Client exited

			packetType = self.extractPacketType (packet);

			# Packet handler
			if (packetType == PacketType.CS_LOGIN):
				self.LoginHandler (packet);

			else:
				print "[WARNING] Unhandled packet type = 0x%x" % packetType;
				print 'Received : ' + binascii.hexlify(packet) + " (" + str(len(packet)) + ")";

	def __init__ (self, sock):
		self.sock = sock;
		self.start ();
