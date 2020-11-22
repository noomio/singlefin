
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
AR = "$(LLVM)/bin/llvm-ar.exe"
CC =  "$(LLVM)/bin/clang.exe"
CLI_TOOL = $(TOOL_PATH_ROOT)/cli-user-app-load.ps1
else
SDK_PATH_ROOT =
LLVM = 
CXX = $(LLVM)
AR = $(LLVM)
CC =  $(LLVM)
endif

DAM_COPTIONS =  -DQAPI_TXM_MODULE \
				-DTXM_MODULE \
				-DTX_DAM_QC_CUSTOMIZATIONS \
				-DTX_ENABLE_PROFILING \
				-DTX_ENABLE_EVENT_TRACE \
				-DTX_DISABLE_NOTIFY_CALLBACKS \
				-DFX_FILEX_PRESENT \
				-DTX_ENABLE_IRQ_NESTING  \
				-DTX3_CHANGES

DAM_CFLAGS =    -marm \
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

MKDIR := C:/Program Files/Git/usr/bin/mkdir.exe
FIND := C:/Program Files/Git/usr/bin/find.exe


VARIABLE+=TRACE_LEVEL
HELP_TRACE_LEVEL=trace verbosity
# TRACE_LEVEL?=0

VARIABLE+=BUILDDIR
HELP_BUILDDIR=build directory
BUILDDIR?=build


VARIABLE+=BENCHMARKDIR
HELP_BENCHMARKDIR=benchmark directory
BENCHMARKDIR?=benchmarks

VARIABLE+=PROFILINGDIR
HELP_PROFILINGDIR=profiling directory
PROFILINGDIR?=profiling

VARIABLE+=MODELS
HELP_MODELS=existing models
ifndef MODELS
MODELS+=mnist
MODELS+=tinyyolov2
MODELS+=super_resolution
MODELS+=mobilenetv2
endif

VARIABLE+=OPERATORS
HELP_OPERATORS=operators to test (all if empty)

VARIABLE+=REPEAT
HELP_REPEAT=default repetition count if not otherwise specified by REPEAT_<modelname>
REPEAT=1

VARIABLE+=FORMAT
HELP_FORMAT=which files to format (git wildcards)
ifndef FORMAT
FORMAT+=**/*.h
FORMAT+=**/*.c
FORMAT+=!**/pb/**/*
FORMAT+=!**/third_party/**/*
endif

VARIABLE+=ONNX_CUSTOM
HELP_ONNX_CUSTOM=use custom onnx installation
ONNX_CUSTOM=third_party/onnx/onnx.build

VARIABLE+=ONNX_INCLUDE
HELP_ONNX_INCLUDE=which schemas to include
ifndef ONNX_INCLUDE
ONNX_INCLUDE+="^Add$$"
ONNX_INCLUDE+="^ArgMax$$"
ONNX_INCLUDE+="^BatchNormalization$$"
ONNX_INCLUDE+="^Clip$$"
ONNX_INCLUDE+="^Constant$$"
ONNX_INCLUDE+="^Conv$$"
ONNX_INCLUDE+="^GlobalAveragePool$$"
ONNX_INCLUDE+="^LeakyRelu$$"
ONNX_INCLUDE+="^MatMul$$"
ONNX_INCLUDE+="^MaxPool$$"
ONNX_INCLUDE+="^Mul$$"
ONNX_INCLUDE+="^Relu$$"
ONNX_INCLUDE+="^Reshape$$"
ONNX_INCLUDE+="^Sigmoid$$"
ONNX_INCLUDE+="^Softmax$$"
ONNX_INCLUDE+="^Transpose$$"
ONNX_INCLUDE+="^Elu$$"
ONNX_INCLUDE+="^Identity$$"
endif

VARIABLE+=ONNX_VERSION
HELP_ONNX_VERSION=which onnx version to use
ONNX_VERSION=latest

VARIABLE+=ONNX_DOMAINS
HELP_ONNX_DOMAINS=which onnx domains to use
ONNX_DOMAINS=ai.onnx

VARIABLE+=ONNX_EXCLUDE
HELP_ONNX_EXCLUDE=which schemas to exclude
ONNX_EXCLUDE=

$(foreach MODEL, $(MODELS), $(eval REPEAT_$(MODEL)=$(REPEAT)))
REPEAT_tinyyolov2=1
REPEAT_super_resolution=1
REPEAT_mnist=5
REPEAT_mobilenetv2=1

CC?=gcc
CFLAGS+=-std=c99
CFLAGS+=-Wall
CFLAGS+=-g3 -gdwarf -O2
# CFLAGS+=-Werror # CI jobs run with flag enabled
ifdef TRACE_LEVEL
CPPFLAGS+=-D "TRACE_LEVEL=$(TRACE_LEVEL)"
endif

LDFLAGS+=-g
LDLIBS+=-lcunit
LDLIBS+=-lm

INCDIR = -Iinclude \
	-Iinclude/operators \
	-Isrc \
	-Isrc/pb


dirs += src/operators
dirs += src/pb
SRCS+=$(foreach dir, $(dirs), $(shell $(FIND) src -type f -name '*.c'))
OBJS=$(SRCS:%.c=$(BUILDDIR)/%.o)


.phony: connxr
TARGET+=connxr
connxr: $(BUILDDIR)/connxr
$(BUILDDIR)/connxr: $(OBJS)
	@echo 'Creating Archive: $@'
	@echo ' '
	$(AR) rc $(TARGET).a $(OBJS)

$(BUILDDIR)/%.o:%.c
	@$(MKDIR) -p $(dir $@)
	@echo 'Building target: $@'
	$(CC) -o $@ -c $(USER_OPTS) $(DAM_COPTIONS) $(DAM_CFLAGS) $(INCDIR) $(DAM_INCPATHS) $^ 
	@echo 'Finished building target: $@'
	@echo ' '


.phony: clean_build
CLEAN+=clean_build
clean_build:
	rm -rf $(BUILDDIR)

