################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/TFTDriver/display_driver.cpp \
../src/TFTDriver/spi_master.cpp 

CPP_DEPS += \
./src/TFTDriver/display_driver.d \
./src/TFTDriver/spi_master.d 

OBJS += \
./src/TFTDriver/display_driver.o \
./src/TFTDriver/spi_master.o 


# Each subdirectory must supply rules for building sources it contributes
src/TFTDriver/%.o: ../src/TFTDriver/%.cpp src/TFTDriver/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	aarch64-poky-linux-gcc  -mcpu=cortex-a53 -march=armv8-a+crc -mbranch-protection=standard -fstack-protector-strong  -O2 -D_FORTIFY_SOURCE=2 -Wformat -Wformat-security -Werror=format-security --sysroot=/home/ubuntu/kirk_yocto/sdk3/sysroots/cortexa53-poky-linux -O0 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-src-2f-TFTDriver

clean-src-2f-TFTDriver:
	-$(RM) ./src/TFTDriver/display_driver.d ./src/TFTDriver/display_driver.o ./src/TFTDriver/spi_master.d ./src/TFTDriver/spi_master.o

.PHONY: clean-src-2f-TFTDriver

