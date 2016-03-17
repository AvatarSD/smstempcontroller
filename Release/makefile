################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/stream/subdir.mk
-include src/init/wachdog/subdir.mk
-include src/init/subdir.mk
-include src/UART/subdir.mk
-include src/Network/HardwareData/ADC/subdir.mk
-include src/Network/HardwareData/subdir.mk
-include src/Network/GSM/subdir.mk
-include src/Network/DallasOneWire/subdir.mk
-include src/Network/subdir.mk
-include src/MainWorker/subdir.mk
-include src/LOG/sdcard/uni_avr/subdir.mk
-include src/LOG/sdcard/subdir.mk
-include src/LOG/LiquidCrystal_I2C/pcf8574/subdir.mk
-include src/LOG/LiquidCrystal_I2C/lcdpcf8574/subdir.mk
-include src/LOG/LiquidCrystal_I2C/lcdTerminal/subdir.mk
-include src/LOG/LiquidCrystal_I2C/i2chw/subdir.mk
-include src/LOG/subdir.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
gsmonewire.lss \

FLASH_IMAGE += \
gsmonewire.hex \

EEPROM_IMAGE += \
gsmonewire.eep \

SIZEDUMMY += \
sizedummy \

AVRDUDEDUMMY += \
avrdudedummy \


# All Target
all: gsmonewire.elf secondary-outputs

# Tool invocations
gsmonewire.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C++ Linker'
	avr-g++ -Wl,-Map,gsmonewire.map,--cref -mrelax -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt -lm -mmcu=atmega2560 -o "gsmonewire.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

gsmonewire.lss: gsmonewire.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S gsmonewire.elf  >"gsmonewire.lss"
	@echo 'Finished building: $@'
	@echo ' '

gsmonewire.hex: gsmonewire.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex gsmonewire.elf  "gsmonewire.hex"
	@echo 'Finished building: $@'
	@echo ' '

gsmonewire.eep: gsmonewire.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex gsmonewire.elf  "gsmonewire.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: gsmonewire.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega2560 gsmonewire.elf
	@echo 'Finished building: $@'
	@echo ' '

avrdudedummy: gsmonewire.elf
	@echo 'Invoking: AVRDude'
	/usr/bin/avrdude -pm2560 -cwiring -P/dev/ttyACM0 -D -Uflash:w:gsmonewire.hex:a
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(C_UPPER_DEPS)$(AVRDUDEDUMMY)$(S_DEPS)$(LSS)$(C_DEPS)$(FLASH_IMAGE)$(CC_DEPS)$(ELFS)$(C++_DEPS)$(OBJS)$(CXX_DEPS)$(ASM_DEPS)$(EEPROM_IMAGE)$(SIZEDUMMY)$(S_UPPER_DEPS)$(CPP_DEPS) gsmonewire.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY) $(AVRDUDEDUMMY)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
