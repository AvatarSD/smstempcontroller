/*
 * SDCardLogger.h
 *
 *  Created on: 27 лист. 2015
 *      Author: sd
 */

#ifndef LOG_SDCARDLOGGER_H_
#define LOG_SDCARDLOGGER_H_

#include "../stream/Stream.h"

class SDCardLogger : public Stream
{
public:
	SDCardLogger();
	~SDCardLogger();

	void putch(char c);
	int  write(const unsigned char *outbuf, unsigned int outlen);
	bool preSendHandler();
	bool postSendHandler();
};

#endif /* LOG_SDCARDLOGGER_H_ */
