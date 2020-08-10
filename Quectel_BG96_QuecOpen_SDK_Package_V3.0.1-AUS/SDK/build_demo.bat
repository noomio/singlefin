@echo off

set BAT_COMMAND=build_demo.bat

if "%1%"=="llvm" (
	set COMPILER_TYPE=llvm
) else if "%1%"=="arm" (
	set COMPILER_TYPE=arm
) else if "%1%"=="help" (
	goto compiler_help_menu
) else if "%1%"=="HELP" (
	goto compiler_help_menu
) else (
	echo Please input a valid compiler type !
	goto compiler_help_menu
)

set DEMO_ELF_OUTPUT_PATH=bin
set DEMO_APP_OUTPUT_PATH=quectel\example\build

if not exist %DEMO_ELF_OUTPUT_PATH% (
	mkdir %DEMO_ELF_OUTPUT_PATH%
)
if not exist %DEMO_APP_OUTPUT_PATH% (
	mkdir %DEMO_APP_OUTPUT_PATH%
)

if "%2%"=="-c" (
	REM last build
	echo == Cleaning... %DEMO_ELF_OUTPUT_PATH%
	del /q /s %DEMO_ELF_OUTPUT_PATH%\*
	echo == Cleaning... %DEMO_APP_OUTPUT_PATH%
	del /q /s %DEMO_APP_OUTPUT_PATH%\*
	echo Done.
	exit /b
) else if "%2%"=="azureiot" (
	if "%COMPILER_TYPE%"=="arm" (
		echo please use llvm to compile azureiot example.
		goto end
	) else (
		REM Example for azureiot
		echo == Compiling %2% example ...
		set BUILD_APP_FLAG=-D__EXAMPLE_AZUREIOT__
	)
) else if "%2%"=="help" (
	goto help_menu
) else if "%2%"=="HELP" (
	goto help_menu
) else (
	goto precompile_process
)

:precompile_process
	setlocal EnableDelayedExpansion
	set z=0
	for /f "delims=- tokens=1,2" %%i in (example.txt) do (
		set x=%%i
		call :trim x
		if !x! == %2 (
			set y=%%j
			set z=1
			break
		)
	)
	endlocal & set a=%y% & set b=%z%

	if %b% == 1 (
		set BUILD_APP_FLAG=-D%a%
	) else (
		echo Please input a valid example build id !
		goto help_menu
	)
	
	REM last build
	echo == Cleaning Last Building ... %DEMO_ELF_OUTPUT_PATH%
	del /q /s %DEMO_ELF_OUTPUT_PATH%\*
	echo == Cleaning... %DEMO_APP_OUTPUT_PATH%
	del /q /s %DEMO_APP_OUTPUT_PATH%\*	
		
	if "%1%"=="llvm" (	
		goto llvm_process
	) else if "%1%"=="arm" (
		goto arm_process
	) else (
		echo Internal error !
		goto end
	)

:trim
if "!%1:~-1!"==" " (set %1=!%1:~0,-1!&&goto trim)
goto :eof

