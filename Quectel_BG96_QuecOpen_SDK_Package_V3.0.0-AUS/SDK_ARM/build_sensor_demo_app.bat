@echo off
set DAM_RO_BASE=0x43000000

set TOOL_PATH_ROOT=C:\compile_tools
set TOOLCHAIN_PATH=%TOOL_PATH_ROOT%\LLVM\4.0.3\bin
set TOOLCHAIN_PATH_STANDARdS=%TOOL_PATH_ROOT%\LLVM\4.0.3\armv7m-none-eabi\libc\include
set LLVMLIB=%TOOL_PATH_ROOT%\LLVM\4.0.3\lib\clang\4.0.3\lib
set LLVMLINK_PATH=%TOOL_PATH_ROOT%\LLVM\4.0.3\tools\bin
set PYTHON_PATH=%TOOL_PATH_ROOT%\Python27\python.exe
set SENSOR_ELF_OUTPUT_PATH=bin
set DAM_INC_BASE=include
set DAM_LIB_PATH=libs
set DAM_SRC_PATH=sensor\src

set DEMO_APP_SRC_PATH=sensor\src\demo
set DEMO_APP_OUTPUT_PATH=sensor\src\build
set SENSOR_DAM_DEMO_LD_PATH=sensor\build

set DAM_LIBNAME=txm_lib.lib
set TIMER_LIBNAME=timer_dam_lib.lib
set DIAG_LIB_NAME=diag_dam_lib.lib
set QMI_LIB_NAME=qcci_dam_lib.lib
set QMI_QCCLI_LIB_NAME=IDL_DAM_LIB.lib

set DAM_ELF_NAME=sensor_demo.elf
set DAM_TARGET_BIN=sensor_demo.bin

echo y | del %DEMO_APP_OUTPUT_PATH%
if not exist %SENSOR_ELF_OUTPUT_PATH% (
  mkdir %SENSOR_ELF_OUTPUT_PATH%
)

if not exist %DEMO_APP_OUTPUT_PATH% (
  mkdir %DEMO_APP_OUTPUT_PATH%
)
if "%1%"=="-c" (
    echo == Cleaning... %SENSOR_ELF_OUTPUT_PATH%
    del /q /s %SENSOR_ELF_OUTPUT_PATH%\*
    echo == Cleaning... %DEMO_APP_OUTPUT_PATH%
    del /q /s  %DEMO_APP_OUTPUT_PATH%\*
  
    echo Done. 
    exit /b
)


echo "Application RO base selected = %DAM_RO_BASE%"

set DAM_CPPFLAGS=-DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS  -DFX_FILEX_PRESENT -DTX_ENABLE_IRQ_NESTING  -DTX3_CHANGES

set DAM_CFLAGS= -marm -target armv7m-none-musleabi -mfloat-abi=softfp -mfpu=none -mcpu=cortex-a7 -mno-unaligned-access  -fms-extensions -Osize -fshort-enums -Wbuiltin-macro-redefined


set DAM_INCPATHS=-I %DAM_INC_BASE% -I %DAM_INC_BASE%\threadx_api  -I %DAM_INC_BASE%\qapi -I %TOOLCHAIN_PATH_STANDARDS% -I %LLVMLIB%


echo "Compiling Demo application"

@echo on

@echo == Compiling .S file...
%TOOLCHAIN_PATH%\clang.exe -E  %DAM_CPPFLAGS% %DAM_CFLAGS% %DAM_SRC_PATH%\txm_module_preamble_llvm.S > txm_module_preamble_llvm_pp.S

%TOOLCHAIN_PATH%\clang.exe  -c %DAM_CPPFLAGS% %DAM_CFLAGS% txm_module_preamble_llvm_pp.S -o %DEMO_APP_OUTPUT_PATH%\txm_module_preamble_llvm.o

if %ERRORLEVEL%==0 goto proceed
if %ERRORLEVEL%==1 goto exit
:proceed
del txm_module_preamble_llvm_pp.S

echo == Compiling .C file...
%TOOLCHAIN_PATH%\clang.exe -c %DAM_CPPFLAGS% %DAM_CFLAGS%  %DAM_INCPATHS% %DEMO_APP_SRC_PATH%\*.c 

if %ERRORLEVEL%==0 (
echo "compilation succeed"
move *.o %DEMO_APP_OUTPUT_PATH%
echo "Linking Demo application"
%TOOLCHAIN_PATH%\clang++.exe -d -o %SENSOR_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% -target armv7m-none-musleabi -fuse-ld=qcld -lc++ -Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt -Wl,-entry=%DAM_RO_BASE% %DEMO_APP_OUTPUT_PATH%\txm_module_preamble_llvm.o -Wl,-T%SENSOR_DAM_DEMO_LD_PATH%\sensor_dam_demo.ld -Wl,-Map,-Wl,-gc-sections %DEMO_APP_OUTPUT_PATH%\*.o %DAM_LIB_PATH%\*.lib

%PYTHON_PATH% %LLVMLINK_PATH%\llvm-elf-to-hex.py --bin %SENSOR_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% --output %SENSOR_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%

echo "Demo application is built at" %SENSOR_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%
)else (
echo "Fail to compile. Exiting...."
echo "compilation failed with errors"
EXIT /B %ERRORLEVEL% 
:exit
EXIT /B %ERRORLEVEL% 
)
