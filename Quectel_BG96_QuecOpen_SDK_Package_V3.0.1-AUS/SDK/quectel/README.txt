This document describes how to use the Microsoft Azure SDK when developing Azure 
applications on the Qualcomm? ThreadX platform.

1. Clone the Microsoft Azure SDK IoT C package from the GitHub.
git clone -b <label> --recursive https://github.com/Azure/azure-iot-sdk-c.git

2. Copy the cloned Microsoft Azure SDK IoT C files to the SDK\quectel\azure-iot-sdk-c
folder.

3. Quectel provide sample LLVM build script to build the Microsoft Azure SDK library
(SDK\quectel\build\build_azure_sdk_lib_llvm.sh in Linux or SDK\quectel\build\
build_azure_sdk_lib_llvm.bat in Windows).

4. Update LLVMROOT path in the build script files to point to the LLVM folder location.
Ensure that the scripts point to the header files in the common folder.

5. Run the build script under SDK\quectel\build to generate the Microsoft Azure SDK IoT C library.
Find the library file under SDK\quectel\build\libs floder.

6. Update azure_sdk.lib to SDK\libs floder, and update the header files to SDK\include\azure_api 
floder.

7. Run build_quectel_demo_app.bat azureiot to build the Azure application by linking to the Microsoft
Azure SDK and Qualcomm porting libraries.