#
#  Example Makefile for building a program with embedded Duktape.
#
#  There are two source sets in the distribution: (1) combined sources where
#  you only need duktape.c, duktape.h, and duk_config.h, and (2) separate
#  sources where you have a bunch of source and header files.  Whichever
#  you use, simply include the relevant sources into your C project.  This
#  Makefile uses the combined source file.
#

# Compiler options are quite flexible.  GCC versions have a significant impact
# on the size of -Os code, e.g. gcc-4.6 is much worse than gcc-4.5.

ifeq ($(OS),Windows_NT)
	SDK_PATH_ROOT = ../../../../Quectel_BG96_QuecOpen_SDK_Package_V3.0.1-AUS/SDK
	TOOL_PATH_ROOT = ../../../../tools
	TOOLCHAIN_PATH_STANDARDS = $(TOOL_PATH_ROOT)/LLVM/4.0.3/armv7m-none-eabi/libc/include
	LLVMLIB = $(TOOL_PATH_ROOT)/LLVM/4.0.3/lib/clang/4.0.3/lib
	LLVMLINK_PATH = $(TOOL_PATH_ROOT)/LLVM/4.0.3/tools/bin
	PYTHON_PATH = $(TOOL_PATH_ROOT)/Python27/python.exe
	LLVM = $(TOOL_PATH_ROOT)/LLVM/4.0.3
	CXX = "$(LLVM)/bin/clang++.exe"
	LLD = $(LLVM)/bin/ld.qcld.exe
	CXX_FLAGS = 	-target armv7m-none-musleabi \
					-fuse-ld=qcld \
					-Wl,-mno-unaligned-access \
					-fuse-baremetal-sysroot \
					-fno-use-baremetal-crt 

	AR = 
	CC =  "$(LLVM)/bin/clang.exe"
	RM := rm.exe -rf
	MKDIR := C:/Program Files/Git/usr/bin/mkdir.exe
	CP := C:/Program Files/Git/usr/bin/cp.exe
	GZIP := C:/Program Files/Git/usr/bin/gzip.exe
	TAR := C:/Program Files/Git/usr/bin/tar.exe
	CLI_TOOL = $(TOOL_PATH_ROOT)/cli-user-app-load.ps1

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
				-DTX_DISABLE_NOTIFY_CALLBACKS  \
				-DFX_FILEX_PRESENT \
				-DTX_ENABLE_IRQ_NESTING  \
				-DTX3_CHANGES 
				
DAM_CFLAGS =	-marm \
				-target armv7m-none-musleabi \
				-mfloat-abi=hard \
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

TARGET_NAME = noomiojs
BUILDDIR = ./out


USER_OPTS = -DTX_DEBUG \
			-DTX_USER_DEBUG_OPT=\"../../ml/inc/user_debug_opt.h\" \
			-DTX_MALLOC_MEM_SIZE=512 \
			-DMALLOC_FREE_INLINE 



DUKTAPE_SOURCES = src/duk/duktape.c

COMMON_DIR = ../../common
COMMON_INC = $(COMMON_DIR)/inc
include $(COMMON_DIR)/Makefile.defs


COMMON_SRC = common/src/retarget.c \
common/src/malloc.c \
common/src/stubs.c \
common/src/printf.c \
common/src/file.c \
common/src/cli.c \
common/src/time.c \


SRCS = ./src/noomiojs.c \
./src/dummy_date_provider.c \
./src/native/event_mngr.c \
./src/native/timers.c \
./src/native/console.c \
./src/native/gpio.c \
./src/native/at.c \
./src/native/i2c.c \
./src/duk_module_node.c

OBJS += $(COMMON_SRC:%.c=$(BUILDDIR)/%.o)
OBJS += $(SRCS:%.c=$(BUILDDIR)/%.o)
OBJS += $(DUKTAPE_SOURCES:%.c=$(BUILDDIR)/%.o)

INCDIR = -Iinc/ -Isrc/duk/ -Iinc/native

