#
#  Example Makefile for building a program with embedded Duktape.
#
#  There are two source sets in the distribution: (1) combined sources where
#  you only need duktape.c, duktape.h, and duk_config.h, and (2) separate
#  sources where you have a bunch of source and header files.  Whichever
#  you use, simply include the relevant sources into your C project.  This
#  Makefile uses the combined source file.
#

RM := rm.exe -rf


# Compiler options are quite flexible.  GCC versions have a significant impact
# on the size of -Os code, e.g. gcc-4.6 is much worse than gcc-4.5.

ifeq ($(OS),Windows_NT)
SDK_PATH_ROOT = ../../../Quectel_BG96_QuecOpen_SDK_Package_V3.0.1-AUS/SDK
TOOL_PATH_ROOT = ../../../tools
TOOLCHAIN_PATH_STANDARDS = $(TOOL_PATH_ROOT)/LLVM/4.0.3/armv7m-none-eabi/libc/include
LLVMLIB = $(TOOL_PATH_ROOT)/LLVM/4.0.3/lib/clang/4.0.3/lib
LLVMLINK_PATH = $(TOOL_PATH_ROOT)/LLVM/4.0.3/tools/bin
PYTHON_PATH = $(TOOL_PATH_ROOT)/Python27/python.exe
LLVM = $(TOOL_PATH_ROOT)/LLVM/4.0.3
CXX = "$(LLVM)/bin/clang++.exe"
CXX_FLAGS = -target armv7m-none-musleabi \
			-fuse-ld=qcld \
			-Wl,-mno-unaligned-access \
			-fuse-baremetal-sysroot \
			-fno-use-baremetal-crt 
AR = 
CC =  "$(LLVM)/bin/clang.exe"
else
SDK_PATH_ROOT =
LLVM = 
CXX = $(LLVM)
AR = $(LLVM)
CC =  $(LLVM)
endif

DAM_COPTIONS = 	-DQAPI_TXM_MODULE \
				-DTXM_MODULE \
				-DTX_DAM_QC_CUSTOMIZATIONS \
				-DTX_ENABLE_PROFILING \
				-DTX_ENABLE_EVENT_TRACE \
				-DTX_DISABLE_NOTIFY_CALLBACKS \
				-DFX_FILEX_PRESENT \
				-DTX_ENABLE_IRQ_NESTING  \
				-DTX3_CHANGES
				
DAM_CFLAGS =	-marm \
				-target armv7m-none-musleabi \
				-mfloat-abi=softfp \
				-mfpu=none \
				-mcpu=cortex-a7 \
				-Wno-parentheses \
				-mno-unaligned-access \
				-fms-extensions \
				-Osize \
				-fshort-enums \
				-Wbuiltin-macro-redefined \
				-Wno-tautological-constant-out-of-range-compare \
				-fomit-frame-pointer \
				-fstrict-aliasing \
                -std=c99 \
                -nostdlibinc \
                -nostdlib \


DAM_INC_BASE = $(SDK_PATH_ROOT)/include
DAM_LIB_PATH = $(SDK_PATH_ROOT)/libs/llvm
DAM_LD_PATH = ./ldscripts

DAM_RO_BASE=0x43000000
DAM_INCPATHS ?= -I"$(DAM_INC_BASE)" \
				-I"$(DAM_INC_BASE)/threadx_api" \
				-I"$(DAM_INC_BASE)/qapi" \
				-I"$(TOOLCHAIN_PATH_STANDARDS)" \
				-I"$(LLVMLIB)"


USER_OPTS = -DTX_DEBUG -DTX_USER_DEBUG_OPT=\"../../httpd/inc/user_debug_opt.h\" -DTX_MALLOC_MEM_SIZE=512

COMMON_INC = "../common/inc"

COMMON_SRC = ../common/src/retarget.c \
../common/src/malloc.c \
../common/src/stubs.c \
../common/src/dss.c 

COMMON_OBJS :=./retarget.o \
./malloc.o \
./stubs.o \
./dss.o

OUTPUT_BIN = ./bin

MALLOC = -Wl,--wrap,malloc \
		 -Wl,--wrap,free \
		 -Wl,--wrap,realloc \
		 -Wl,--wrap,calloc

PRINTF = -Wl,--wrap,printf \
	#	 -Wl,--wrap,snprintf \
	#	 -Wl,--wrap,sprintf \
	#	 -Wl,--wrap,vsnprintf

OBJ_DIR = ./src/ \


OBJS = ./httpd.o \
./main.o \

OBJS += $(COMMON_OBJS)

SRCS = ./src/main.c \
./src/httpd.c \




# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: clean sections common httpd.elf httpd.bin

# Tool invocations

sections:
	@echo '$@'
	$(CC) -E $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm.S > $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S
	$(CC) -c $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S -o $(DAM_LD_PATH)/txm_module_preamble_llvm.o

common:
	@echo 'Building $@'
	$(CC) $(USER_OPTS) $(DAM_COPTIONS) $(DAM_CFLAGS) -I$(COMMON_INC) $(DAM_INCPATHS) $(COMMON_SRC) -c 
	@echo 'Finished building target: $@'
	@echo ' '

httpd.elf: $(SRCS)
	@echo 'Building file: $@'
	$(CC) $(USER_OPTS) $(DAM_COPTIONS) $(DAM_CFLAGS) -B$(OBJ_DIR) -I$(COMMON_INC) -Iinc/ -Isrc/ $(DAM_INCPATHS) $(SRCS) -c 
	@echo 'Invoking: Cross LLVM C Linker'
	$(CC) -d -o $(OUTPUT_BIN)/$@ $(CXX_FLAGS) -Wl,-entry=$(DAM_RO_BASE) $(DAM_LD_PATH)/txm_module_preamble_llvm.o $(PRINTF) $(MALLOC) -Wl,-T $(DAM_LD_PATH)/sections.ld -Wl,-Map=$(OUTPUT_BIN)/$(patsubst '%.elf','%.map','$@'),-gc-sections -Wl,-gc-sections $(OBJS) $(DAM_LIB_PATH)/*.lib -lm -lc
	@echo 'Finished building target: $@'
	@echo ' '

httpd.bin:
	@echo 'Building target: $@'
	$(PYTHON_PATH) $(LLVMLINK_PATH)/llvm-elf-to-hex.py --bin $(OUTPUT_BIN)/$(patsubst '%.bin','%.elf','$@') --output $(OUTPUT_BIN)/$@
	

# Other Targets
clean:
	-@$(RM) ./*.o ./ldscripts/*_pp.S ./ldscripts/*.o
	-@echo ' '

.PHONY: httpd clean
.SECONDARY: