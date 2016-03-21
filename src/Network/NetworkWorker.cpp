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
#include "../init/wachdog/WachDog.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <avr/pgmspace.h>

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
	while (!setupSms())
		;

	char position = smsIface.IsSMSPresent(SMSGSM::SMS_ALL);
	// issues not all sms parse
	auto ret = smsIface.GetSMS(position, phoneBuff, smsBuff, SMS_BUFF_LEN);
	smsIface.DeleteSMS(position);
	if ((ret == SMSGSM::GETSMS_READ_SMS) || (ret == SMSGSM::GETSMS_UNREAD_SMS))
	{
		printSMS(smsBuff, phoneBuff);
		parseSMS(smsBuff, phoneBuff);
	};

	iterateNodes();
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
	if (strstr(msg, "search") == msg)
	{
		uint8_t attmpt = strtol(strchr(msg, ' ') + 1, NULL, 10);
		if (attmpt <= 0)
			attmpt = DEFAULT_SEARCH_ATTEMPTS;
		else if (attmpt > MAX_SEARCH_ATTEMPTS)
			attmpt = MAX_SEARCH_ATTEMPTS;
		searchSensors(attmpt, phone);
	}
	else if (strstr(msg, "status") == msg)
	{
		const char * args = strchr(msg, ' ');
		if (args != NULL)
			args++;
		returnStatus(args, phone);

	}
	else if (strstr(msg, "setnode") == msg)
	{
		const char * args = strchr(msg, ' ');
		if (args != NULL)
			args++;
		setNode(args, phone);
	}
	else if (strstr(msg, "shownode") == msg)
	{
		const char * args = strchr(msg, ' ');
		if (args != NULL)
			args++;
		showNode(args, phone);
	}
	else if (strstr(msg, "deletenode") == msg)
	{
		const char * args = strchr(msg, ' ');
		if (args != NULL)
			args++;
		deleteNode(args, phone);
	}
	else if (strstr(msg, "disablenode") == msg)
	{
		const char * args = strchr(msg, ' ');
		if (args != NULL)
			args++;
		setNodeEnabled(false, args, phone);
	}
	else if (strstr(msg, "enablenode") == msg)
	{
		const char * args = strchr(msg, ' ');
		if (args != NULL)
			args++;
		setNodeEnabled(true, args, phone);
	}
	else if (strstr(msg, "help") == msg)
		returnHelp(phone);
	else
	{
		sprintf(smsBuff, "Invalid parse message, try send \"help\"");
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
	}
}

void NetworkWorker::iterateNodes()
{
	bool timeToAlarm = false;
	static unsigned long long int lasttime = 0;
	unsigned long long int currtime = getUnixTime();
	if (lasttime + ALARM_PERID * 60 < currtime)
	{
		lasttime = currtime;
		timeToAlarm = true;
	}

	const uint16_t nodeBuffSize = getNodeBuffSize();
	for (uint16_t i = 0; i < nodeBuffSize; i++)
	{
		wachdog.doCheckpoint();

		if (!_nodeBuff[i].isEnabled())
			continue;

		if ((_nodeBuff[i].isAlarmed()) && (!timeToAlarm))
			continue;

		double temperarure;
		bool readingOk = false;
		for (uint8_t n = 0; n < NUM_OF_READING_ATEMPT; n++)
			if (sensors.readSensor(_nodeBuff[i].getRom(), temperarure))
			{
				readingOk = true;
				break;
			}

		if (!readingOk)
		{
			_nodeBuff[i].setAlarmed(true);
			char buff[100];
			sprintf(buff, "Sensor wasn't read: %s",
					_nodeBuff[i].getRom().toString());
			WARNING(buff);
			smsIface.SendSMS(_nodeBuff[i].getPhone(), buff);
			continue;
		}

		else if ((temperarure < _nodeBuff[i].getMin())
				|| (temperarure > _nodeBuff[i].getMax()))
		{
			_nodeBuff[i].setAlarmed(true);
			char buff[100];
			sprintf(buff, "Node %d: Out of range [%d;%d]`C: %s: %3.1lf`C",
					i + 1, _nodeBuff[i].getMin(), _nodeBuff[i].getMax(),
					_nodeBuff[i].getRom().toString(), temperarure);
			WARNING(buff);
			smsIface.SendSMS(_nodeBuff[i].getPhone(), buff);
		}
		else
			_nodeBuff[i].setAlarmed(false);
	}
}