.phony: all clean

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: clean preamble $(TARGET_NAME) $(TARGET_NAME).bin

release: all release-firmware

# Tool invocations


version:
	@echo|set /p TAG=#define NOOMIOJS_VERSION	> src/$(TARGET_NAME).h.in  
	git describe --tags --dirty --abbrev=0 >> src/$(TARGET_NAME).h.in 


preamble:
	@echo 'Preamble LLVM...'
	$(CC) -E $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm.S > $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S
	$(CC) -c $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S -o $(DAM_LD_PATH)/txm_module_preamble_llvm.o


# common
$(BUILDDIR)/%.o:../../%.c
	@$(MKDIR) -p $(dir $@)
	@echo 'Building target: $@'
	$(CC) -o $@ -c $(USER_OPTS) $(DAM_COPTIONS) $(DAM_CFLAGS) -I. $(INCDIR) -I$(COMMON_INC) $(DAM_INCPATHS) $^ 
	@echo 'Finished building target: $@'
	@echo ' '

$(BUILDDIR)/%.o:%.c
	@$(MKDIR) -p $(dir $@)
	@echo 'Building target: $@'
	$(CC) -o $@ -c $(USER_OPTS) $(DAM_COPTIONS) $(DAM_CFLAGS) -I. $(INCDIR) -I$(COMMON_INC) $(DAM_INCPATHS) $^ 
	@echo 'Finished building target: $@'
	@echo ' '

.PHONY: $(TARGET_NAME)
$(TARGET_NAME): $(BUILDDIR)/$(TARGET_NAME).elf
$(BUILDDIR)/$(TARGET_NAME).elf: $(OBJS)
	@echo 'Invoking: Cross LLVM C Linker'
	$(CXX) -d -o $@ $(CXX_FLAGS) -Wl,-entry=$(DAM_RO_BASE) \
	-Wl,-entry=$(DAM_RO_BASE) $(DAM_LD_PATH)/txm_module_preamble_llvm.o \
	$(GETX) $(PRINTF) $(MALLOC) \
	-Wl,-T $(DAM_LD_PATH)/sections.ld \
	-Wl,-Map=$(patsubst '%.elf','%.map','$@'),-gc-sections \
	-Wl,-gc-sections $(OBJS) \
	$(DAM_LIB_PATH)/*.lib -lm -lc
	@echo 'Finished building target: $@'
	@echo ' '

$(TARGET_NAME).bin:
	@echo 'Building target: $@'
	$(PYTHON_PATH) $(LLVMLINK_PATH)/llvm-elf-to-hex.py --bin $(BUILDDIR)/$(patsubst '%.bin','%.elf','$@') --output $(BUILDDIR)/$@


release-firmware:
	@$(eval GIT_TAG=$(shell git describe --tags --dirty --abbrev=0 )) 
	@echo $(GIT_TAG)
	$(eval REL_DIR= $(TARGET_NAME)-$(TARGET_NAME)/$(TARGET_NAME)-$(GIT_TAG))
	@$(if $(findstring dirty,$(GIT_TAG)),echo "!!! DIRTY !!!", \
	@$(shell $(MKDIR) -p $(REL_DIR)) \
	@$(shell echo /datatx/$(TARGET_NAME)-$(GIT_TAG).bin > $(REL_DIR)/oem_app_path.ini)  \
	@$(shell $(CP) bin/$(TARGET_NAME).bin $(REL_DIR)/$(TARGET_NAME)-$(GIT_TAG).bin ) \
	@$(shell $(CP) src/app.js $(REL_DIR)/app.js ) \
	@$(shell $(TAR) -cvf $(REL_DIR).tar $(REL_DIR)/*  ) )


# Other Targets
clean:
	-@$(RM) $(OBJS) ./ldscripts/*_pp.S ./ldscripts/*.o
	-@echo ' '


export CLI_TOOL TARGET_NAME BUILDDIR

include ../../Makefile.tools