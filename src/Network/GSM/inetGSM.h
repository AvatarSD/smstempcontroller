#ifndef _INETGSM_H_
#define _INETGSM_H_

#define BUFFERSIZE 1

#include "GSM.h"

#define RECONNECT_EACH_CONNECTIONS

class InetGSM
{
public:
	InetGSM(GSM& _gsm);

	int httpGET(const char* server, int port, const char* path, char* result, int resultlength);
	int httpPOST(const char* server, int port, const char* path, const char* parameters, char* result, int resultlength);

	// Fast and dirty solution. Should make a "mail" object. And by the moment it does not run.
	int openmail(char* server, char* loginbase64, char* passbase64, char* from, char* to, char* subj);
	int closemail();

	int attachGPRS(const char* domain, const char* dom1, const char* dom2);
	int dettachGPRS();

	int connectTCP(const char* server, unsigned int port);
	int disconnectTCP();
	int connectTCPServer(int port);
	bool connectedClient();

	int WriteINET(unsigned int outlen, char *outbuf);
	int  ReadINET(unsigned int inlen, char *inbuf);

	bool beginWriteInet();
	bool endWriteInet();

	//refresh internal RTC over NTP
	bool refreshTime(const char* domain, const char* ntp);

	// This runs, yes
	//int tweet(const char* token, const char* msg);

private:
	char _buffer[BUFFERSIZE];
	GSM& gsm;

};

#endif
