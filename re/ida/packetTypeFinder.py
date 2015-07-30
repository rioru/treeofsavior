#!/usr/bin/python
"""
Tree of Savior IDAPython Script
Find packet listing
"""

import idaapi
import idautils
import idc

gePacketTable__PACKET_TABLE__AddPacketSize = 0x00991F00;

f = open ('PacketType.h', 'w');
f.write ("typedef enum PacketType {\n\n");

# Rename all functions declared with gePacketTable__PACKET_TABLE__AddPacketSize
occ = RfirstB (gePacketTable__PACKET_TABLE__AddPacketSize);
while occ != BADADDR:
    packetIdInsn = PrevHead (occ);
    packetSizeInsn = PrevHead (packetIdInsn);

    packetId = GetOperandValue (packetIdInsn, 0);
    packetSize = GetOperandValue (packetSizeInsn, 0);
    packetName = GetString (Dword (occ + 6));
    packetLine = "\t%s = %d, // Size = %d" % (packetName, packetId, packetSize);
    print packetLine;
    f.write (packetLine + "\n");
    occ = RnextB (gePacketTable__PACKET_TABLE__AddPacketSize, occ);

f.write ('\n\tPACKET_TYPE_COUNT\n\n');
f.write ('}\tPacketType;');
f.close ();