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
	voltage *= VOLTAGE_MULTIPLY_RATIO;

	char buff[30];
	sprintf(buff, "Battery voltage: %2.1fv", voltage);
	INFO(buff);

	return voltage;
}

bool HardwareData::isHaveVoltageSupply()
{
	//pinsSetup();
	return !((POWER_PIN >> POWER_PIN_NUM) & 0x01);
}

bool HardwareData::isCaseOpen()
{
	//pinsSetup();
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
	checkPins();

	if (noSupplyFlag)
	{
		WARNING(F("There was no mains voltage"));
		noSupplyFlag = 0;
		return true;
	}
	noSupplyFlag = 0;
	INFO(F("Mains supply is OK"));
	return false;
}

bool HardwareData::didHadCaseOpen()
{
	checkPins();

	if (caseOpenedFlag)
	{
		WARNING(F("Box was opened"));
		caseOpenedFlag = 0;
		return true;
	}
	caseOpenedFlag = 0;
	INFO(F("Case is OK"));
	return false;
}

void HardwareData::pinsSetup()
{
	POWER_DDR &= ~(1 << POWER_PIN_NUM);
	POWER_PORT |= (1 << POWER_PIN_NUM);

	CASE_DDR &= ~(1 << CASE_PIN_NUM);
	CASE_PORT |= (1 << CASE_PIN_NUM);

	NEWSRC_DDR &= ~(1 << NEWSRC_PIN_NUM);
	NEWSRC_PORT |= (1 << NEWSRC_PIN_NUM);

	ADDSRC_DDR &= ~(1 << ADDSRC_PIN_NUM);
	ADDSRC_PORT |= (1 << ADDSRC_PIN_NUM);
}

bool HardwareData::isNewSrhBtnPress()
{
	return !((NEWSRC_PIN >> NEWSRC_PIN_NUM) & 0x01);
}

bool HardwareData::isAddSrhBtnPress()
{
	return !((ADDSRC_PIN >> ADDSRC_PIN_NUM) & 0x01);
}
