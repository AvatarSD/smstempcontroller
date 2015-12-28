/*
 * _1wireTempNetworkController.cpp
 *
 * Created: 30.07.2015 2:51:47
 *  Author: sd
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "MainWorker/MainWorker.h"
#include "init/init.h"

MainWorker * _worker;
ISR(TIMER1_CAPT_vect)
{
	_worker->iterationTimerLoop();
}

int main()
{
	init();

	MainWorker worker;
	_worker = &worker;

	worker.startingProcedure();

	while (true)
		;
}
