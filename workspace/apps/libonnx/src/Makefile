#
# Makefile for library
#

LLVM = ../../../../tools/LLVM/4.0.3
CROSS_COMPILE	?= $(LLVM)

AS			:= $(CROSS_COMPILE)/bin/clang.exe -x assembler-with-cpp
CC			:= $(CROSS_COMPILE)/bin/clang.exe
CXX			:= $(CROSS_COMPILE)/bin/clang++.exe
LD			:= $(CROSS_COMPILE)ld
AR			:= $(CROSS_COMPILE)/bin/llvm-ar.exe
OC			:= $(CROSS_COMPILE)objcopy
OD			:= $(CROSS_COMPILE)objdump
RM			:= rm -fr

ASFLAGS		:= -Wall -O3
CFLAGS		:= -Wall -O3 
CXXFLAGS	:= -Wall -O3 
INCDIRS		:= -I . -I$(LLVM)/armv7m-none-eabi/libc/include
SRCDIRS		:= . default

SFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.S))
CFILES		:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.c))
CPPFILES	:= $(foreach dir, $(SRCDIRS), $(wildcard $(dir)/*.cpp))

SDEPS		:= $(patsubst %, %, $(SFILES:.S=.o.d))
CDEPS		:= $(patsubst %, %, $(CFILES:.c=.o.d))
CPPDEPS		:= $(patsubst %, %, $(CPPFILES:.cpp=.o.d))
DEPS		:= $(SDEPS) $(CDEPS) $(CPPDEPS)

SOBJS		:= $(patsubst %, %, $(SFILES:.S=.o))
COBJS		:= $(patsubst %, %, $(CFILES:.c=.o))
CPPOBJS		:= $(patsubst %, %, $(CPPFILES:.cpp=.o)) 
OBJS		:= $(SOBJS) $(COBJS) $(CPPOBJS)

NAME		:= libonnx.a

.PHONY: all clean

all : $(NAME)

$(NAME) : $(OBJS)
	@echo [AR] Archiving $@
	@$(AR) -rcs $@ $(OBJS)

$(SOBJS) : %.o : %.S
	@echo [AS] $<
	@$(AS) $(ASFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(COBJS) : %.o : %.c
	@echo [CC] $<
	@$(CC) $(CFLAGS)  -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

$(CPPOBJS) : %.o : %.cpp
	@echo [CXX] $<
	@$(CXX) $(CXXFLAGS) -MD -MP -MF $@.d $(INCDIRS) -c $< -o $@

clean:
	@$(RM) $(DEPS) $(OBJS) $(NAME)

sinclude $(DEPS)
