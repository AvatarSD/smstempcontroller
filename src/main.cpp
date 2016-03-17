/*
 * _1wireTempNetworkController.cpp
 *
 * Created: 30.07.2015 2:51:47
 *  Author: sd
 */

#include "MainWorker/MainWorker.h"

int main()
{
	MainWorker worker;

	worker.startingProcedure();

	while (true)
		worker.mainLoop();

}
