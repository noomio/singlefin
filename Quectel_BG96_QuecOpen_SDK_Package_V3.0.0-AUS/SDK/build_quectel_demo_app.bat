@echo off
set DAM_RO_BASE=0x43000000

set TOOL_PATH_ROOT=C:\compile_tools
set TOOLCHAIN_PATH=%TOOL_PATH_ROOT%\LLVM\4.0.3\bin
set TOOLCHAIN_PATH_STANDARdS=%TOOL_PATH_ROOT%\LLVM\4.0.3\armv7m-none-eabi\libc\include
set LLVMLIB=%TOOL_PATH_ROOT%\LLVM\4.0.3\lib\clang\4.0.3\lib
set LLVMLINK_PATH=%TOOL_PATH_ROOT%\LLVM\4.0.3\tools\bin
set PYTHON_PATH=%TOOL_PATH_ROOT%\Python27\python.exe

set BAT_COMMAND=build_quectel_demo_app.bat
set DEMO_ELF_OUTPUT_PATH=bin
set DAM_INC_BASE=include
set DAM_LIB_PATH=libs
set DEMO_SRC_PATH=quectel\example

set DEMO_APP_SRC_PATH=quectel\example\%1%\src
set DEMO_APP_INC_PATH=quectel\example\%1%\inc
set DEMO_APP_OUTPUT_PATH=quectel\example\build
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

set DAM_ELF_NAME=quectel_demo_%1%.elf
set DAM_TARGET_BIN=quectel_demo_%1%.bin

if not exist %DEMO_ELF_OUTPUT_PATH% (
    mkdir %DEMO_ELF_OUTPUT_PATH%
)

if not exist %DEMO_APP_OUTPUT_PATH% (
    mkdir %DEMO_APP_OUTPUT_PATH%
)

if "%1%"=="-c" (
    echo == Cleaning... %DEMO_ELF_OUTPUT_PATH%
    del /q /s %DEMO_ELF_OUTPUT_PATH%\*
    echo == Cleaning... %DEMO_APP_OUTPUT_PATH%
    del /q /s %DEMO_APP_OUTPUT_PATH%\*

    echo Done.
    exit /b
) else if "%1%"=="adc" (
	REM Example for adc
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_ADC__
) else if "%1%"=="device_info" (
	REM Example for device_info
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_DEVICE_INFO__
) else if "%1%"=="dns_client" (
	REM Example for dns_client
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_DNSCLIENT__
) else if "%1%"=="gpio" (
	REM Example for gpio
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_GPIO__
) else if "%1%"=="gpio_int" (
	REM Example for gpio_int
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_GPIO_INT__
) else if "%1%"=="gps" (
	REM Example for gps
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_GPS__
) else if "%1%"=="qt_gps" (
	REM Example for gps
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_QT_GPS__
) else if "%1%"=="http" (
	REM Example for http
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_HTTP__
) else if "%1%"=="i2c" (
	REM Example for i2c
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_I2C__
) else if "%1%"=="mqtt" (
	REM Example for mqtt
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_MQTT__
) else if "%1%"=="psm" (
	REM Example for psm
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_PSM__
) else if "%1%"=="rtc" (
	REM Example for rtc
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_RTC__
) else if "%1%"=="spi" (
	REM Example for spi
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_SPI__
) else if "%1%"=="task_create" (
	REM Example for task_create
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_TASK_CREATE__
) else if "%1%"=="tcp_client" (
	REM Example for tcp_client
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_TCPCLIENT__
) else if "%1%"=="time" (
	REM Example for time
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_TIME__
) else if "%1%"=="timer" (
	REM Example for timer
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_TIMER__
) else if "%1%"=="uart" (
	REM Example for UART
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_UART__
) else if "%1%"=="atc_pipe" (
	REM Example for atc_pipe
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_ATC_PIPE__
) else if "%1%"=="atc_sms" (
	REM Example for atc_sms
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_ATC_SMS__

) else if "%1%"=="dns_client" (
	REM Example for dns_client
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_DNSCLIENT__

) else if "%1%"=="qt_adc" (
	REM Example for qt_adc
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_QT_ADC__
) else if "%1%"=="nonip" (
	REM Example for nonip
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_NONIP__
) else if "%1%"=="fota" (
	REM Example for fota
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_FOTA__
) else if "%1%"=="lwm2m" (
	REM Example for lwm2m
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_LWM2M_EXT__
)else if "%1%"=="atfwd" (
	REM Example for atfwd
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_ATFWD__
)else if "%1%"=="stdlib" (
	REM Example for stdlib
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_STDLIB__
)else if "%1%"=="file" (
	REM Example for file
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_FILE__
)else if "%1%"=="ftp_client" (
	REM Example for ftp_client
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_FTPCLIENT__
)else if "%1%"=="azureiot" (
	REM Example for azureiot
    echo == Compiling %1% example ...
    set BUILD_APP_FLAG=-D__EXAMPLE_AZUREIOT__
) else if "%1%"=="help" (
    goto help_menu
) else if "%1%"=="HELP" (
    goto help_menu
) else (
    echo Please input a valid example build id !
	goto help_menu
)

