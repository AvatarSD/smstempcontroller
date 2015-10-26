/*
 * HardwareData.cpp
 *
 *  Created on: 30 вер. 2015 р.
 *      Author: sd
 */

#include <avr/io.h>
#include "HardwareData.h"
#include "../ADC/Analog.h"


/********* Settings ***********/
#define VOLTAGE_DIVIDER_RATIO 1
#define VOLTAGE_ANALOG_PIN 15
#define CASE_PIN_NUM 6
#define CASE_PIN PINK
#define CASE_DDR DDRK
#define POWER_PIN_NUM 5
#define POWER_PIN PINK
#define POWER_DDR DDRK
/******************************/


HardwareData::HardwareData()
{
	// TODO Auto-generated constructor stub

}

double HardwareData::getVoltage()
{
	double voltage = analog[VOLTAGE_ANALOG_PIN];
	voltage /= 400;
	voltage *= VOLTAGE_DIVIDER_RATIO;
	return voltage;
}

bool HardwareData::isHaveVoltageSupply()
{
	POWER_DDR &=~ (1 << POWER_PIN_NUM);
	return (POWER_PIN >> POWER_PIN_NUM) & 0x01;
}

bool HardwareData::isCaseOpen()
{
	CASE_DDR &=~ (1 << CASE_PIN_NUM);
	return (CASE_PIN >> CASE_PIN_NUM) & 0x01;
}

char HardwareData::getError()
{
	return 0;
}
