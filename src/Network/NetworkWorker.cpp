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

	char position = smsIface.IsSMSPresent(SMSGSM::SMS_UNREAD);
	auto ret = smsIface.GetSMS(position, phoneBuff, smsBuff, SMS_BUFF_LEN);
	smsIface.DeleteSMS(position);
	if ((ret == SMSGSM::GETSMS_READ_SMS) || (ret == SMSGSM::GETSMS_UNREAD_SMS))
	{
		printSMS(smsBuff, phoneBuff);
		parseSMS(smsBuff, phoneBuff);
	};

	iterateNodes();

	//smsIface.SendSMS("+380635765200", "Heloo!");
}

bool NetworkWorker::refreshModemTimeFromNTP()
{
	for (int i = 0; i < NUM_ATTEMP_TO_NTP_COMNNECT; i++)
		if (gsm.forceON())
			if (gsm.isRegistered() == GSM::REG_REGISTERED)
				if (inetIface.refreshTime(NETWORK_AP, NTP_ADDR))
					return true;
	CRITICAL(F("Time wasn't refreshed at several attempts"));
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

	gsm.forceON();
}

void NetworkWorker::printSMS(const char* msg, const char* phone)
{
	char buff[200];
	sprintf(buff, "New SMS: Ph:%s, Msg:%s", phone, msg);
	INFO(buff);
}

void NetworkWorker::parseSMS(const char* msg, const char* phone)
{

}

void NetworkWorker::iterateNodes()
{
	for (uint16_t i = 0;
			((i < RULENODE_BUFF_SIZE) && (!_nodeBuff[i].getRom().isNull()));
			i++)
	{
		double temperarure;
		bool readingOk = false;
		for (uint8_t n = 0; n < NUM_OF_READING_ATEMPT; n++)
			if (sensors.readSensor(_nodeBuff[i].getRom(), temperarure))
			{
				readingOk = true;
				break;
			}
		if (readingOk == false)
		{
			char buff[100];
			sprintf(buff, "Sensor wasn't read: %s",
					_nodeBuff[i].getRom().toString());
			WARNING(buff);
			smsIface.SendSMS(_nodeBuff[i].getPhone(), buff);
			continue;
		}

		if ((temperarure < _nodeBuff[i].getMin())
				|| (temperarure > _nodeBuff[i].getMax()))
		{
			char buff[100];
			sprintf(buff, "Out of range [%d;%d]: %s", _nodeBuff[i].getMin(),
					_nodeBuff[i].getMax(), _nodeBuff[i].getRom().toString());
			WARNING(buff);
			smsIface.SendSMS(_nodeBuff[i].getPhone(), buff);
		}
	}

}

