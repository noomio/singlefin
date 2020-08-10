Compilation instruction

## Open command window in sdk directory.

## Run compile command "build_demo.bat compiler_type build_id".

 * compiler_type: This compilation script support both llvm and arm compilers.
 - If you want to build sdk example with compiler tools "llvm", please use "llvm" as compiler_type, 
   and if you want to build sdk example with compiler tools "arm", please use "arm" as compiler_type.
 - If no arguments are entered or the first argument is "help"("HELP"), you can see the following prompts:

	Please input a valid compiler type !
	Supported compiler type:
	llvm [ cmd - build_demo.bat llvm build_id ]
	arm  [ cmd - build_demo.bat arm  build_id ]

 * build_id: Used to identify the example that need to be compiled.
 - For example: 
   command for build uart example with llvm: build_demo.bat llvm uart
   command for build time example with arm:  build_demo.bat arm time
 - azureiot example only supports llvm compilation.
   If compile command "build_demo.bat arm azureiot" is entered, you can see the following prompts:
	please use llvm to compile azureiot example.

 - If the second argument is "-c", files generated from the last building will be deleted.
 - If no second argument is entered or the second argument is "help"("HELP"), you can see the following prompts:

	Please input a valid example type !
	Supported example:
	adc         [ cmd - build_demo.bat <compile_type> adc         ]
	dam_fota    [ cmd - build_demo.bat <compile_type> dam_fota    ]
	device_info [ cmd - build_demo.bat <compile_type> device_info ]
	dns_client  [ cmd - build_demo.bat <compile_type> dns_client  ]
	fota        [ cmd - build_demo.bat <compile_type> fota        ]
	gpio        [ cmd - build_demo.bat <compile_type> gpio        ]
	gpio_int    [ cmd - build_demo.bat <compile_type> gpio_int    ]
	gps         [ cmd - build_demo.bat <compile_type> gps         ]
	http        [ cmd - build_demo.bat <compile_type> http        ]
	i2c         [ cmd - build_demo.bat <compile_type> i2c         ]
	mqtt        [ cmd - build_demo.bat <compile_type> mqtt        ]
	psm         [ cmd - build_demo.bat <compile_type> psm         ]
	rtc         [ cmd - build_demo.bat <compile_type> rtc         ]
	spi         [ cmd - build_demo.bat <compile_type> spi         ]
	task_create [ cmd - build_demo.bat <compile_type> task_create ]
	tcp_client  [ cmd - build_demo.bat <compile_type> tcp_client  ]
	time        [ cmd - build_demo.bat <compile_type> time        ]
	timer       [ cmd - build_demo.bat <compile_type> timer       ]
	uart        [ cmd - build_demo.bat <compile_type> uart        ]
	atc_pipe    [ cmd - build_demo.bat <compile_type> atc_pipe    ]
	atc_sms     [ cmd - build_demo.bat <compile_type> atc_sms     ]
	qt_adc      [ cmd - build_demo.bat <compile_type> qt_adc      ]
	lwm2m       [ cmd - build_demo.bat <compile_type> lwm2m       ]
	atfwd       [ cmd - build_demo.bat <compile_type> atfwd       ]
	file        [ cmd - build_demo.bat <compile_type> file        ]
	ftp_client  [ cmd - build_demo.bat <compile_type> ftp_client  ]
	azureiot    [ cmd - build_demo.bat <compile_type> azureiot    ]   
 - note:<compile_type> in the above prompts will display as the first argument you entered, that is, llvm or arm.
        if <compile_type> is arm, the last line about azureiot will not display.

## Execute the compile command, and when "Done." is printed, the compilation is successful.
 - If you compiled with llvm, the following three files will be generated in sdk/bin automatically:
   oem_app_path.ini
   quectel_demo_<build_id>.bin
   quectel_demo.elf
 
 - If you compiled with arm, the following four files will be generated in sdk/bin automatically:
   oem_app_path.ini
   quectel_demo_<build_id>.bin
   quectel_demo.elf
   quectel_demo_<build_id>.map

## Put oem_app_path.ini and quectel_demo_<build_id>.bin into /datatx in Alternate File System of EFS, and then restart the module.

## Update QuecOpen Application(refer to the example application "dam_fota")
 * Customers can download their new application image form their own Revision Control Server. For example,
   you can use HTTP(s) to download the upgrade image to the file system and store it in the specified path.

 * If you want two places which are used to store the application images, you need modify the oem_app_path.ini file contents 
   according to the format shown as below:

   /datatx/quectel_uart_demo.bin:/datatx/upgrade/quectel_uart_demo_upgrade.bin

 - "/datatx/quectel_uart_demo.bin"(path #1) indicates the full path of the boot-up image
 - "/datatx/upgrade/quectel_uart_demo_upgrade.bin"(path #2) indicates the full path of the upgraded image
   In the example application "dam_fota", the folder "upgrade" will be created automatically if it is not present.
 
 * After download is completed, you only need to restart the module.
   While booting, the Module Loader can check whether the path #2 is present and will load it if it presents.
 
 * If oem_app_path.ini file contents is a single path, after the new app is dowmloaded successfully,
   you need use it to ovrewrite the original app in your application and then restart the module.

  