################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/PWMDriver/custom_PWM.cpp 

CPP_DEPS += \
./src/PWMDriver/custom_PWM.d 

OBJS += \
./src/PWMDriver/custom_PWM.o 


# Each subdirectory must supply rules for building sources it contributes
src/PWMDriver/%.o: ../src/PWMDriver/%.cpp src/PWMDriver/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	aarch64-poky-linux-gcc  -mcpu=cortex-a53 -march=armv8-a+crc -mbranch-protection=standard -fstack-protector-strong  -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security --sysroot=/home/ubuntu/kirk_yocto/sdk3/sysroots/cortexa53-poky-linux -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-PWMDriver

clean-src-2f-PWMDriver:
	-$(RM) ./src/PWMDriver/custom_PWM.d ./src/PWMDriver/custom_PWM.o

.PHONY: clean-src-2f-PWMDriver

