

RM := rm.exe -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs


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
			-lc++ \
			-Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt
AR = 
CC =  "$(LLVM)/bin/clang.exe"
else
SDK_PATH_ROOT =
LLVM = 
CXX = $(LLVM)
AR = $(LLVM)
CC =  $(LLVM)
CLI_TOOL = $(TOOL_PATH_ROOT)/cli-user-app-load.ps1 
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
				-mfloat-abi=softfp \
				-mfpu=none \
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

OUTPUT_BIN = ./bin


MALLOC = -Wl,--wrap,malloc \
		 -Wl,--wrap,free \
		 -Wl,--wrap,realloc \
		 -Wl,--wrap,calloc

PRINTF = -Wl,--wrap,printf \
		 -Wl,--wrap,puts \
	#	 -Wl,--wrap,snprintf \
	#	 -Wl,--wrap,sprintf \
	#	 -Wl,--wrap,vsnprintf

GETX = -Wl,--wrap,getchar \
	   -Wl,--wrap,gets	


USER_OPTS = -DTX_DEBUG -DTX_USER_DEBUG_OPT=\"../../blinky/inc/user_debug_opt.h\" -DTX_MALLOC_MEM_SIZE=512

COMMON_INC = "../common/inc"

COMMON_SRC = ../common/src/retarget.c \
../common/src/malloc.c \
../common/src/stubs.c \

COMMON_OBJS :=./retarget.o \
./malloc.o \
./stubs.o \

TARGET_NAME = blinky

# All Target
all: pre-build common blinky.elf blinky.bin

# Tool invocations

load:
	@echo 'Loading to target...' 
	$(CLI_TOOL) -BinPath '.\bin' -BinFile $(TARGET_NAME).bin -ResetTarget $True

pre-build:
	@echo 'Preamble LLVM...'
	$(CC) -E $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm.S > $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S
	$(CC) -c $(DAM_COPTIONS) $(DAM_CFLAGS) $(DAM_LD_PATH)/txm_module_preamble_llvm_pp.S -o $(DAM_LD_PATH)/txm_module_preamble_llvm.o


common: $(COMMON_SRC) 
	@echo 'Building target: $@'
	$(CC) $(USER_OPTS) $(DAM_COPTIONS) $(DAM_CFLAGS) -I"$(COMMON_INC)" $(DAM_INCPATHS) $(COMMON_SRC) -c 
	@echo 'Finished building target: $@'
	@echo ' '


$(TARGET_NAME).elf: $(OBJS)
	@echo 'Invoking: Cross LLVM C++ Linker'
	$(CXX) -d -o  $(OUTPUT_BIN)/$@ $(CXX_FLAGS) -Wl,-entry=$(DAM_RO_BASE) $(DAM_LD_PATH)/txm_module_preamble_llvm.o $(GETX) $(PRINTF) $(MALLOC)  -Wl,-T $(DAM_LD_PATH)/sections.ld -Wl,-Map=$(OUTPUT_BIN)/$(patsubst '%.elf','%.map','$@'),-gc-sections -Wl,-gc-sections $(COMMON_OBJS) $(OBJS) $(DAM_LIB_PATH)/*.lib -lm -lc
	@echo 'Finished building target: $@'
	@echo ' '


# Other Targets
clean:
	-$(RM) $(OBJS)$(ARCHIVES)$(ASM_DEPS)$(S_UPPER_DEPS)$(C_DEPS) $(COMMON_OBJS) $(DAM_LD_PATH)/*.o $(DAM_LD_PATH)/*_pp.S
	-@echo ' '

$(TARGET_NAME).bin:
	@echo 'Building target: $@'
	$(PYTHON_PATH) $(LLVMLINK_PATH)/llvm-elf-to-hex.py --bin $(OUTPUT_BIN)/$(patsubst '%.bin','%.elf','$@') --output $(OUTPUT_BIN)/$@

.PHONY: all clean 
.SECONDARY:

