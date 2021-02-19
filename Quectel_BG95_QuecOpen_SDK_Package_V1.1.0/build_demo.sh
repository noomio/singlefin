SH_COMMAND=build_demo.sh

DEMO_ELF_OUTPUT_PATH="./bin"
DEMO_APP_OUTPUT_PATH="./quectel/example/build"

if [ ! -d $DEMO_ELF_OUTPUT_PATH ]; then
	mkdir $DEMO_ELF_OUTPUT_PATH
fi
	
if [ ! -d $DEMO_APP_OUTPUT_PATH ]; then
	mkdir $DEMO_APP_OUTPUT_PATH
fi


compiler_help_menu(){
	echo "Supported compiler type :"
	echo " llvm	[ cmd - $SH_COMMAND llvm build_id ]"
	exit
}

if [ $# -eq 1 ]; then
	if [ $1 == "help" -o $1 == "HELP" ]; then
		compiler_help_menu
	elif [ $1 == "llvm" -o $1 == "LLVM" ]; then
		COMPILER_TYPE=llvm
		echo "Supported example :"
		awk -F "-" '{print $1"[ cmd - '$0' '$1' ",$1"]"}' example.txt
		exit
	else
		echo "Please input a valid compiler type !"
	fi
elif [ $# -eq 2 ]; then
	if [ $1 == "llvm" -o $1 == "LLVM" ]; then
		COMPILER_TYPE=llvm
	else
		echo "Please input a valid compiler type !"
		exit
	fi
	
	# build help
	if [ $2 == "help" -o $2 == "HELP" ]; then
		echo "Supported example :"
		awk -F "-" '{print $1"[ cmd - '$0' '$1' ",$1"]"}' example.txt
		exit
	# build clean
	elif [ $2 == "-c" -o $2 == "-C" ]; then
		echo == Cleaning... $DEMO_ELF_OUTPUT_PATH
		rm -rf $DEMO_ELF_OUTPUT_PATH/*
		rm -rf $DEMO_ELF_OUTPUT_PATH/*o
		echo == Cleaning... $DEMO_APP_OUTPUT_PATH
		rm -rf $DEMO_APP_OUTPUT_PATH/*
		rm -rf $DEMO_APP_OUTPUT_PATH/*o
		echo "Done."
		exit
	else
		PRECOMPILED_MACRO=$(grep "\<$2\>" example.txt | awk -F "-" '{print $2}')
		if [ "$PRECOMPILED_MACRO" == "" ]; then
			echo "Please input a valid example build id !"
			exit
		else
			BUILD_APP_FLAG="-D$PRECOMPILED_MACRO"
		fi
	fi
	
	# clean last build
	echo == Cleaning... $DEMO_ELF_OUTPUT_PATH
	rm -rf $DEMO_ELF_OUTPUT_PATH/*
	rm -rf $DEMO_ELF_OUTPUT_PATH/*o
	echo == Cleaning... $DEMO_APP_OUTPUT_PATH
	rm -rf $DEMO_APP_OUTPUT_PATH/*
	rm -rf $DEMO_APP_OUTPUT_PATH/*o
	
	if [ $COMPILER_TYPE == "llvm" ]; then
		# Virtual address start from 0x4300_0000
		DAM_RO_BASE=0x43000000
		
		TOOL_PATH_ROOT="C:/compile_tools"
		TOOLCHAIN_PATH="$TOOL_PATH_ROOT/LLVM/4.0.3/bin"
		# echo TOOLCHAIN_PATH is $TOOLCHAIN_PATH
		TOOLCHAIN_PATH_STANDARdS="$TOOL_PATH_ROOT/LLVM/4.0.3/armv7m-none-eabi/libc/include"
		LLVMLIB="$TOOL_PATH_ROOT/LLVM/4.0.3/lib/clang/4.0.3/lib"
		LLVMLINK_PATH="$TOOL_PATH_ROOT/LLVM/4.0.3/tools/bin"
		PYTHON_PATH="$TOOL_PATH_ROOT/Python27/python.exe"

		DAM_INC_BASE="./include"
		DAM_LIB_PATH="./libs"
		DEMO_SRC_PATH="./quectel/example"
		
		DEMO_APP_SRC_PATH="./quectel/example/$2/src"
		DEMO_APP_INC_PATH="./quectel/example/$2/inc"
		DEMO_APP_LD_PATH="./quectel/build"
		DEMO_APP_UTILS_SRC_PATH="./quectel/utils/source"
		DEMO_APP_UTILS_INC_PATH="./quectel/utils/include"

		DAM_LIBNAME="txm_lib.lib"
		TIMER_LIBNAME="timer_dam_lib.lib"
		DIAG_LIB_NAME="diag_dam_lib.lib"
		QMI_LIB_NAME="qcci_dam_lib.lib"
		QMI_QCCLI_LIB_NAME="IDL_DAM_LIB.lib"

		DAM_ELF_NAME="quectel_demo_$2.elf"
		DAM_TARGET_BIN="quectel_demo_$2.bin"
		DAM_MAP_NAME="quectel_demo_$2.map"
		
		echo "== Application RO base selected = $DAM_RO_BASE"

		export DAM_CPPFLAGS="-DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS  -DFX_FILEX_PRESENT -DTX_ENABLE_IRQ_NESTING  -DTX3_CHANGES" 
		export DAM_CFLAGS="-marm -target armv7m-none-musleabi -mfloat-abi=softfp -mfpu=none -mcpu=cortex-a7 -mno-unaligned-access  -fms-extensions -Osize -fshort-enums -Wbuiltin-macro-redefined"
		export DAM_INCPATHS="-I $DAM_INC_BASE -I $DAM_INC_BASE/threadx_api -I $DAM_INC_BASE/qmi -I $DAM_INC_BASE/qapi -I $TOOLCHAIN_PATH_STANDARdS -I $DAM_CPPFLAGS -I $LLVMLIB -I $DEMO_APP_UTILS_INC_PATH -I $DEMO_APP_INC_PATH"
		
		#set -x;
		
		echo "== Compiling .S file..."
		$TOOLCHAIN_PATH/clang.exe -E  $DAM_CPPFLAGS $DAM_CFLAGS $DEMO_SRC_PATH/txm_module_preamble_llvm.S > txm_module_preamble_llvm_pp.S
		$TOOLCHAIN_PATH/clang.exe  -c $DAM_CPPFLAGS $DAM_CFLAGS txm_module_preamble_llvm_pp.S -o $DEMO_APP_OUTPUT_PATH/txm_module_preamble_llvm.o
		rm txm_module_preamble_llvm_pp.S

		echo "== Compiling .C file..."
		$TOOLCHAIN_PATH/clang.exe  -c  $DAM_CPPFLAGS $BUILD_APP_FLAG $DAM_CFLAGS $DAM_INCPATHS $DEMO_APP_SRC_PATH/*.c
		$TOOLCHAIN_PATH/clang.exe  -c  $DAM_CPPFLAGS $BUILD_APP_FLAG $DAM_CFLAGS $DAM_INCPATHS $DEMO_APP_UTILS_SRC_PATH/*.c
		mv *.o $DEMO_APP_OUTPUT_PATH

		echo "== Linking Example $2 application"
		$TOOLCHAIN_PATH/clang++.exe -d -o $DEMO_ELF_OUTPUT_PATH/$DAM_ELF_NAME -target armv7m-none-musleabi -fuse-ld=qcld -lc++ -Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt -Wl,-entry=$DAM_RO_BASE $DEMO_APP_OUTPUT_PATH/txm_module_preamble_llvm.o -Wl,-T$DEMO_APP_LD_PATH/quectel_dam_demo.ld -Wl,-Map=$DEMO_ELF_OUTPUT_PATH/$DAM_MAP_NAME,-gc-sections -Wl,-gc-sections $DEMO_APP_OUTPUT_PATH/*.o $DAM_LIB_PATH/*.lib
		$PYTHON_PATH $LLVMLINK_PATH/llvm-elf-to-hex.py --bin $DEMO_ELF_OUTPUT_PATH/$DAM_ELF_NAME --output $DEMO_ELF_OUTPUT_PATH/$DAM_TARGET_BIN
		
		#set +x;
		
		echo "== Demo application is built in $DEMO_ELF_OUTPUT_PATH"
		echo -n "quectel_demo_$2.bin" > ./bin/oem_app_path.ini
		echo "Done."
		exit
	else
		echo "Internal error !"
		exit
	fi
else
	echo "Please input a valid compiler type and build id !"
	exit
fi
