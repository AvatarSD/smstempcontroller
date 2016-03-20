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

#define SMS_BUFF_LEN 160


class NetworkWorker
{
public:
	NetworkWorker(DallasTemp & Sensors, HardwareData & data);

	void mainLoop();

	bool refreshModemTimeFromNTP();
	void refreshInternalTimeFromModem();

	void init();

private:
	void loadROMs();
	void saveROMs();
	void loadNodes();
	void saveNodes();

	bool setupSms();

	void parseSMS(const char* msg, const char* phone);
	void iterateNodes();

	void printSMS(const char* msg, const char* phone);

	void searchSensors(uint8_t searchAttempts, const char* phone);
	void returnStatus(const char* arg, const char* phone);
	void setNode(const char* arg, const char* phone);
	void showNode(const char* arg, const char* phone);
	void deleteNode(const char* arg, const char* phone);
	int8_t deleteNode(uint16_t num);
	void returnHelp(const char* phone);

	uint16_t getRomBuffSize();
	uint16_t getNodeBuffSize();

	char smsBuff[SMS_BUFF_LEN];
	char phoneBuff[PHONE_LEN];

	SIMCOM900  gsm;
	InetGSM inetIface;
	SMSGSM  smsIface;
	DallasTemp & sensors;
	HardwareData & HWdata;


	ROM _romBuff[ROM_MAINBUFF_SIZE];
	RuleNode _nodeBuff[RULENODE_BUFF_SIZE];
};

#endif /* NETWORK_NETWORKWORKER_H_ */
