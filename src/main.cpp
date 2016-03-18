/*
 * _1wireTempNetworkController.cpp
 *
 * Created: 30.07.2015 2:51:47
 *  Author: sd
 */

#include "MainWorker/MainWorker.h"
#include "init/init.h"

int main()
{
	init();

	MainWorker worker;

	while (true);
		worker.mainLoop();
}