void NetworkWorker::searchSensors(uint8_t searchAttempts, const char* phone)
{
	char buff[40];
	sprintf(buff, "Search perform with %d attempts", searchAttempts);
	INFO(buff);

	smsIface.SendSMS(phone, "Searches perform... Wait");

	memset(_romBuff, 0, ROM_MAINBUFF_SIZE * sizeof(ROM));
	INFO(F("Saved sensor ROMs was cleaned"));

	uint16_t sensorsCount = 0;
	for (; searchAttempts > 0; searchAttempts--)
	{
		uint16_t newSensors;
		int8_t retVal = sensors.searchAllTempSensors(_romBuff,
		ROM_MAINBUFF_SIZE, newSensors);
		wachdog.doCheckpoint();
		if (retVal == 0)
		{
			sensorsCount += newSensors;
			for (uint16_t i = sensorsCount - newSensors; i < sensorsCount; i++)
			{
				char buff[30];
				sprintf(buff, "New sensor: %s", _romBuff[i].toString());
				INFO(buff);
			}
		}
		else if (retVal == -1)
		{
			CRITICAL(F("Line error"));
			smsIface.SendSMS(phone, "Critical error! Line not answer.");
			loadROMs();
			return;
		}
		else if (retVal == -2)
		{
			smsIface.SendSMS(phone, "Not enough memory for all sensors!");
			break;
		}

	}

	sprintf(buff, "Sensors founded: %d", sensorsCount);
	INFO(buff);

	INFO(F("Perform to saving founding ROMs in eeprom memory"));
	saveROMs();
	INFO(F("Saving successful"));

	if (sensorsCount == 0)
		smsIface.SendSMS(phone, "Sensors not found.");
	else
	{
		sprintf(smsBuff, "Found sensors: %d\r\n", sensorsCount);
		uint16_t byteCounter = strlen(smsBuff);
		for (uint16_t i = 0; i < sensorsCount; i++)
		{
			wachdog.doCheckpoint();
			char tempBuff[25];
			byteCounter += sprintf(tempBuff, "%d: %s\r\n", i + 1,
					_romBuff[i].toString());
			if (byteCounter < SMS_BUFF_LEN - 1)
				strcpy(smsBuff + strlen(smsBuff), tempBuff);
			else
			{
				smsIface.SendSMS(phone, smsBuff);
				strcpy(smsBuff, tempBuff);
				byteCounter = strlen(smsBuff);
			}
		}
		if (byteCounter != 0)
			smsIface.SendSMS(phone, smsBuff);
	}
}

void NetworkWorker::returnStatus(const char* arg, const char* phone)
{
	INFO(F("Sensors reading... Wait"));
	smsIface.SendSMS(phone, "Sensors reading... Wait");

	sprintf(smsBuff, "Status is: \r\n");
	uint16_t byteCounter = strlen(smsBuff);
	uint16_t i = 0;
	const uint16_t romBuffSize = getRomBuffSize();
	for (; i < romBuffSize; i++)
	{
		wachdog.doCheckpoint();
		char buff[35];

		double temp;
		if (sensors.readSensor(_romBuff[i], temp))
			byteCounter += sprintf(buff, "%d: %s: %4.1f`C\r\n", i + 1,
					_romBuff[i].toString(), temp);

		else
			byteCounter += sprintf(buff, "%d: %s: ERROR\r\n", i + 1,
					_romBuff[i].toString());

		if (byteCounter < SMS_BUFF_LEN - 1)
			strcpy(smsBuff + strlen(smsBuff), buff);
		else
		{
			smsIface.SendSMS(phone, smsBuff);
			strcpy(smsBuff, buff);
			byteCounter = strlen(smsBuff);
		}

		buff[strlen(buff) - 2] = '\0';
		INFO(buff);
	}
	if (i == 0)
	{
		smsIface.SendSMS(phone,
				"No any sensors available. Please, do \"search\".");
		return;
	}
	if (byteCounter != 0)
		smsIface.SendSMS(phone, smsBuff);
}

