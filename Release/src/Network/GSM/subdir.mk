################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Network/GSM/GSM.cpp \
../src/Network/GSM/SIM900.cpp \
../src/Network/GSM/call.cpp \
../src/Network/GSM/gps.cpp \
../src/Network/GSM/inetGSM.cpp \
../src/Network/GSM/sms.cpp 

OBJS += \
./src/Network/GSM/GSM.o \
./src/Network/GSM/SIM900.o \
./src/Network/GSM/call.o \
./src/Network/GSM/gps.o \
./src/Network/GSM/inetGSM.o \
./src/Network/GSM/sms.o 

CPP_DEPS += \
./src/Network/GSM/GSM.d \
./src/Network/GSM/SIM900.d \
./src/Network/GSM/call.d \
./src/Network/GSM/gps.d \
./src/Network/GSM/inetGSM.d \
./src/Network/GSM/sms.d 


# Each subdirectory must supply rules for building sources it contributes
src/Network/GSM/%.o: ../src/Network/GSM/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I/usr/lib/avr/include/std -Wall -O3 -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -std=c++11 -fno-strict-aliasing -Wno-switch -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


