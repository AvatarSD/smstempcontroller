/*
 * DallasTemp.h
 *
 *  Created on: 1 вер. 2015 р.
 *      Author: sd
 */

#ifndef DALLASONEWIRE_DALLASTEMP_H_
#define DALLASONEWIRE_DALLASTEMP_H_


#include <list>
#include "DallasSensorData.h"


class DallasTemp {
public:
	DallasTemp(DallasOneWire& iface);

	const std::list<DallasSensorData>&  readAllTempSerial(bool isCurr = true);
	const std::list<DallasSensorData>&  readAllTempParalel(char attemptNum);

	void readingInit();
	int readOnce(DallasSensorData & data);
private:
	int justStartConversion();
	const std::list<DallasSensorData>& justGetTemp();
	float calculateTemperature(ROM & deviceAddress, unsigned char * scratchPad);

	std::list<DallasSensorData> _sensorsRes;
	DallasOneWire& _iface;
};

#endif /* DALLASONEWIRE_DALLASTEMP_H_ */
