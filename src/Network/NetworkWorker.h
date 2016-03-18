/*
 * NetworkWorker.h
 *
 *  Created on: 25 вер. 2015 р.
 *      Author: sd
 */

#ifndef NETWORK_NETWORKWORKER_H_
#define NETWORK_NETWORKWORKER_H_


#include "GSM/sms.h"
#include "GSM/inetGSM.h"
#include "GSM/SIM900.h"
#include "DallasOneWire/DallasTemp.h"
#include "HardwareData/HardwareData.h"
#include "RuleNode.h"

class NetworkWorker
{
public:
	NetworkWorker(DallasTemp & Sensors, HardwareData & data);

	void mainLoop();
	bool refreshTime();

	void init();

private:
	void loadROMs();
	void saveROMs();
	void loadNodes();
	void saveNodes();

	bool setupSms();


	SIMCOM900  gsm;
	InetGSM inetIface;
	SMSGSM  smsIface;
	DallasTemp & sensors;
	HardwareData & HWdata;


	ROM _romBuff[ROM_MAINBUFF_SIZE];
	RuleNode _nodeBuff[RULENODE_BUFF_SIZE];
};

#endif /* NETWORK_NETWORKWORKER_H_ */
