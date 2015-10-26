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

	double getVoltage();
	bool isHaveVoltageSupply();
	bool isCaseOpen();
	char getError();
};

#endif /* HARDWAREDATA_HARDWAREDATA_H_ */
