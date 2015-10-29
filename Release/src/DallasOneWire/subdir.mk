################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/DallasOneWire/DallasOneWire.cpp \
../src/DallasOneWire/DallasSensorData.cpp \
../src/DallasOneWire/DallasTemp.cpp \
../src/DallasOneWire/ROM.cpp 

OBJS += \
./src/DallasOneWire/DallasOneWire.o \
./src/DallasOneWire/DallasSensorData.o \
./src/DallasOneWire/DallasTemp.o \
./src/DallasOneWire/ROM.o 

CPP_DEPS += \
./src/DallasOneWire/DallasOneWire.d \
./src/DallasOneWire/DallasSensorData.d \
./src/DallasOneWire/DallasTemp.d \
./src/DallasOneWire/ROM.d 


# Each subdirectory must supply rules for building sources it contributes
src/DallasOneWire/%.o: ../src/DallasOneWire/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I/usr/lib/avr/include/std -Wall -O3 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -std=c++11 -fno-strict-aliasing -Wno-switch -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


