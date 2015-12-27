/*
 * MainWorker.h
 *
 *  Created on: 26 груд. 2015
 *      Author: sd
 */

#ifndef MAINWORKER_MAINWORKER_H_
#define MAINWORKER_MAINWORKER_H_

#include "../config.h"

class MainWorker
{
public:
	MainWorker();

	void mainLoop();
	void screensaver(int time);
	void iterationTimerLoop();
	void timerStart();
	void timerStop();
	void startingProcedure();
	void loadMainbuff();
	void saveMainbuff();

private:
	DallasOneWire _sensorsIface;
	DallasTemp _sensors;
	NetworkWorker _network;

	ROM _mainbuf[ROM_MAINBFF_SIZE];
};

#endif /* MAINWORKER_MAINWORKER_H_ */
