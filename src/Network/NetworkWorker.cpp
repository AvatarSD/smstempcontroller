/*
 * NetworkWorker.cpp
 *
 *  Created on: 25 вер. 2015 р.
 *      Author: sd
 */

#include "NetworkWorker.h"
#include "avr/interrupt.h"
#include "../LOG/debug.h"
#include "../init/rtc.h"
#include "../config.h"

UART * _gsm;

ISR(NETWORK_RXINT)
{
	_gsm->rx_byte_int();
}
ISR(NETWORK_TXINT)
{
	_gsm->tx_byte_int();
}

NetworkWorker::NetworkWorker(DallasTemp & Sensors, HardwareData & data,
		ROM * buffer) :
		gsm(NETWORK_PORT), inetIface(gsm), sensors(Sensors), HWdata(data), _romMainBuff(
				buffer)

{
	errorCount = 0;
	attemptCount = 0;
	errorPercent = 0;

	_gsm = &gsm;
}

bool NetworkWorker::sendTemp()
{
#ifdef LEVEL_DEBUG
	bool retVal = false;
	attemptCount++;
	errorPercent = ((float) (errorCount * 100)) / attemptCount;

	char buff[40];
	sprintf(buff, "Error Percent is: %f%%", (double)errorPercent);
	DEBUG(buff);
#endif

	errorCount++;
	INFO(F("Starting network procedure..."));
	if (forceConnectToServer(NETWORK_SERVER_ADDR, NETWORK_SERVER_PORT))
	{
		unsigned long int data = gsm.getUNIXdate();
		setUnixTime(data);
		const char * imei = gsm.getIMEI();
		char pktCountStr[6];
		static unsigned int pktCount = 0;
		sprintf(pktCountStr, "%5u", pktCount);
		//DallasSensorData sensorData;
		//sensors.readingInit();

		if (!inetIface.beginWriteInet())
			return false;

		gsm("$");
		gsm(imei);
		gsm(",");
		gsm(pktCountStr);
		gsm(",");
		gsm(data);
		gsm(",");
		gsm(HWdata.getVoltage());
		gsm(",");
		gsm(!HWdata.didHadNoVoltageSupply());
		gsm(",");
		gsm(HWdata.didHadCaseOpen());
		gsm(",");
		gsm(HWdata.getError());

		uint16_t sensorsCount = 0, i = 0;
		for (; ((!_romMainBuff[i].isNull()) && (i < ROM_MAINBFF_SIZE)); i++)
		{
			double temp;
			for (uint8_t n = 0; n < NUM_OF_READING_ATEMPT; n++)
				if (sensors.readSensor(_romMainBuff[i], temp))
				{
					gsm("\r");
					gsm(_romMainBuff[i].toString());
					gsm(",");
					gsm(temp);
					sensorsCount++;
					break;
				}
		}

#ifdef LEVEL_INFO
		char charbuf[40];
		sprintf(charbuf, "Num of sensors in memory: %d", i);
		INFO(charbuf);

		sprintf(charbuf, "Read sensors count: %d", sensorsCount);
		INFO(charbuf);
#endif

		gsm("&");

		if (inetIface.endWriteInet())
		{
			char buf[9];
			if (gsm.getString("$", "&", buf, 8))
			{
				long int i;
				int flag;
				sscanf(buf, "%ld,%d", &i, &flag);
#ifdef LEVEL_INFO
				sprintf(charbuf, "Package number: %ld", i);
				INFO(charbuf);
#endif
				if (i == pktCount)
				{
					INFO(F("Server answered"));
					pktCount++;
					errorCount--;
					retVal = true;
				}
			}
		}
	}
	inetIface.disconnectTCP();
	return retVal;
}

bool NetworkWorker::forceConnectToServer(const char* server, int port)
{
	for (int i = 0; i < NUM_ATTEMP_TO_COMNNECT; i++)
		if (gsm.forceON())
			if (gsm.isRegistered() == GSM::REG_REGISTERED)
				if (inetIface.attachGPRS(NETWORK_AP, "", ""))
					if (inetIface.connectTCP(server, port) == 1)
						return true;
	CRITICAL(F("Did not connect at 10 attempts"));
	gsm.forceOFF();
	return false;
}

bool NetworkWorker::disconnectWithPowerDown()
{
	inetIface.disconnectTCP();
	inetIface.dettachGPRS();
	if (gsm.forceOFF())
		return true;
	else
		return false;
}

bool NetworkWorker::refreshTime()
{
	for (int i = 0; i < NUM_ATTEMP_TO_COMNNECT; i++)
		if (gsm.forceON())
			if (gsm.isRegistered() == GSM::REG_REGISTERED)
				if (inetIface.refreshTime(NETWORK_AP, NTP_ADDR))
					return true;
	CRITICAL(F("Time wasn't connect at 10 attempts"));
	return false;
}
