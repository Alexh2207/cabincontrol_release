################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/custom_gpio/custom_gpio.cpp 

CPP_DEPS += \
./src/custom_gpio/custom_gpio.d 

OBJS += \
./src/custom_gpio/custom_gpio.o 


# Each subdirectory must supply rules for building sources it contributes
src/custom_gpio/%.o: ../src/custom_gpio/%.cpp src/custom_gpio/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	aarch64-poky-linux-gcc  -mcpu=cortex-a53 -march=armv8-a+crc -mbranch-protection=standard -fstack-protector-strong  -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security --sysroot=/home/ubuntu/kirk_yocto/sdk3/sysroots/cortexa53-poky-linux -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-custom_gpio

clean-src-2f-custom_gpio:
	-$(RM) ./src/custom_gpio/custom_gpio.d ./src/custom_gpio/custom_gpio.o

.PHONY: clean-src-2f-custom_gpio