void NetworkWorker::setNode(const char* arg, const char* phone)
{
	uint16_t romNum;
	int16_t minTemp, maxTemp;
	char phoneBuff[PHONE_LEN + 1];

#define XSTR(A) STR(A)
#define STR(A) #A
	int8_t argNum = sscanf(arg, "%u,%d,%d,%" XSTR(PHONE_LEN) "s", &romNum,
			&minTemp, &maxTemp, phoneBuff);

	if ((argNum == 3) || (argNum == 4))
	{
		if (argNum == 3)
			strcpy(phoneBuff, phone);

		//check phone number length
		if (strlen(phoneBuff) == PHONE_LEN)
		{
			INFO(F("Phone number too long"));
			smsIface.SendSMS(phone, "Phone number too long");
			return;
		}

		//check number of rom

		if ((romNum > getRomBuffSize()) || (romNum <= 0))
		{
			INFO(F("Invalid number of sensor"));
			smsIface.SendSMS(phone, "Invalid number of sensor");
			return;
		}

		//check for range
		if (minTemp >= maxTemp)
		{
			INFO(F("Invalid temperature range"));
			smsIface.SendSMS(phone, "Invalid temperature range");
			return;
		}

		//temp node
		RuleNode node(_romBuff[romNum - 1], minTemp, maxTemp, phoneBuff);

		uint16_t i = 0;
		const uint16_t nodeBuffSize = getNodeBuffSize();
		for (; i < nodeBuffSize; i++)
			if (_nodeBuff[i] == node)
			{
				INFO(F("This node already exists"));
				smsIface.SendSMS(phone, "This node already exists");
				return;
			}
		if (i == RULENODE_BUFF_SIZE - 1)
		{
			INFO(F("Not enough memory to append node"));
			smsIface.SendSMS(phone, "Not enough memory to append node");
		}
		else
		{
			_nodeBuff[i] = node;
			saveNodes();
			sprintf(smsBuff, "Node %d appended:\r\n%d: %s\r\nMin temp: "
					"%d.0`C\r\nMax temp: %d.0`C\r\nPh: %s", i + 1, romNum,
					_nodeBuff[i].getRom().toString(), _nodeBuff[i].getMin(),
					_nodeBuff[i].getMax(), _nodeBuff[i].getPhone());
			INFO(smsBuff);
			smsIface.SendSMS(phone, smsBuff);
		}
	}
	else
	{
		sprintf(smsBuff, "Arguments not valid, try send \"help\"");
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
	}
}

void NetworkWorker::showNode(const char* arg, const char* phone)
{
	if (arg == NULL)
	{
		sprintf(smsBuff, "Need argument, try send \"help\"");
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
		return;
	}

	if (strlen(arg) > PHONE_LEN - 1)
	{
		sprintf(smsBuff, "Argument too long, max %d character", PHONE_LEN - 1);
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
		return;
	}

	char argBuff[PHONE_LEN];
	strcpy(argBuff, arg);

	enum SearchMode
	{
		ALL, NUMBER, PHONE
	};
	SearchMode mode = PHONE;
	uint16_t pos;
	if (strstr(argBuff, "all") == argBuff)
		mode = ALL;
	else if (strstr(argBuff, "my") == argBuff)
	{
		mode = PHONE;
		strcpy(argBuff, phone);
	}
	else if (sscanf(argBuff, "%u", &pos) == 1)
		if ((pos <= getRomBuffSize()) && (pos > 0))
			mode = NUMBER;

	if (mode == NUMBER)
	{
		sprintf(smsBuff, "Show node with sensor %s",
				_romBuff[pos - 1].toString());
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
	}
	else if (mode == PHONE)
	{
		sprintf(smsBuff, "Search %s in phone number", argBuff);
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
	}

	uint16_t byteCounter = 0;
	smsBuff[0] = 0;
	uint16_t i = 0;
	const uint16_t nodeBuffSize = getNodeBuffSize();
	uint16_t counter = 0;
	for (; i < nodeBuffSize; i++)
	{
		wachdog.doCheckpoint();

		if (mode == NUMBER)
		{
			if (_nodeBuff[i].getRom() != _romBuff[pos - 1])
				continue;
		}
		else if (mode == PHONE)
			if (strstr(_nodeBuff[i].getPhone(), argBuff) == NULL)
				continue;

		char tempBuff[100];
		byteCounter += sprintf(tempBuff,
				"%d node: %s, Min: %d, Max: %d, Ph: %s\r\n", i + 1,
				_nodeBuff[i].getRom().toString(), _nodeBuff[i].getMin(),
				_nodeBuff[i].getMax(), _nodeBuff[i].getPhone());

		if (byteCounter < SMS_BUFF_LEN - 1)
			strcpy(smsBuff + strlen(smsBuff), tempBuff);
		else
		{
			smsIface.SendSMS(phone, smsBuff);
			strcpy(smsBuff, tempBuff);
			byteCounter = strlen(smsBuff);
		}

		tempBuff[strlen(tempBuff) - 2] = '\0';
		INFO(tempBuff);

		counter++;
	}
	if (byteCounter != 0)
		smsIface.SendSMS(phone, smsBuff);
	if (counter == 0)
	{
		sprintf(smsBuff, "No any node found");
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
	}
	else
	{
		sprintf(smsBuff, "Found %d node(s)", counter);
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
	}
}

