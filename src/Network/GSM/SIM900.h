#ifndef SIMCOM900_H
#define SIMCOM900_H

#include "GSM.h"

class SIMCOM900 : public virtual GSM
{
public:
	SIMCOM900(volatile unsigned char & port);
	virtual ~SIMCOM900();
	int getCCI(char* cci);
	const char * getIMEI();
	const unsigned long int getUNIXdate();
	int sendSMS(const char* to, const char* msg);
	bool readCall(char* number, int nlength);
	bool call(char* number, unsigned int milliseconds);
	int readCellData(int &mcc, int &mnc, long &lac, long &cellid);
	char IsUserButtonPushed();
	bool forceON();
	bool forceOFF();

	// Phonebook's methods
	char GetPhoneNumber(char position, char *phone_number);
	char WritePhoneNumber(char position, char *phone_number);
	char DelPhoneNumber(char position);
	char ComparePhoneNumber(char position, char *phone_number);

	// turns off/on the speaker
	void SetSpeaker(char off_on);

	//call when module ask pin
	void pinProcedure();

private:
//	int configandwait(char* pin);
	int setPIN(char *pin);
	int changeNSIPmode(char);
	void turnPWR();

	void byteRecived(char rxByte);
};

#endif

