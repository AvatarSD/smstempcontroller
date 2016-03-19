/*
 * NetworkWorker.cpp
 *
 *  Created on: 25 вер. 2015 р.
 *      Author: sd
 */

#include "NetworkWorker.h"
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "../LOG/debug.h"
#include "../init/rtc.h"
#include "../config.h"
#include <stdio.h>

uint8_t eepromROMsBuff[ROM_MAINBUFF_SIZE * sizeof(ROM)] EEMEM;
uint8_t eepromNodesBuff[RULENODE_BUFF_SIZE * sizeof(RuleNode)] EEMEM;

UART * _gsm;
ISR(NETWORK_RXINT)
{
	_gsm->rx_byte_int();
}
ISR(NETWORK_TXINT)
{
	_gsm->tx_byte_int();
}

NetworkWorker::NetworkWorker(DallasTemp & Sensors, HardwareData & data) :
		gsm(NETWORK_PORT), inetIface(gsm), smsIface(gsm), sensors(Sensors), HWdata(
				data)
{
	_gsm = &gsm;

}

void NetworkWorker::mainLoop()
{
#define SMS_BUFF_LEN 160

	char smsBuff[SMS_BUFF_LEN];
	char phoneBuff[PHONE_LEN];

	while (!setupSms())
		;

	//if
	INFO("-------------\r\nNew SMS!!");
	char position = smsIface.IsSMSPresent(SMSGSM::SMS_ALL);
	INFO(position);
	SMSGSM::getsms_ret_val_enum ret = smsIface.GetSMS(position, phoneBuff,
			smsBuff, SMS_BUFF_LEN); // == SMSGSM::GETSMS_UNREAD_SMS)
	INFO(ret);
	if (ret > 0)
	{

		INFO(phoneBuff);
		INFO(smsBuff);
		//smsIface.DeleteSMS(position);
	};

}

bool NetworkWorker::refreshModemTimeFromNTP()
{
	for (int i = 0; i < NUM_ATTEMP_TO_NTP_COMNNECT; i++)
		if (gsm.forceON())
			if (gsm.isRegistered() == GSM::REG_REGISTERED)
				if (inetIface.refreshTime(NETWORK_AP, NTP_ADDR))
					return true;
	CRITICAL(F("Time wasn't connect at 10 attempts"));
	return false;
}

void NetworkWorker::refreshInternalTimeFromModem()
{
	setUnixTime(convertToUNIXtime(gsm.getDateTime()));
}

void NetworkWorker::loadROMs()
{
	eeprom_read_block(_romBuff, eepromROMsBuff,
	ROM_MAINBUFF_SIZE * sizeof(ROM));
}

void NetworkWorker::saveROMs()
{
	eeprom_write_block(_romBuff, eepromROMsBuff,
	ROM_MAINBUFF_SIZE * sizeof(ROM));
}

void NetworkWorker::loadNodes()
{
	eeprom_read_block(_nodeBuff, eepromNodesBuff,
	RULENODE_BUFF_SIZE * sizeof(RuleNode));
}

void NetworkWorker::saveNodes()
{
	eeprom_write_block(_nodeBuff, eepromNodesBuff,
	RULENODE_BUFF_SIZE * sizeof(RuleNode));
}

bool NetworkWorker::setupSms()
{
	for (int i = 0; i < NUM_ATTEMP_TO_SMS_SETUP; i++)
		if (gsm.forceON())
			if (gsm.isRegistered() == GSM::REG_REGISTERED)
				if (smsIface.InitSMSMemory() > 0)
					return true;
	CRITICAL(F("Modem wasn't initialized"));
	return false;
}

void NetworkWorker::init()
{
	loadROMs();
	loadNodes();

	uint16_t i = 0;
	INFO(F("Sensor ROMs in memory:"));
	for (; ((!_romBuff[i].isNull()) && (i < ROM_MAINBUFF_SIZE)); i++)
		INFO(_romBuff[i].toString());
	char buff[10];
	sprintf(buff, "count: %d", i);
	INFO(buff);

}
