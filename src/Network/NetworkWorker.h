/*
 * NetworkWorker.h
 *
 *  Created on: 25 вер. 2015 р.
 *      Author: sd
 */

#ifndef NETWORK_NETWORKWORKER_H_
#define NETWORK_NETWORKWORKER_H_

#include "../DallasOneWire/DallasTemp.h"
#include "GSM/inetGSM.h"
#include "GSM/SIM900.h"
#include "../HardwareData/HardwareData.h"

class NetworkWorker
{
public:
	NetworkWorker(DallasTemp & Sensors, HardwareData & data, ROM * buffer);

	bool sendTemp();
	bool refreshTime();

private:
	bool forceConnectToServer(const char* server, int port);
	bool disconnectWithPowerDown();

	unsigned long long int errorCount;
	unsigned long long int attemptCount;
	float errorPercent;

	SIMCOM900  gsm;
	InetGSM  inetIface;
	DallasTemp & sensors;
	HardwareData & HWdata;
	ROM * _romMainBuff;
};

#endif /* NETWORK_NETWORKWORKER_H_ */
