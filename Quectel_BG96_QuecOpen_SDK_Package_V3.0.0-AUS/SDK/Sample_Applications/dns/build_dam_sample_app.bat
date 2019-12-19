@echo off
set DAM_RO_BASE=0x43000000

set LLVM_ROOT_PATH=C:\Apps\LLVM\4.0.3
set TOOLCHAIN_PATH=%LLVM_ROOT_PATH%\bin
set TOOLCHAIN_PATH_STANDARDS=%LLVM_ROOT_PATH%\armv7m-none-eabi\libc\include
set LLVMLIB=%LLVM_ROOT_PATH%\lib\clang\4.0.3\lib
set LLVMLINK_PATH=%LLVM_ROOT_PATH%\tools\bin
set DAM_ELF_OUTPUT_PATH=bin
set DAM_INC_BASE=..\..\common\include
set DAM_LIB_PATH=..\..\common\libs
set DAM_SRC_PATH=src
set DEMO_APP_SRC_PATH=src\demo
set DAM_DATA_CALL_SRC_PATH=..\data_common\data_call
set DAM_UTILS_SRC_PATH=..\data_common\utils
set PYTHON_PATH=C:\Python27\python.exe

set DEMO_APP_OUTPUT_PATH=src\build
set DAM_DAM_DEMO_LD_PATH=build

set DAM_LIBNAME=txm_lib.lib
set TIMER_LIBNAME=timer_dam_lib.lib
set DIAG_LIB_NAME=diag_dam_lib.lib
set QMI_LIB_NAME=qcci_dam_lib.lib
set QMI_QCCLI_LIB_NAME=IDL_DAM_LIB.lib

set DAM_ELF_NAME=dns_dam_demo.elf
set DAM_TARGET_BIN=dns_dam_demo.bin
set DAM_MAP_NAME=dns_dam_demo.map

echo y | del %DEMO_APP_OUTPUT_PATH%
if not exist %DAM_ELF_OUTPUT_PATH% (
  mkdir %DAM_ELF_OUTPUT_PATH%
)

if not exist %DEMO_APP_OUTPUT_PATH% (
  mkdir %DEMO_APP_OUTPUT_PATH%
)


echo "Application RO base selected = %DAM_RO_BASE%"

set DAM_CPPFLAGS=-DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS  -DFX_FILEX_PRESENT -DTX_ENABLE_IRQ_NESTING  -DTX3_CHANGES

set DAM_CFLAGS= -marm -target armv7m-none-musleabi -mfloat-abi=softfp -mfpu=none -mcpu=cortex-a7 -mno-unaligned-access  -fms-extensions -Osize -fshort-enums -Wbuiltin-macro-redefined

set DAM_INCPATHS=-I %DAM_INC_BASE% -I %DAM_INC_BASE%\threadx_api  -I %DAM_INC_BASE%\qapi -I %TOOLCHAIN_PATH_STANDARDS% -I %LLVMLIB%

set DAM_APP_INCPATHS=-I %DAM_DATA_CALL_SRC_PATH% -I %DAM_UTILS_SRC_PATH% -I %DEMO_APP_SRC_PATH% %DAM_INCPATHS%

echo "Compiling Demo application"

@echo on

%TOOLCHAIN_PATH%\clang.exe -E  %DAM_CPPFLAGS% %DAM_CFLAGS% %DAM_SRC_PATH%\txm_module_preamble_llvm.S > txm_module_preamble_llvm_pp.S

%TOOLCHAIN_PATH%\clang.exe  -c %DAM_CPPFLAGS% %DAM_CFLAGS% txm_module_preamble_llvm_pp.S -o %DEMO_APP_OUTPUT_PATH%\txm_module_preamble_llvm.o

if %ERRORLEVEL%==0 goto proceed
if %ERRORLEVEL%==1 goto exit
:proceed
del txm_module_preamble_llvm_pp.S

%TOOLCHAIN_PATH%\clang.exe -c %DAM_CPPFLAGS% %DAM_CFLAGS% %DAM_APP_INCPATHS% %DEMO_APP_SRC_PATH%\*.c 
%TOOLCHAIN_PATH%\clang.exe -c %DAM_CPPFLAGS% %DAM_CFLAGS% %DAM_APP_INCPATHS% %DAM_DATA_CALL_SRC_PATH%\*.c
%TOOLCHAIN_PATH%\clang.exe -c %DAM_CPPFLAGS% %DAM_CFLAGS% %DAM_INCPATHS% %DAM_UTILS_SRC_PATH%\*.c

if %ERRORLEVEL%==0 (
echo "compilation succeed"
move *.o %DEMO_APP_OUTPUT_PATH%
echo "Linking Demo application"

%TOOLCHAIN_PATH%\clang++.exe -d -o %DAM_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% -target armv7m-none-musleabi -fuse-ld=qcld -lc++ -Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt -Wl,-entry=%DAM_RO_BASE% %DEMO_APP_OUTPUT_PATH%\txm_module_preamble_llvm.o -Wl,-T%DAM_DAM_DEMO_LD_PATH%\app_dam_demo.ld -Wl,-Map=%DAM_ELF_OUTPUT_PATH%\%DAM_MAP_NAME%,-gc-sections -Wl,-gc-sections %DEMO_APP_OUTPUT_PATH%\*.o %DAM_LIB_PATH%\*.lib
%PYTHON_PATH% %LLVMLINK_PATH%\llvm-elf-to-hex.py --bin %DAM_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% --output %DAM_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%

echo "Demo application is built at" %DAM_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%
)else (
echo "Fail to compile. Exiting...."
echo "compilation failed with errors"
EXIT /B %ERRORLEVEL% 
:exit
EXIT /B %ERRORLEVEL% 
)
