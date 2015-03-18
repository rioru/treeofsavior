#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC = gcc
CXX = g++
AR = ar
LD = g++
WINDRES = windres

INC = -I../../../src -I../../../include -I../../../include/zmq
CFLAGS = -Wall -std=gnu99
RESINC = 
LIBDIR = -L../../../lib
LIB = -lzmq -lczmq
LDFLAGS = 

INC_DEBUG = $(INC)
CFLAGS_DEBUG = $(CFLAGS) -g
RESINC_DEBUG = $(RESINC)
RCFLAGS_DEBUG = $(RCFLAGS)
LIBDIR_DEBUG = $(LIBDIR)
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG = $(LDFLAGS)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = ../../../bin/ZoneServer

INC_RELEASE = $(INC)
CFLAGS_RELEASE = $(CFLAGS) -O2
RESINC_RELEASE = $(RESINC)
RCFLAGS_RELEASE = $(RCFLAGS)
LIBDIR_RELEASE = $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE = $(LDFLAGS) -s
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = ../../../bin/ZoneServer

OBJ_DEBUG = $(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionWorker/SessionWorker.o $(OBJDIR_DEBUG)/__/__/__/src/Common/dbg/dbg.o $(OBJDIR_DEBUG)/__/__/__/src/R1EMU.o $(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionServer.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Session/SessionTable.o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneHandler/ZoneHandler.o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneServer.o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneWorker/ZoneWorker.o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/main.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketHandler.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Commander/Commander.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Crypto/Crypto.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/Packet.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketStream.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketType.o $(OBJDIR_DEBUG)/__/__/__/src/Common/Session/ClientSession.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionWorker/SessionWorker.o $(OBJDIR_RELEASE)/__/__/__/src/Common/dbg/dbg.o $(OBJDIR_RELEASE)/__/__/__/src/R1EMU.o $(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionServer.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Session/SessionTable.o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneHandler/ZoneHandler.o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneServer.o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneWorker/ZoneWorker.o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/main.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketHandler.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Commander/Commander.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Crypto/Crypto.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/Packet.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketStream.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketType.o $(OBJDIR_RELEASE)/__/__/__/src/Common/Session/ClientSession.o

all: debug release

clean: clean_debug clean_release

before_debug: 
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionWorker || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionWorker
	test -d $(OBJDIR_DEBUG)/__/__/__/src/Common/dbg || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/Common/dbg
	test -d $(OBJDIR_DEBUG)/__/__/__/src || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src
	test -d $(OBJDIR_DEBUG)/__/__/__/src/SessionServer || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/SessionServer
	test -d $(OBJDIR_DEBUG)/__/__/__/src/Common/Session || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/Common/Session
	test -d $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneHandler || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneHandler
	test -d $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer
	test -d $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneWorker || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneWorker
	test -d $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet
	test -d $(OBJDIR_DEBUG)/__/__/__/src/Common/Commander || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/Common/Commander
	test -d $(OBJDIR_DEBUG)/__/__/__/src/Common/Crypto || mkdir -p $(OBJDIR_DEBUG)/__/__/__/src/Common/Crypto

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LIBDIR_DEBUG) -o $(OUT_DEBUG) $(OBJ_DEBUG)  $(LDFLAGS_DEBUG) $(LIB_DEBUG)

$(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionWorker/SessionWorker.o: ../../../src/SessionServer/SessionWorker/SessionWorker.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/SessionServer/SessionWorker/SessionWorker.c -o $(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionWorker/SessionWorker.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/dbg/dbg.o: ../../../src/Common/dbg/dbg.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/dbg/dbg.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/dbg/dbg.o

$(OBJDIR_DEBUG)/__/__/__/src/R1EMU.o: ../../../src/R1EMU.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/R1EMU.c -o $(OBJDIR_DEBUG)/__/__/__/src/R1EMU.o

$(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionServer.o: ../../../src/SessionServer/SessionServer.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/SessionServer/SessionServer.c -o $(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionServer.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Session/SessionTable.o: ../../../src/Common/Session/SessionTable.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Session/SessionTable.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Session/SessionTable.o

$(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneHandler/ZoneHandler.o: ../../../src/ZoneServer/ZoneHandler/ZoneHandler.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/ZoneServer/ZoneHandler/ZoneHandler.c -o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneHandler/ZoneHandler.o

$(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneServer.o: ../../../src/ZoneServer/ZoneServer.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/ZoneServer/ZoneServer.c -o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneServer.o

$(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneWorker/ZoneWorker.o: ../../../src/ZoneServer/ZoneWorker/ZoneWorker.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/ZoneServer/ZoneWorker/ZoneWorker.c -o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneWorker/ZoneWorker.o

$(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/main.o: ../../../src/ZoneServer/main.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/ZoneServer/main.c -o $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/main.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketHandler.o: ../../../src/Common/Packet/PacketHandler.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Packet/PacketHandler.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketHandler.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Commander/Commander.o: ../../../src/Common/Commander/Commander.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Commander/Commander.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Commander/Commander.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Crypto/Crypto.o: ../../../src/Common/Crypto/Crypto.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Crypto/Crypto.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Crypto/Crypto.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/Packet.o: ../../../src/Common/Packet/Packet.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Packet/Packet.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/Packet.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketStream.o: ../../../src/Common/Packet/PacketStream.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Packet/PacketStream.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketStream.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketType.o: ../../../src/Common/Packet/PacketType.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Packet/PacketType.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet/PacketType.o

$(OBJDIR_DEBUG)/__/__/__/src/Common/Session/ClientSession.o: ../../../src/Common/Session/ClientSession.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c ../../../src/Common/Session/ClientSession.c -o $(OBJDIR_DEBUG)/__/__/__/src/Common/Session/ClientSession.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf ../../../bin
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/SessionServer/SessionWorker
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/Common/dbg
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/SessionServer
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/Common/Session
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneHandler
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/ZoneServer/ZoneWorker
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/Common/Packet
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/Common/Commander
	rm -rf $(OBJDIR_DEBUG)/__/__/__/src/Common/Crypto

before_release: 
	test -d ../../../bin || mkdir -p ../../../bin
	test -d $(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionWorker || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionWorker
	test -d $(OBJDIR_RELEASE)/__/__/__/src/Common/dbg || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/Common/dbg
	test -d $(OBJDIR_RELEASE)/__/__/__/src || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src
	test -d $(OBJDIR_RELEASE)/__/__/__/src/SessionServer || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/SessionServer
	test -d $(OBJDIR_RELEASE)/__/__/__/src/Common/Session || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/Common/Session
	test -d $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneHandler || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneHandler
	test -d $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer
	test -d $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneWorker || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneWorker
	test -d $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet
	test -d $(OBJDIR_RELEASE)/__/__/__/src/Common/Commander || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/Common/Commander
	test -d $(OBJDIR_RELEASE)/__/__/__/src/Common/Crypto || mkdir -p $(OBJDIR_RELEASE)/__/__/__/src/Common/Crypto

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LIBDIR_RELEASE) -o $(OUT_RELEASE) $(OBJ_RELEASE)  $(LDFLAGS_RELEASE) $(LIB_RELEASE)

$(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionWorker/SessionWorker.o: ../../../src/SessionServer/SessionWorker/SessionWorker.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/SessionServer/SessionWorker/SessionWorker.c -o $(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionWorker/SessionWorker.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/dbg/dbg.o: ../../../src/Common/dbg/dbg.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/dbg/dbg.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/dbg/dbg.o

$(OBJDIR_RELEASE)/__/__/__/src/R1EMU.o: ../../../src/R1EMU.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/R1EMU.c -o $(OBJDIR_RELEASE)/__/__/__/src/R1EMU.o

$(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionServer.o: ../../../src/SessionServer/SessionServer.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/SessionServer/SessionServer.c -o $(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionServer.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Session/SessionTable.o: ../../../src/Common/Session/SessionTable.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Session/SessionTable.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Session/SessionTable.o

$(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneHandler/ZoneHandler.o: ../../../src/ZoneServer/ZoneHandler/ZoneHandler.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/ZoneServer/ZoneHandler/ZoneHandler.c -o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneHandler/ZoneHandler.o

$(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneServer.o: ../../../src/ZoneServer/ZoneServer.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/ZoneServer/ZoneServer.c -o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneServer.o

$(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneWorker/ZoneWorker.o: ../../../src/ZoneServer/ZoneWorker/ZoneWorker.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/ZoneServer/ZoneWorker/ZoneWorker.c -o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneWorker/ZoneWorker.o

$(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/main.o: ../../../src/ZoneServer/main.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/ZoneServer/main.c -o $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/main.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketHandler.o: ../../../src/Common/Packet/PacketHandler.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Packet/PacketHandler.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketHandler.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Commander/Commander.o: ../../../src/Common/Commander/Commander.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Commander/Commander.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Commander/Commander.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Crypto/Crypto.o: ../../../src/Common/Crypto/Crypto.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Crypto/Crypto.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Crypto/Crypto.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/Packet.o: ../../../src/Common/Packet/Packet.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Packet/Packet.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/Packet.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketStream.o: ../../../src/Common/Packet/PacketStream.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Packet/PacketStream.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketStream.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketType.o: ../../../src/Common/Packet/PacketType.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Packet/PacketType.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet/PacketType.o

$(OBJDIR_RELEASE)/__/__/__/src/Common/Session/ClientSession.o: ../../../src/Common/Session/ClientSession.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c ../../../src/Common/Session/ClientSession.c -o $(OBJDIR_RELEASE)/__/__/__/src/Common/Session/ClientSession.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf ../../../bin
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/SessionServer/SessionWorker
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/Common/dbg
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/SessionServer
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/Common/Session
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneHandler
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/ZoneServer/ZoneWorker
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/Common/Packet
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/Common/Commander
	rm -rf $(OBJDIR_RELEASE)/__/__/__/src/Common/Crypto

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release

