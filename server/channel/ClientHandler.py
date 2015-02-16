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
    return struct.unpack(fmt, buf)

# ==========================================================
class ClientHandler:
	# Configurations
	PACKETSIZE_MAX = 8192

	def LoginHandler (self, packetStr):
		# CS_LOGIN = 0x0B55, Size: 48
		print 'CS_LOGIN expected. Received : ' + binascii.hexlify (packetStr) + " (" + str(len(packetStr)) + ")";

		"""
			struct CSLoginPacket {
			  WORD type;			## 0b55
			  __int16 unk1;			## 0000
			  char plaintext[17];	## 0000000000000000000000000000000000
			  DWORD md5[4];			## d41d8cd9 8f00b204 e9800998 ecf8427e
			  char unk2;			## 00
			  DWORD channelId;		## 00000100
			  int unk3;				## 00000000
			};
		"""

		packet = io.BytesIO (packetStr);
		print "packet = " + binascii.hexlify (packetStr);

		type = stream_unpack ("<H", packet);
		print "type = %x" % type;								# packetType

		unk1 = packet.read (4);
		print "unk1 = " + binascii.hexlify(unk1);				# UNKNOWN

		plaintext = packet.read (17);
		print "plaintext = " + binascii.hexlify(plaintext);		# Plaintext

		md5 = packet.read (16);
		print "md5 = " + binascii.hexlify(md5);					# MD5

		unk2 = packet.read (1);
		print "unk2 = " + binascii.hexlify(unk2);				# UNKNOWN

		channelId = stream_unpack ("<I", packet);
		print "channelId = %x" % channelId;						# UNKNOWN - Maybe channel ID ?

		unk3 = packet.read (4);
		print "unk3 = " + binascii.hexlify(unk3);				# UNKNOWN

		if packet.read (1):
			print "WARNING : The packet still contains data sent from the client that hasn't been read.";

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
