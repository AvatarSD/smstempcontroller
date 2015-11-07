/*
 * NetworkWorker.cpp
 *
 *  Created on: 25 вер. 2015 р.
 *      Author: sd
 */

#include "NetworkWorker.h"
#include "avr/interrupt.h"
#include "../LOG/debug.h"

/************ Network Settings ************/
#define NETWORK_TXINT USART2_TX_vect
#define NETWORK_RXINT USART2_RX_vect
#define NETWORK_PORT    UDR2

#define NETWORK_AP "www.kyivstar.net"
#define NTP_ADDR "pool.ntp.org"

#define NETWORK_SERVER_ADDR "termo.big-gps.com"//"avatarsd.com"
#define NETWORK_SERVER_PORT 3050

#define NUM_ATTEMP_TO_COMNNECT 10
/*****************************************/


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
gsm(NETWORK_PORT), inetIface(gsm),sensors(Sensors), HWdata(data)

{
	errorCount = 0;
	attemptCount = 0;
	errorPercent = 0;

	_gsm = &gsm;
}

bool NetworkWorker::sendTemp()
{
	bool retVal = false;
	attemptCount++;
	errorPercent = ((float)(errorCount*100))/attemptCount;
#ifdef LEVEL_DEBUG
	debug.print(F("[DEBUG]: "));
	debug.print(F("Error Percent is: "));
	debug.print((double)errorPercent);
	debug.print("%");
	debug.print(F("\r\n"));
#endif

	errorCount++;
	INFO(F("Starting network procedure..."));
	if(forceConnectToServer(NETWORK_SERVER_ADDR, NETWORK_SERVER_PORT))
	{
		unsigned long int data = gsm.getUNIXdate();
		const char * imei = gsm.getIMEI();
		char pktCountStr[6];
		static unsigned int pktCount = 0;
		sprintf(pktCountStr, "%5u", pktCount);
		DallasSensorData sensorData;
		sensors.readingInit();

		if(!inetIface.beginWriteInet())
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
		gsm(HWdata.isHaveVoltageSupply());
		gsm(",");
		gsm(HWdata.isCaseOpen());
		gsm(",");
		gsm(HWdata.getError());

		int i = 0;
		while(sensors.readOnce(sensorData))
		{
			i++;
			gsm("\r");
			gsm(sensorData.getROM().toString());
			gsm(",");
			gsm(sensorData.getTempStr());
		}
		gsm("&");
#ifdef LEVEL_INFO
		debug.print(F("[INFO]: Sensors count: ")); debug.print(i); debug(F("\r\n"));
#endif

		if(inetIface.endWriteInet())
		{
			char buf[9];
			if(gsm.getString("$", "&", buf, 8))
			{
				long int i;
				int flag;
				sscanf(buf, "%ld,%d", &i, &flag);
#ifdef LEVEL_INFO
				debug.print(F("[INFO]: Package number: ")); debug.print(i); debug(F("\r\n"));
#endif
				if(i == pktCount)
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
	for(int i = 0; i < NUM_ATTEMP_TO_COMNNECT; i++)
		if(gsm.forceON())
			if(gsm.isRegistered() == GSM::REG_REGISTERED)
				if(inetIface.attachGPRS(NETWORK_AP, "", ""))
					if(inetIface.connectTCP(server, port) == 1)
						return true;
	CRITICAL(F("Did not connect at 10 attempts"));
	gsm.forceOFF();
	return false;
}

bool NetworkWorker::disconnectWithPowerDown()
{
	inetIface.disconnectTCP();
	inetIface.dettachGPRS();
	if(gsm.forceOFF())
		return true;
	else return false;
}

bool NetworkWorker::refreshTime()
{
	for(int i = 0; i < NUM_ATTEMP_TO_COMNNECT; i++)
		if(gsm.forceON())
			if(gsm.isRegistered() == GSM::REG_REGISTERED)
				if(inetIface.refreshTime(NETWORK_AP, NTP_ADDR))
					return true;
	CRITICAL(F("Time wasn't connect at 10 attempts"));
	return false;
}
