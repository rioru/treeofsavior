# Dependencies
import binascii
import struct
import time
import socket
import io
from ChannelPacketType import ChannelPacketType
from PacketType import PacketType
import itertools as IT

def stream_unpack (fmt, stream):
    size = struct.calcsize(fmt)
    buf = stream.read(size)
    return struct.unpack(fmt, buf)

# ==========================================================
class ClientHandler:
	# Configurations
	PACKETSIZE_MAX = 8192

	def start (self, sock):
		"""
			00000000  55 0b 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |U...............|
			00000010  00 00 00 00 00 00 00 d4  1d 8c d9 8f 00 b2 04 e9  |.......Ô.OÙ..².é|
			00000020  80 09 98 ec f8 42 7e 00  00 00 01 00 00 00 00 00  |. .ìøB~.........|
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
		packetStr = sock.recv (self.PACKETSIZE_MAX);
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

			
		reply  = struct.pack ("<H", PacketType.SC_NORMAL);
		reply += struct.pack("<I", 0); # UNKNOWN
		# reply += struct.pack("<H", X); # Size of the packet, added dynamically at the end
		reply += struct.pack("<I", ChannelPacketType.RECV_SNS_PC_GAME_INFO);  # Encapsulated type of the packet (cf 0x087B5F0)
		
		reply += "\x01"; # Index
		reply += "\x01"; # Count of something
		
		# Add dynamically the size of the packet
		size = struct.pack("<H", len(reply) + 2); # +2 because it counts itself
		reply = reply[:6] + size + reply[6:];
		
		sock.send (reply);
		
		
	def __init__ (self, sock):
		self.start (sock);
