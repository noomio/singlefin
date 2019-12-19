
# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
./src/blinky.c \


OBJS += \
./src/blinky.o \

CPP_DEPS += \



# Each subdirectory must supply rules for building sources it contributes
./src/%.o: ./src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: LLVM Cross ARM C Compiler'
	$(CC)  -c $(USER_OPTS) $(DAM_COPTIONS) $(DAM_CFLAGS) -I. -I./inc -I$(COMMON_INC) $(DAM_INCPATHS) -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


