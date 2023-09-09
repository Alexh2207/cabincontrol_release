################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/i2c_master/i2c_master.cpp 

CPP_DEPS += \
./src/i2c_master/i2c_master.d 

OBJS += \
./src/i2c_master/i2c_master.o 


# Each subdirectory must supply rules for building sources it contributes
src/i2c_master/%.o: ../src/i2c_master/%.cpp src/i2c_master/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	aarch64-poky-linux-gcc  -mcpu=cortex-a53 -march=armv8-a+crc -mbranch-protection=standard -fstack-protector-strong  -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security --sysroot=/home/ubuntu/kirk_yocto/sdk3/sysroots/cortexa53-poky-linux -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-i2c_master

clean-src-2f-i2c_master:
	-$(RM) ./src/i2c_master/i2c_master.d ./src/i2c_master/i2c_master.o

.PHONY: clean-src-2f-i2c_master

