################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/init/freeram.cpp \
../src/init/init.cpp \
../src/init/millis.cpp \
../src/init/rtc.cpp 

OBJS += \
./src/init/freeram.o \
./src/init/init.o \
./src/init/millis.o \
./src/init/rtc.o 

CPP_DEPS += \
./src/init/freeram.d \
./src/init/init.d \
./src/init/millis.d \
./src/init/rtc.d 


# Each subdirectory must supply rules for building sources it contributes
src/init/%.o: ../src/init/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: AVR C++ Compiler'
	avr-g++ -I/usr/lib/avr/include/std -Wall -O3 -fshort-enums -ffunction-sections -fdata-sections -funsigned-char -funsigned-bitfields -fno-exceptions -std=c++11 -fno-strict-aliasing -Wno-switch -mmcu=atmega2560 -DF_CPU=16000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