void NetworkWorker::deleteNode(const char* arg, const char* phone)
{
	if (arg == NULL)
	{
		INFO(F("Need argument"));
		smsIface.SendSMS(phone, "Need argument");
		return;
	}

	if (strlen(arg) > PHONE_LEN - 1)
	{
		INFO(F("Argument too long"));
		smsIface.SendSMS(phone, "Argument too long");
		return;
	}

	if (strstr(arg, "all") == arg)
	{
		memset(_nodeBuff, 0, RULENODE_BUFF_SIZE * sizeof(RuleNode));
		saveNodes();
		INFO(F("All nodes deleted"));
		smsIface.SendSMS(phone, "All nodes deleted");
		return;
	}

	uint16_t pos;
	if (sscanf(arg, "%u", &pos) == 1)
	{
		if (deleteNode(pos) == 0)
			sprintf(smsBuff, "%d node deleted", pos);
		else
			sprintf(smsBuff, "%d is invalid node number", pos);
		saveNodes();
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
		return;
	}

	sprintf(smsBuff, "\"%s\" not found", arg);
	INFO(smsBuff);
	smsIface.SendSMS(phone, smsBuff);
	return;

}

int8_t NetworkWorker::deleteNode(uint16_t num)
{
	uint16_t nodeBuffSize = getNodeBuffSize();
	if (num >= nodeBuffSize)
		return -1;
	_nodeBuff[num].zeroing();
	for (; num < nodeBuffSize - 1; num++)
		_nodeBuff[num] = _nodeBuff[num + 1];
	if (num < RULENODE_BUFF_SIZE - 1)
		_nodeBuff[num + 1].zeroing();
	return 0;
}

void NetworkWorker::returnHelp(const char* phone)
{
}

uint16_t NetworkWorker::getRomBuffSize()
{
	uint16_t i = 0;
	for (; ((i < ROM_MAINBUFF_SIZE) && (!_romBuff[i].isNull())); i++)
		;
	return i;
}

uint16_t NetworkWorker::getNodeBuffSize()
{
	uint16_t i = 0;
	for (; (i < (RULENODE_BUFF_SIZE) && (!_nodeBuff[i].getRom().isNull())); i++)
		;
	return i;
}

void NetworkWorker::setNodeEnabled(bool enable, const char* arg,
		const char* phone)
{
	if (arg == NULL)
	{
		sprintf(smsBuff, "Need argument, try send \"help\"");
		INFO(smsBuff);
		smsIface.SendSMS(phone, smsBuff);
		return;
	}

	uint16_t pos;
	if (sscanf(arg, "%u", &pos) == 1)
		if ((pos <= getRomBuffSize()) && (pos > 0))
		{
			_nodeBuff[pos - 1].setEnabled(enable);
			if (enable)
				sprintf(smsBuff, "%d node enabled", pos);
			else
				sprintf(smsBuff, "%d node disabled", pos);
			INFO(smsBuff);
			smsIface.SendSMS(phone, smsBuff);
			return;
		}

	sprintf(smsBuff, "Invalid argument");
	INFO(smsBuff);
	smsIface.SendSMS(phone, smsBuff);

}