:llvm_process
	REM Virtual address start from 0x4300_0000
	set DAM_RO_BASE=0x43000000
	
	set TOOL_PATH_ROOT=C:\compile_tools
	set TOOLCHAIN_PATH=%TOOL_PATH_ROOT%\LLVM\4.0.3\bin
	rem echo TOOLCHAIN_PATH is %TOOLCHAIN_PATH%
	set TOOLCHAIN_PATH_STANDARdS=%TOOL_PATH_ROOT%\LLVM\4.0.3\armv7m-none-eabi\libc\include
	set LLVMLIB=%TOOL_PATH_ROOT%\LLVM\4.0.3\lib\clang\4.0.3\lib
	set LLVMLINK_PATH=%TOOL_PATH_ROOT%\LLVM\4.0.3\tools\bin
	set PYTHON_PATH=%TOOL_PATH_ROOT%\Python27\python.exe

	set DAM_INC_BASE=include
	set DAM_LIB_PATH=libs\llvm
	set DEMO_SRC_PATH=quectel\example
	
	set DEMO_APP_SRC_PATH=quectel\example\%2%\src
	set DEMO_APP_INC_PATH=quectel\example\%2%\inc
	set DEMO_APP_LD_PATH=quectel\build
	set DEMO_APP_UTILS_SRC_PATH=quectel\utils\source
	set DEMO_APP_UTILS_INC_PATH=quectel\utils\include
	set AZURE_SDK_INC_PATH=include\azure_api
	set AZURE_SDK_PORT_INC_PATH=include\porting_laye

	set DAM_LIBNAME=txm_lib.lib
	set TIMER_LIBNAME=timer_dam_lib.lib
	set DIAG_LIB_NAME=diag_dam_lib.lib
	set QMI_LIB_NAME=qcci_dam_lib.lib
	set QMI_QCCLI_LIB_NAME=IDL_DAM_LIB.lib
	set AZURE_SDK_LIBNAME=azure_sdk.lib
	set AZURE_SDK_PORT_LIBNAME=azure_sdk_port.lib

	set DAM_ELF_NAME=quectel_demo_%2%.elf
	set DAM_TARGET_BIN=quectel_demo_%2%.bin
	set DAM_MAP_NAME=quectel_demo_%2%.map
		
	echo == Application RO base selected = %DAM_RO_BASE%

	set DAM_CPPFLAGS=-DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS  -DFX_FILEX_PRESENT -DTX_ENABLE_IRQ_NESTING  -DTX3_CHANGES
	 
	set DAM_CFLAGS= -marm -target armv7m-none-musleabi -mfloat-abi=softfp -mfpu=none -mcpu=cortex-a7 -mno-unaligned-access  -fms-extensions -Osize -fshort-enums -Wbuiltin-macro-redefined

	set DAM_INCPATHS=-I %DAM_INC_BASE% -I %DAM_INC_BASE%\threadx_api -I %DAM_INC_BASE%\qmi -I %DAM_INC_BASE%\qapi -I %TOOLCHAIN_PATH_STANDARdS% -I %DAM_CPPFLAGS% -I %LLVMLIB% -I %DEMO_APP_UTILS_INC_PATH% -I %DEMO_APP_INC_PATH%

	if "%2%"=="azureiot" (
	set DAM_CPPFLAGS=%DAM_CPPFLAGS% -DQCOM_THREADX_LOG -DAZURE_IOT -DDONT_USE_UPLOADTOBLOB

	set DAM_INCPATHS=%DAM_INCPATHS% -I %AZURE_SDK_PORT_INC_PATH%\inc -I %AZURE_SDK_INC_PATH%\c-utility\inc -I %AZURE_SDK_INC_PATH%\c-utility\pal\generic -I %AZURE_SDK_INC_PATH%\serializer\inc -I %AZURE_SDK_INC_PATH%\iothub_client\inc -I %AZURE_SDK_INC_PATH%\deps\parson -I %AZURE_SDK_INC_PATH%\umqtt\inc
	)

	@echo off
	@echo == Compiling .S file...
	%TOOLCHAIN_PATH%\clang.exe -E  %DAM_CPPFLAGS% %DAM_CFLAGS% %DEMO_SRC_PATH%\txm_module_preamble_llvm.S > txm_module_preamble_llvm_pp.S
	%TOOLCHAIN_PATH%\clang.exe  -c %DAM_CPPFLAGS% %DAM_CFLAGS% txm_module_preamble_llvm_pp.S -o %DEMO_APP_OUTPUT_PATH%\txm_module_preamble_llvm.o
	del txm_module_preamble_llvm_pp.S

	echo == Compiling .C file...
	for %%x in (%DEMO_APP_SRC_PATH%\*.c) do (
		 %TOOLCHAIN_PATH%\clang.exe  -c  %DAM_CPPFLAGS% %BUILD_APP_FLAG% %DAM_CFLAGS% %DAM_INCPATHS% %%x
	)
	for %%x in (%DEMO_APP_UTILS_SRC_PATH%\*.c) do (
		 %TOOLCHAIN_PATH%\clang.exe  -c  %DAM_CPPFLAGS% %BUILD_APP_FLAG% %DAM_CFLAGS% %DAM_INCPATHS% %%x
	)
	move *.o %DEMO_APP_OUTPUT_PATH%

	echo == Linking Example %2% application
	%TOOLCHAIN_PATH%\clang++.exe -d -o %DEMO_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% -target armv7m-none-musleabi -fuse-ld=qcld -lc++ -Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt -Wl,-entry=%DAM_RO_BASE% %DEMO_APP_OUTPUT_PATH%\txm_module_preamble_llvm.o -Wl,-T%DEMO_APP_LD_PATH%\quectel_dam_demo.ld -Wl,-Map=%DEMO_ELF_OUTPUT_PATH%\%DAM_MAP_NAME%,-gc-sections -Wl,-gc-sections %DEMO_APP_OUTPUT_PATH%\*.o %DAM_LIB_PATH%\*.lib
	%PYTHON_PATH% %LLVMLINK_PATH%\llvm-elf-to-hex.py --bin %DEMO_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% --output %DEMO_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%

	echo == Demo application is built in %DEMO_ELF_OUTPUT_PATH%
	set /p =/datatx/quectel_demo_%2%.bin<nul > .\bin\oem_app_path.ini
	echo Done.
	exit /b

