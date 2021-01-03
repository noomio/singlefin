
include ../../common/Makefile.defs

RM := rm.exe -rf

ifeq ($(OS),Windows_NT)
	SDK_PATH_ROOT = ../../../../Quectel_BG96_QuecOpen_SDK_Package_V3.0.1-AUS/SDK
	TOOL_PATH_ROOT = ../../../../tools
	TOOLCHAIN_PATH_STANDARDS = $(TOOL_PATH_ROOT)/LLVM/4.0.3/armv7m-none-eabi/libc/include
	LLVMLIB = $(TOOL_PATH_ROOT)/LLVM/4.0.3/lib/clang/4.0.3/lib
	LLVMLINK_PATH = $(TOOL_PATH_ROOT)/LLVM/4.0.3/tools/bin
	PYTHON_PATH = $(TOOL_PATH_ROOT)/Python27/python.exe
	LLVM = $(TOOL_PATH_ROOT)/LLVM/4.0.3
	CXX = "$(LLVM)/bin/clang++.exe"
	CXX_FLAGS = -target armv7m-none-musleabi \
				-fuse-ld=qcld \
				-Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt
	AR = 
	CC =  "$(LLVM)/bin/clang.exe"
	CLI_TOOL = $(TOOL_PATH_ROOT)/cli-user-app-load.ps1
	MKDIR := C:/Program Files/Git/usr/bin/mkdir.exe
	FIND := C:/Program Files/Git/usr/bin/find.exe
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

DAM_CFLAGS = 	-marm \
				-target armv7m-none-musleabi \
				-mfloat-abi=hard \
				-mfpu=vfpv3-d16 \
				-mcpu=cortex-a7 \
				-mno-unaligned-access  \
				-fms-extensions \
				-Osize -fshort-enums \
				-Wbuiltin-macro-redefined

DAM_INC_BASE = $(SDK_PATH_ROOT)/include
DAM_LIB_PATH = $(SDK_PATH_ROOT)/libs/llvm
DAM_LD_PATH = ./ldscripts

DAM_RO_BASE=0x43000000

DAM_INCPATHS ?= -I"$(DAM_INC_BASE)" \
				-I"$(DAM_INC_BASE)/threadx_api" \
				-I"$(DAM_INC_BASE)/qapi" \
				-I"$(TOOLCHAIN_PATH_STANDARDS)" \
				-I"$(LLVMLIB)"


TARGET_NAME = kermit
BUILDDIR = out

USER_OPTS = -DTX_DEBUG \
			-DTX_USER_DEBUG_OPT=\"../../ml/inc/user_debug_opt.h\" \
			-DTX_MALLOC_MEM_SIZE=512 \
			-DMALLOC_FREE_INLINE 

COMMON_DIR = ../../common/
COMMON_INC = $(COMMON_DIR)/inc

COMMON_SRC = common/src/retarget.c \
common/src/malloc.c \
common/src/stubs.c \
common/src/printf.c \
common/src/file.c \
common/src/cli.c \
common/src/time.c \
common/src/strings.c \


LIBS = ../ek16

SRCS = main.c 

OBJS += $(COMMON_SRC:%.c=$(BUILDDIR)/%.o)
OBJS += $(SRCS:%.c=$(BUILDDIR)/%.o)


.phony: all clean

all: preamble $(TARGET_NAME) $(TARGET_NAME).bin

preamble:
	@echo 'Preamble LLVM...'
	$(CC) -E $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm.S > $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S
	$(CC) -c $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S -o $(DAM_LD_PATH)/txm_module_preamble_llvm.o

.PHONY: $(TARGET_NAME)
$(TARGET_NAME): $(BUILDDIR)/$(TARGET_NAME).elf
$(BUILDDIR)/$(TARGET_NAME).elf: $(OBJS)
	@echo 'Invoking: Cross LLVM C++ Linker'
	$(CXX) -d -o $@ $(CXX_FLAGS) -Wl,-entry=$(DAM_RO_BASE) \
	$(DAM_LD_PATH)/txm_module_preamble_llvm.o \
	$(GETX) $(PRINTF) $(MALLOC) $(WRITE) $(FWRITE) \
	$(OBJS) \
	-Wl,-T $(DAM_LD_PATH)/sections.ld \
	-Wl,-Map=$(patsubst '%.elf','%.map','$@'),-gc-sections \
	-Wl,-gc-sections \
	-L $(LIBS) \
	$(DAM_LIB_PATH)/*.lib \
	-lek
	@echo 'Finished building target: $@'
	@echo ' '

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

# Other Targets
clean:
	-$(RM) $(OBJS) $(ARCHIVES) $(ASM_DEPS) $(S_UPPER_DEPS) $(C_DEPS) $(DAM_LD_PATH)/*.o $(DAM_LD_PATH)/*_pp.S
	-$(RM) $(BUILDDIR)
	-@echo ' '

$(TARGET_NAME).bin:
	@echo 'Building target: $@'
	$(PYTHON_PATH) $(LLVMLINK_PATH)/llvm-elf-to-hex.py --bin $(BUILDDIR)/$(patsubst '%.bin','%.elf','$@') --output $(BUILDDIR)/$@


export CLI_TOOL TARGET_NAME BUILDDIR

include ../../../cli/Makefile.tools