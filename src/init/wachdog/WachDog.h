/*
 * wachdog.h
 *
 *  Created on: 11 січ. 2016
 *      Author: sd
 */

#ifndef INIT_WACHDOG_WACHDOG_H_
#define INIT_WACHDOG_WACHDOG_H_

#include <inttypes.h>

class WachDog
{
public:
	WachDog();
	void begin();
	void begin(uint16_t timeoutSec);
	void end();
	void doCheckpoint();
	void setTimeout(uint16_t sec);
	void _intReset();

private:
	uint16_t _timeout;
	int32_t _curr;
};

extern WachDog wachdog;

#endif /* INIT_WACHDOG_WACHDOG_H_ */