:arm_process
	REM Virtual address start from 0x4000_0000
	set DAM_RO_BASE=0x40000000
	
	set TOOL_PATH_ROOT=C:\compile_tools
	set TOOLCHAIN_PATH=%TOOL_PATH_ROOT%\ARM_Compiler_5\bin
	rem set LM_LICENSE_FILE=%TOOL_PATH_ROOT%\license.dat
	set ARMLMD_LICENSE_FILE=8620@192.168.1.149

	REM DAM related path
	set DAM_INC_BASE=include
	set DAM_LIB_PATH=libs\arm

	REM application related path
	set APP_SRC_P_PATH=quectel

	REM example utils source and header path
	set APP_UTILS_SRC_PATH=quectel\utils\source
	set APP_UTILS_INC_PATH=quectel\utils\include

	REM example source and header path
	set APP_SRC_PATH=quectel\example\%2%\src
	set APP_INC_PATH=quectel\example\%2%\inc

	set DAM_LIBNAME="txm_lib.lib"
	set TIMER_LIBNAME="timer_dam_lib.lib"

	set DAM_ELF_NAME=quectel_demo_%2%.elf
	set DAM_TARGET_BIN=quectel_demo_%2%.bin
	set DAM_TARGET_MAP=quectel_demo_%2%.map
	
	echo == Application RO base selected = %DAM_RO_BASE%

	set DAM_CPPFLAGS=-DT_ARM -D__RVCT__ -D_ARM_ASM_ -DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS -DTX_DAM_QC_CUSTOMIZATIONS
	set DAM_CLAGS=-O1 --diag_suppress=9931 --diag_error=warning --cpu=Cortex-A7 --protect_stack --arm_only --apcs=\interwork
	set DAM_INCPATHS=-I %DAM_INC_BASE% -I %DAM_INC_BASE%\threadx_api -I %DAM_INC_BASE%\qapi -I %APP_UTILS_INC_PATH% -I %APP_INC_PATH%
	set APP_CFLAGS=-DTARGET_THREADX -DENABLE_IOT_INFO -DENABLE_IOT_DEBUG -DSENSOR_SIMULATE

	@echo off

	@echo == Compiling .S file...
	%TOOLCHAIN_PATH%\armcc -E -g %DAM_CPPFLAGS% %APP_SRC_P_PATH%\example\txm_module_preamble.S > txm_module_preamble_pp.S
	%TOOLCHAIN_PATH%\armcc -g -c %DAM_CPPFLAGS% txm_module_preamble_pp.S -o %DEMO_APP_OUTPUT_PATH%\txm_module_preamble.o
	del txm_module_preamble_pp.S

	@echo == Compiling .C file...
	for %%x in (%APP_SRC_PATH%\*.c) do (
		%TOOLCHAIN_PATH%\armcc -g -c %DAM_CPPFLAGS% %APP_CFLAGS% %BUILD_APP_FLAG% %DAM_INCPATHS% %%x
	)
	for %%x in (%APP_UTILS_SRC_PATH%\*.c) do (
		%TOOLCHAIN_PATH%\armcc -g -c %DAM_CPPFLAGS% %APP_CFLAGS% %BUILD_APP_FLAG% %DAM_INCPATHS% %%x
	)
	move *.o %DEMO_APP_OUTPUT_PATH%

	setlocal enabledelayedexpansion
	set OBJ_FULL_PATH=
	for %%x in (%DEMO_APP_OUTPUT_PATH%\*.o) do (
		set OBJ_FULL_PATH=!OBJ_FULL_PATH! %%x
	)

	echo == Linking Example %2% application
	%TOOLCHAIN_PATH%\armlink -d -o %DEMO_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% --elf --ro %DAM_RO_BASE% --first txm_module_preamble.o --entry=_txm_module_thread_shell_entry --map --remove --symbols --list %DEMO_ELF_OUTPUT_PATH%\%DAM_TARGET_MAP% %OBJ_FULL_PATH% %DAM_LIB_PATH%\timer_dam_lib.lib %DAM_LIB_PATH%\txm_lib.lib
	%TOOLCHAIN_PATH%\fromelf --bincombined %DEMO_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% --output %DEMO_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%

	echo == Example image built at %DEMO_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%
	set /p =/datatx/quectel_demo_%2%.bin<nul > .\bin\oem_app_path.ini
	echo Done.
	exit /b

:compiler_help_menu
	echo Supported compiler type :
	echo	llvm	[ cmd - %BAT_COMMAND% llvm build_id ]
	echo	arm     [ cmd - %BAT_COMMAND% arm  build_id ]
	exit /b

:help_menu
    setlocal EnableDelayedExpansion
	set z=0
	for /f "delims=- tokens=1" %%i in (example.txt) do (
		set x=%%i
		echo !x![ cmd - %BAT_COMMAND% %COMPILER_TYPE% !x!]
	)
	endlocal
    exit /b

:end
