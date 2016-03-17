/*
 * MainWorker.h
 *
 *  Created on: 26 груд. 2015
 *      Author: sd
 */

#ifndef MAINWORKER_MAINWORKER_H_
#define MAINWORKER_MAINWORKER_H_

#include "../config.h"
#include "../Network/NetworkWorker.h"

class MainWorker
{
public:
	MainWorker();

	void mainLoop();
	//void screensaver(int time);
//	void iterationTimerLoop();
//	void timerStart();
	//void timerStop();
	void startingProcedure();


private:
	DallasOneWire _sensorsIface;
	DallasTemp _sensors;
	NetworkWorker _network;


};

#endif /* MAINWORKER_MAINWORKER_H_ */
