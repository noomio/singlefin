#Makefile for embedded Kermit.
#
# Copyright (C) 1995, 2011,
#  Trustees of Columbia University in the City of New York.
#  All Rights Reserved.  See kermit.c for license.


LLVM = ../../../../tools/LLVM/4.0.3
CROSS_COMPILE	?= $(LLVM)


AS			:= $(CROSS_COMPILE)/bin/clang.exe -x assembler-with-cpp
CC			:= $(CROSS_COMPILE)/bin/clang.exe
CXX			:= $(CROSS_COMPILE)/bin/clang++.exe
LD			:= $(CROSS_COMPILE)/bin/ld.qcld.exe
AR			:= $(CROSS_COMPILE)/bin/llvm-ar.exe
OC			:= $(CROSS_COMPILE)objcopy
OD			:= $(CROSS_COMPILE)objdump
RM			:= rm -fr

ASFLAGS		:= -Wall -O3 
CFLAGS		:= 
CXXFLAGS	:= -Wall -O3 
INCDIRS		:= -I$(LLVM)/armv7m-none-eabi/libc/include

OBJS= ek.o kermit.o 920xio.o 
EK = makewhat
ALL = $(EK)

all: $(ALL)

ek: $(OBJS)
	$(LD) $(CFLAGS) -o $(OBJS)

libek.a: $(OBJS)
	@$(AR) -rcs $@ $(OBJS)

#Dependencies

ek.o: ek.c cdefs.h debug.h kermit.h platform.h

kermit.o: kermit.c cdefs.h debug.h kermit.h

920xio.o: 920xio.c cdefs.h debug.h platform.h kermit.h


#Targets

#Build with cc.
cc:
	make ek

#Build with gcc.
gcc:
	@UNAME=`uname` ; make "CC=gcc" "CC2=gcc" "CFLAGS=-D$$UNAME -O2" ek

llvmminro.lib:
	make "CC=$(CC)" "CC2=$(CC)"  "CPPFLAGS=$(INCDIRS)" \
	"CFLAGS=-DMINSIZE -DOBUFLEN=256 -DFN_MAX=16 -DRECVONLY -DNODEBUG -O2" libek.a

llvmminro:
	make "CC=$(CC)" "CC2=$(CC)"  "CPPFLAGS=$(INCDIRS)" \
	"CFLAGS=-DMINSIZE -DOBUFLEN=256 -DFN_MAX=16 -DRECVONLY -DNODEBUG -O2" ek

#Ditto but no debugging.
gccnd:
	make "CC=gcc" "CC2=gcc" "CFLAGS=-DNODEBUG -O2" ek

#Build with gcc, Receive-Only, minimum size and features.
gccmin:
	make "CC=gcc" "CC2=gcc" \
	"CFLAGS=-DMINSIZE -DOBUFLEN=256 -DFN_MAX=16 -O2" ek

#Ditto but Receive-Only:
gccminro:
	make "CC=gcc" "CC2=gcc" \
	"CFLAGS=-DMINSIZE -DOBUFLEN=256 -DFN_MAX=16 -DRECVONLY -O2" ek

#Minimum size, receive-only, but with debugging:
gccminrod:
	make "CC=gcc" "CC2=gcc" \
	"CFLAGS=-DMINSIZE -DOBUFLEN=256 -DFN_MAX=16 -DRECVONLY -DDEBUG -O2" ek

#HP-UX 9.0 or higher with ANSI C.
hp:
	make "SHELL=/usr/bin/sh" CC=/opt/ansic/bin/cc CC2=/opt/ansic/bin/cc \
	ek "CFLAGS=-DHPUX -Aa"

#To get profile, build this target, run it, then "gprof ./ek > file".
gprof:
	make "CC=gcc" "CC2=gcc" ek "CFLAGS=-DNODEBUG -pg" "LNKFLAGS=-pg"

clean:
	rm -f $(OBJS) core

makewhat:
	@echo 'Defaulting to gcc...'
	make gcc

#End of Makefile
