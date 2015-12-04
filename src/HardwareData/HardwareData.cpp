/*
 * HardwareData.cpp
 *
 *  Created on: 30 вер. 2015 р.
 *      Author: sd
 */

#include <avr/io.h>
#include "HardwareData.h"
#include "ADC/Analog.h"
#include "../config.h"

HardwareData::HardwareData()
{
	// TODO Auto-generated constructor stub
	supplyFlag = 0;
	caseOpenedFlag = 0;
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
	POWER_DDR &= ~(1 << POWER_PIN_NUM);
	return (POWER_PIN >> POWER_PIN_NUM) & 0x01;
}

bool HardwareData::isCaseOpen()
{
	CASE_DDR &= ~(1 << CASE_PIN_NUM);
	return (CASE_PIN >> CASE_PIN_NUM) & 0x01;
}

char HardwareData::getError()
{
	return 0;
}

void HardwareData::checkPins()
{
	supplyFlag = isHaveVoltageSupply();
	caseOpenedFlag = isCaseOpen();
}

bool HardwareData::didHadVoltageSupply()
{
	bool ret = supplyFlag;
	supplyFlag = 0;
	return ret;
}

bool HardwareData::didHadCaseOpen()
{
	bool ret = caseOpenedFlag;
	caseOpenedFlag = 0;
	return ret;
}
