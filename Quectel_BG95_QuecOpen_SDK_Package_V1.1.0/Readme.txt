Compilation instruction

## Open command window in sdk directory.

## Run compile command "build_demo.bat compiler_type build_id".

 * compiler_type: This compilation script support llvm compilers only.
 - If you want to build sdk example with compiler tools "llvm", please use "llvm" as compiler_type, 
 - If no arguments are entered or the first argument is "help"("HELP"), you can see the following prompts:

	Please input a valid compiler type !
	Supported compiler type:
	llvm [ cmd - build_demo.bat llvm build_id ]

 * build_id: Used to identify the example that need to be compiled.
 - For example: 
   command for build uart example with llvm: build_demo.bat llvm uart

 - If the second argument is "-c", files generated from the last building will be deleted.
 - If no second argument is entered or the second argument is "help"("HELP"), you can see the following prompts:

	Please input a valid example type !
	Supported example:
	adc         [ cmd - build_demo.bat <compile_type> adc         ]
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
 - note:<compile_type> in the above prompts will display as the first argument you entered, that is, llvm in current SDK package.

## Execute the compile command, and when "Done." is printed, the compilation is successful.
 - If you compiled with llvm, the following three files will be generated in sdk/bin automatically:
   oem_app_path.ini
   quectel_demo_<build_id>.bin
   quectel_demo.elf

## Put oem_app_path.ini and quectel_demo_<build_id>.bin into /datatx in Alternate File System of EFS, and then restart the module.
  