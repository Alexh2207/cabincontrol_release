################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/LSM6DSOX/LSM6DSOX.cpp 

CPP_DEPS += \
./src/LSM6DSOX/LSM6DSOX.d 

OBJS += \
./src/LSM6DSOX/LSM6DSOX.o 


# Each subdirectory must supply rules for building sources it contributes
src/LSM6DSOX/%.o: ../src/LSM6DSOX/%.cpp src/LSM6DSOX/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	aarch64-poky-linux-gcc  -mcpu=cortex-a53 -march=armv8-a+crc -mbranch-protection=standard -fstack-protector-strong  -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security --sysroot=/home/ubuntu/kirk_yocto/sdk3/sysroots/cortexa53-poky-linux -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-LSM6DSOX

clean-src-2f-LSM6DSOX:
	-$(RM) ./src/LSM6DSOX/LSM6DSOX.d ./src/LSM6DSOX/LSM6DSOX.o

.PHONY: clean-src-2f-LSM6DSOX

