# Aret N Carlsen (aret@ncarlsen.net)
#
# Makefile for packetServer
#
# Last mod: 2010-05

DEBUG = -D DEBUG

PROJROOT = /root/Projects
IROOT = -I $(PROJROOT)
IPATH = $(IROOT) -I .

ARCH = linux

TARGET = sendPacket endlessPipe
#SRC = server.cpp
all: $(TARGET)

OBJS = MemoryPool.o
OBJS += MEPEncoder.o MEPDecoder.o PosixCRC32Checksum.o SimpleServer.o MAP.o AddressGraph.o
OBJS += templateInstantiations.o

VPATH = .
VPATH += :$(PROJROOT)/ATcommon/arch/$(ARCH):$(PROJROOT)/ATcommon/MemoryPool/arch/$(ARCH):$(PROJROOT)/ATcommon/arch/$(ARCH)/virtual
VPATH += :$(PROJROOT)/Upacket/MEP/arch/$(ARCH):$(PROJROOT)/Upacket/MAP/arch/$(ARCH):$(PROJROOT)/Upacket/PosixCRC32ChecksumEngine/arch/$(ARCH)
VPATH += :$(PROJROOT)/Upacket/Servers/arch/$(ARCH):$(PROJROOT)/Upacket/Routing/arch/$(ARCH)

include $(PROJROOT)/ATcommon/arch/$(ARCH)/Makefile.inc

serverOBJS = $(OBJS) -pthread
sendPacketOBJS = $(OBJS)

endlessPipe: endlessPipe.cpp endlessPipe.o
	@echo "*** Building endlessPipe ..."
	$(CXX) $(CXXFLAGS) -o endlessPipe endlessPipe.o

#server: server.cpp server.o $(serverOBJS)
#	@echo "*** Building server ..."
#	$(CXX) $(CXXFLAGS) -o server server.o $(serverOBJS)

sendPacket: sendPacket.cpp sendPacket.o $(sendPacketOBJS)
	@echo "*** Building sendPacket ..."
	$(CXX) $(CXXFLAGS) -o sendPacket sendPacket.o $(sendPacketOBJS)

