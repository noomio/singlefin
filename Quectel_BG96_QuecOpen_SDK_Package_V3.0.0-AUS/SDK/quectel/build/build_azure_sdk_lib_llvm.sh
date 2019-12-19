#!/bin/sh
set fileformat=unix
echo "beginning"
#export LLVMROOT=/pkg/qct/software/llvm/release/arm/4.0.3
export LLVMROOT=C:/compile_tools/LLVM/4.0.3
export TOOLCHAIN_PATH=$LLVMROOT/bin
CC=clang
AR=llvm-ar
SDK_LIB_PATH=libs
INC_BASE=../../common/include
AZURE_SDK_SRC_PATH=../azure-iot-sdk-c
AZURE_SDK_INC_PATH=../azure-iot-sdk-c
AZURE_PORT_INC_PATH=../porting-layer
SDK_OUTPUT_PATH=../azure-iot-sdk-c/build
SDK_LIB_NAME=azure_sdk.lib

if test -d "${SDK_OUTPUT_PATH}"; then
	echo $SDK_OUTPUT_PATH
else
	echo "mkdir ../azure-iot-sdk-c/build"
	mkdir -p ${SDK_OUTPUT_PATH}
fi

SDK_CFLAGS="-DSTRINGS_C_SPRINTF_BUFFER_SIZE=300 -DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS -DQCOM_THREADX_LOG -DAZURE_IOT -DDONT_USE_UPLOADTOBLOB -D_WANT_IO_C99_FORMATS -marm -target armv7m-none-musleabi -mfloat-abi=softfp -mfpu=none -mcpu=cortex-a7 -mno-unaligned-access -fms-extensions -fuse-baremetal-sysroot -fdata-sections -ffunction-sections -fomit-frame-pointer -fshort-enums -Wno-unused-parameter -Wno-missing-field-initializers"					 
SDK_INCPATHS="-I $LLVMROOT/armv7m-none-eabi/libc/include -I $INC_BASE -I $INC_BASE/threadx_api -I $INC_BASE/qapi -I $AZURE_PORT_INC_PATH/inc -I $AZURE_SDK_INC_PATH/c-utility/inc -I $AZURE_SDK_INC_PATH/c-utility/pal/generic -I $AZURE_SDK_INC_PATH/serializer/inc -I $AZURE_SDK_INC_PATH/iothub_client/inc -I $AZURE_SDK_INC_PATH/deps/parson -I $AZURE_SDK_INC_PATH/umqtt/inc"			  		  
			  	
declare -a AZURE_SDK_FILES=("$AZURE_SDK_SRC_PATH/c-utility/src/base64.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/buffer.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/connection_string_parser.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/constbuffer.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/consolelogger.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/crt_abstractions.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/constmap.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/doublylinkedlist.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/gballoc.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/gb_stdio.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/gb_time.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/gb_rand.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/hmac.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/hmacsha256.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/http_proxy_io.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/xio.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/singlylinkedlist.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/map.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/sastoken.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/sha1.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/sha224.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/sha384-512.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/strings.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/string_tokenizer.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/urlencode.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/usha.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/vector.c"	
	"$AZURE_SDK_SRC_PATH/c-utility/src/optionhandler.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/etwxlogging.c"
	"$AZURE_SDK_SRC_PATH/c-utility/src/xlogging.c"
	
	"$AZURE_SDK_SRC_PATH/serializer/src/schemaserializer.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/agenttypesystem.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/codefirst.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/commanddecoder.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/datamarshaller.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/datapublisher.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/dataserializer.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/iotdevice.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/jsondecoder.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/jsonencoder.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/multitree.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/schema.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/schemalib.c"
	"$AZURE_SDK_SRC_PATH/serializer/src/methodreturn.c"	
	
	"$AZURE_SDK_SRC_PATH/iothub_client/src/version.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_message.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_client_ll.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_client.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothubtransport.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothubtransport_mqtt_common.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothubtransportmqtt.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_client_authorization.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_client_retry_control.c"	
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_client_diagnostic.c"	
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_client_core.c"	
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_client_core_ll.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub_device_client.c"
	"$AZURE_SDK_SRC_PATH/iothub_client/src/iothub.c"
	
	"$AZURE_SDK_SRC_PATH/deps/parson/parson.c"
	
	"$AZURE_SDK_SRC_PATH/umqtt/src/mqtt_client.c"
	"$AZURE_SDK_SRC_PATH/umqtt/src/mqtt_codec.c"
	"$AZURE_SDK_SRC_PATH/umqtt/src/mqtt_message.c")

echo "continue"
rm $SDK_OUTPUT_PATH/*.o 

for sdk_file in "${AZURE_SDK_FILES[@]}" 
do
    echo "Building porting library - $TOOLCHAIN_PATH/$CC -g -c $SDK_CFLAGS $SDK_INCPATHS $sdk_file"	
	$TOOLCHAIN_PATH/$CC -g -c $SDK_CFLAGS $SDK_INCPATHS $sdk_file
done

mv *.o $SDK_OUTPUT_PATH
echo "end"

for outfile in $SDK_OUTPUT_PATH/*.o
do
	$TOOLCHAIN_PATH/$AR -rcs $SDK_OUTPUT_PATH/$SDK_LIB_NAME $outfile
done

mv $SDK_OUTPUT_PATH/$SDK_LIB_NAME $SDK_LIB_PATH

echo "Finished compiling SDK library, built at $SDK_OUTPUT_PATH/$SDK_LIB_NAME"