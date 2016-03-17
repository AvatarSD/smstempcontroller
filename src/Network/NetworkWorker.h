/*
 * NetworkWorker.h
 *
 *  Created on: 25 вер. 2015 р.
 *      Author: sd
 */

#ifndef NETWORK_NETWORKWORKER_H_
#define NETWORK_NETWORKWORKER_H_


#include "GSM/sms.h"
#include "GSM/SIM900.h"
#include "DallasOneWire/DallasTemp.h"
#include "HardwareData/HardwareData.h"

class NetworkWorker
{
public:
	NetworkWorker(DallasTemp & Sensors, HardwareData & data);

	bool mainLoop();
	bool refreshTime();

private:


	void loadROMs();
	void saveROMs();
	void loadNodes();
	void saveNodes();

	SIMCOM900  gsm;
	SMSGSM  smsIface;
	InetGSM inetIface;
	DallasTemp & sensors;
	HardwareData & HWdata;


	ROM _romBuff[ROM_MAINBUFF_SIZE];
	RuleNode _nodeBuff[RULENODE_BUFF_SIZE];
};

#endif /* NETWORK_NETWORKWORKER_H_ */
