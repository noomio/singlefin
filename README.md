# singlefin
SingleFin Platform for IoT Enablement

## Compile

Add SingleFin C source and header to your build. The distributable contains an example Makefile using the llvm compiler for reference (clang). In the simplest case:

```
clang.exe -o singlefin.o -c singlefin.c
```

> Look at Generate Source Files on how to generate the source files.

## Build

Open the folder in **apps/tests/file** and from the command line type:

```
make clean all
```

The output directory is out and the binary file is placed inside. Deploy to target by connecting to the USB port MODEM and type:

```
make load
```

## Library

A single include header makes it easy to integrate in your app.

```
#include "singlefin.h"

int main (void){
	
 printf("Aloha!\r\n");

 for(;;){
  fin_sleep(1000);
 }

}
```

## Generate Source Files

To configure and generate the source files you need to run the configure.py script. There is also a helper script configure.ps1 to call with PowerShell.

```
cd singlefin
```

```
python tools/configure.py 
	--source-directory src-input 
	--platform threadx 
	--architecture arm32 
	--module bg96 
	--output-directory build/fin 
```

### Supported Modems:
- BG96

## Prerequisites

Python27

## Documentation

You can view the online documention here: [SingleFin API](http://noomio.com.au/singlefin/)

There is also a local copy in the folder **website/out**
