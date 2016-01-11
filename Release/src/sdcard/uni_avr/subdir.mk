################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/sdcard/uni_avr/ccsbcs_avr.c 

C_DEPS += \
./src/sdcard/uni_avr/ccsbcs_avr.d 

OBJS += \
./src/sdcard/uni_avr/ccsbcs_avr.o 


# Each subdirectory must supply rules for building sources it contributes
src/sdcard/uni_avr/%.o: ../src/sdcard/uni_avr/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -I/usr/lib/avr/include/std -Wall -O3 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


