/*
 * HardwareData.h
 *
 *  Created on: 30 вер. 2015 р.
 *      Author: sd
 */

#ifndef HARDWAREDATA_HARDWAREDATA_H_
#define HARDWAREDATA_HARDWAREDATA_H_

class HardwareData
{
public:
	HardwareData();

	void checkPins(); /** call this function regularity **/

	char getError();
	double getVoltage();
	bool didHadNoVoltageSupply();
	bool didHadCaseOpen();

	void pinsSetup();

private:
	bool isHaveVoltageSupply();
	bool isCaseOpen();

	bool caseOpenedFlag, noSupplyFlag;

};

extern HardwareData HWdata;

#endif /* HARDWAREDATA_HARDWAREDATA_H_ */
