/*
 * HardwareData.cpp
 *
 *  Created on: 30 вер. 2015 р.
 *      Author: sd
 */

#include "stdio.h"
#include <avr/io.h>
#include "HardwareData.h"
#include "ADC/Analog.h"
#include "../config.h"
#include "../LOG/debug.h"

HardwareData HWdata;

HardwareData::HardwareData()
{
	noSupplyFlag = 0;
	caseOpenedFlag = 0;
}

double HardwareData::getVoltage()
{
	double voltage = analog[VOLTAGE_ANALOG_PIN];
	voltage /= 400;
	voltage *= VOLTAGE_DIVIDER_RATIO;

	char buff[30];
	sprintf(buff, "Battery voltage: %2.1fv", voltage);
	INFO(buff);

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
	DEBUG(F("No hardware error"));
	return 0;
}

void HardwareData::checkPins()
{
	if (!isHaveVoltageSupply())
		noSupplyFlag = 1;
	if (isCaseOpen())
		caseOpenedFlag = 1;
}

bool HardwareData::didHadNoVoltageSupply()
{
	if (noSupplyFlag)
	{
		WARNING(F("There was no mains voltage"));
		noSupplyFlag = 0;
		return true;
	}
	INFO(F("Mains supply is OK"));
	return false;
}

bool HardwareData::didHadCaseOpen()
{
	if (caseOpenedFlag)
	{
		WARNING(F("Box was opened"));
		caseOpenedFlag = 0;
		return true;
	}
	return false;
}
