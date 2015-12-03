/*
 * SDCardLogger.h
 *
 *  Created on: 27 лист. 2015
 *      Author: sd
 */

#ifndef LOG_SDCARDLOGGER_H_
#define LOG_SDCARDLOGGER_H_

#include "../stream/Stream.h"
#include "../sdcard/ff.h"
#include "../init/rtc.h"

class SDCardLogger : public Stream
{
public:
	SDCardLogger();
	~SDCardLogger();

	int begin();
	int end();

	void putch(char c);
	int  write(const unsigned char *outbuf, unsigned int outlen);
	bool preSendHandler();
	bool postSendHandler();

private:
	FATFS fatFs;	// FatFs work area
	FIL logFile;	// file object
};

extern SDCardLogger debugSDcardLog;

#endif /* LOG_SDCARDLOGGER_H_ */
