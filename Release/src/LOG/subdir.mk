################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/LOG/SDCardLogger.cpp \
../src/LOG/debug.cpp 

OBJS += \
./src/LOG/SDCardLogger.o \
./src/LOG/debug.o 

CPP_DEPS += \
./src/LOG/SDCardLogger.d \
./src/LOG/debug.d 


# Each subdirectory must supply rules for building sources it contributes
src/LOG/%.o: ../src/LOG/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I/usr/lib/avr/include/std -Wall -O3 -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -std=c++11 -fno-strict-aliasing -Wno-switch -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