echo == Application RO base selected = %DAM_RO_BASE%

set DAM_CPPFLAGS=-DQAPI_TXM_MODULE -DTXM_MODULE -DTX_DAM_QC_CUSTOMIZATIONS -DTX_ENABLE_PROFILING -DTX_ENABLE_EVENT_TRACE -DTX_DISABLE_NOTIFY_CALLBACKS  -DFX_FILEX_PRESENT -DTX_ENABLE_IRQ_NESTING  -DTX3_CHANGES
 
set DAM_CFLAGS= -marm -target armv7m-none-musleabi -mfloat-abi=softfp -mfpu=none -mcpu=cortex-a7 -mno-unaligned-access  -fms-extensions -Osize -fshort-enums -Wbuiltin-macro-redefined

set DAM_INCPATHS=-I %DAM_INC_BASE% -I %DAM_INC_BASE%\threadx_api -I %DAM_INC_BASE%\qmi -I %DAM_INC_BASE%\qapi -I %TOOLCHAIN_PATH_STANDARdS% -I %DAM_CPPFLAGS% -I %LLVMLIB% -I %DEMO_APP_UTILS_INC_PATH% -I %DEMO_APP_INC_PATH%

if "%1%"=="azureiot" (
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

echo == Linking Example %1% application
%TOOLCHAIN_PATH%\clang++.exe -d -o %DEMO_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% -target armv7m-none-musleabi -fuse-ld=qcld -lc++ -Wl,-mno-unaligned-access -fuse-baremetal-sysroot -fno-use-baremetal-crt -Wl,-entry=%DAM_RO_BASE% %DEMO_APP_OUTPUT_PATH%\txm_module_preamble_llvm.o -Wl,-T%DEMO_APP_LD_PATH%\quectel_dam_demo.ld -Wl,-Map,-Wl,-gc-sections %DEMO_APP_OUTPUT_PATH%\*.o %DAM_LIB_PATH%\*.lib
%PYTHON_PATH% %LLVMLINK_PATH%\llvm-elf-to-hex.py --bin %DEMO_ELF_OUTPUT_PATH%\%DAM_ELF_NAME% --output %DEMO_ELF_OUTPUT_PATH%\%DAM_TARGET_BIN%

echo == Demo application is built in %DEMO_ELF_OUTPUT_PATH%
set /p =/datatx/quectel_demo_%1%.bin<nul > .\bin\oem_app_path.ini
echo Done.
goto end

:help_menu
    echo Supported example :
    echo    adc         [ cmd - %BAT_COMMAND% adc         ]
    echo    device_info [ cmd - %BAT_COMMAND% device_info ]
    echo    dns_client  [ cmd - %BAT_COMMAND% dns_client  ]
	echo    gpio        [ cmd - %BAT_COMMAND% gpio        ]
    echo    gpio_int    [ cmd - %BAT_COMMAND% gpio_int    ]
    echo    gps         [ cmd - %BAT_COMMAND% gps         ]
    echo    http        [ cmd - %BAT_COMMAND% http        ]
    echo    i2c         [ cmd - %BAT_COMMAND% i2c         ]
	echo    mqtt        [ cmd - %BAT_COMMAND% mqtt        ]
    echo    psm         [ cmd - %BAT_COMMAND% psm         ]
    echo    rtc         [ cmd - %BAT_COMMAND% rtc         ]
	echo    spi         [ cmd - %BAT_COMMAND% spi         ]
    echo    task_create [ cmd - %BAT_COMMAND% task_create ]
    echo    tcp_client  [ cmd - %BAT_COMMAND% tcp_client  ]
    echo    time        [ cmd - %BAT_COMMAND% time        ]
    echo    timer       [ cmd - %BAT_COMMAND% timer       ]
    echo    uart        [ cmd - %BAT_COMMAND% uart        ]
    echo    atc_pipe    [ cmd - %BAT_COMMAND% atc_pipe    ]
    echo    atc_sms     [ cmd - %BAT_COMMAND% atc_sms     ]

    echo    qt_adc      [ cmd - %BAT_COMMAND% qt_adc      ]
    echo    nonip       [ cmd - %BAT_COMMAND% nonip       ]
    echo    fota        [ cmd - %BAT_COMMAND% fota        ]
    echo    lwm2m       [ cmd - %BAT_COMMAND% lwm2m       ]
	echo    atfwd       [ cmd - %BAT_COMMAND% atfwd       ]
	echo    stdlib      [ cmd - %BAT_COMMAND% stdlib      ]
	echo    file        [ cmd - %BAT_COMMAND% file        ]
	echo    ftp_client  [ cmd - %BAT_COMMAND% ftp_client  ]
	echo    azureiot    [ cmd - %BAT_COMMAND% azureiot    ]   
	exit /b

:end