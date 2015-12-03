/*
This is a Beta version.
last modified 18/08/2012.

This library is based on one developed by Arduino Labs
and it is modified to preserve the compability
with the Arduino's product.

The library is modified to use the GSM Shield,
developed by www.open-electronics.org
(http://www.open-electronics.org/arduino-gsm-shield/)
and based on SIM900 chip,
with the same commands of Arduino Shield,
based on QuectelM10 chip.
 */

#include "GSM.h"
#include "util/delay.h"
#include "../../init/millis.h"
#include "../../LOG/debug.h"
#define delay(x) _delay_ms(x)

#include "avr/io.h"

#include <string.h>
#include <stdio.h>

GSM::GSM(volatile unsigned char & port) : UART(port, 9600, 64, 16)
{
	millisInit();

	setGSMStatus(GSM_IDLE);
	setCOMStatus(CLS_FREE);
	setRegistrationStatus(REG_NOT_REGISTERED);

	p_comm_buf = 0;
	comm_buf_len = 0;

	rx_state = RX_NOT_STARTED;
	start_reception_tmout = 0;
	interchar_tmout = 0;
	prev_time = 0;

	last_speaker_volume = 0;
};

bool GSM::isResponse()
{
	if (AT_RESP_OK == SendATCmdWaitResp("AT", 500, 100, "OK", 3))
		return true;
	else
	{
		DEBUG(F("Modem not answered OK"));
		return false;
	}
}

//
//int GSM::begin(long baud_rate)
//{
//	//int response=-1;
//	int cont=0;
//	bool norep=true;
//	bool turnedON=false;
//	SetCommLineStatus(CLS_ATCMD);
//	begin(baud_rate);
//	p_comm_buf = &comm_buf[0];
//	setStatus(IDLE);
//
//	// if no-reply we turn to turn on the module
//	for (cont=0; cont<3; cont++){
//		if (AT_RESP_ERR_NO_RESP == SendATCmdWaitResp("AT", 500, 100, "OK", 5)&&!turnedON)
//		{		//check power
//			// there is no response => turn on the module
//			// generate turn on pulse
//			digitalWrite(GSM_ON, HIGH);
//			delay(1200);
//			digitalWrite(GSM_ON, LOW);
//			delay(10000);
//			WaitResp(1000, 1000);
//		}
//		else{
//			WaitResp(1000, 1000);
//		}
//	}
//
//
//	if (AT_RESP_OK == SendATCmdWaitResp("AT", 500, 100, "OK", 5)){
//		turnedON=true;
//		norep=false;
//	}
//
//
//	if (AT_RESP_ERR_DIF_RESP == SendATCmdWaitResp("AT", 500, 100, "OK", 5)&&!turnedON)
//	{		//check OK
//		for (int i=0;i<8;i++){
//			switch (i) {
//			case 0:
//				begin(1200);
//				break;
//
//			case 1:
//				begin(2400);
//				break;
//
//			case 2:
//				begin(4800);
//				break;
//
//			case 3:
//				begin(9600);
//				break;
//
//			case 4:
//				begin(19200);
//				break;
//
//			case 5:
//				begin(38400);
//				break;
//
//			case 6:
//				begin(57600);
//				break;
//
//			case 7:
//				begin(115200);
//				break;
//
//				// if nothing else matches, do the default
//				// default is optional
//			}
//
//			delay(100);
//
//			if (AT_RESP_OK == SendATCmdWaitResp("AT", 500, 100, "OK", 5))
//			{
//				print("AT+IPR=");
//				print(baud_rate);
//				print("\r"); // send <CR>
//				delay(500);
//				begin(baud_rate);
//				delay(100);
//				if (AT_RESP_OK == SendATCmdWaitResp("AT", 500, 100, "OK", 5))
//				{
//#ifdef DEBUG_ON
//					Serial.println("DB:OK BR");
//#endif
//				}
//				turnedON=true;
//				break;
//			}
//		}
//		// communication line is not used yet = free
//		SetCommLineStatus(CLS_FREE);
//		// pointer is initialized to the first item of comm. buffer
//		p_comm_buf = &comm_buf[0];
//	}
//
//	if(norep==true&&!turnedON)
//	{
//		//Serial.println("Trying to force the baud-rate to 9600\n");
//		switch (i) {
//		case 0:
//			begin(1200);
//			delay(1000);
//			//	Serial.println("1200");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//
//		case 1:
//			begin(2400);
//			delay(1000);
//			//		Serial.println("2400");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//
//		case 2:
//			begin(4800);
//			delay(1000);
//			//	Serial.println("4800");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//
//		case 3:
//			begin(9600);
//			delay(1000);
//			//	Serial.println("9600");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//
//		case 4:
//			begin(19200);
//			delay(1000);
//			//	Serial.println("19200");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//
//		case 5:
//			begin(38400);
//			delay(1000);
//			//	Serial.println("38400");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//
//		case 6:
//			begin(57600);
//			delay(1000);
//			//	Serial.println("57600");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//
//		case 7:
//			begin(115200);
//			delay(1000);
//			//	Serial.println("115200");
//			print("AT+IPR=9600\r");
//			delay(1000);
//			begin(9600);
//			delay(1000);
//			SendATCmdWaitResp("AT", 500, 100, "OK", 5);
//			delay(1000);
//			WaitResp(1000,1000);
//			break;
//		}
//	}
//
//	//	Serial.println("ERROR: SIM900 doesn't answer. Check power and serial pins in GSM.cpp");
//	//	digitalWrite(GSM_ON, HIGH);
//	//	delay(1200);
//	//	digitalWrite(GSM_ON, LOW);
//	//	delay(10000);
//	return 0;
//
//
//	SetCommLineStatus(CLS_FREE);
//
//	if(turnedON)
//	{
//		WaitResp(50, 50);
//		InitParam(PARAM_SET_0);
//		InitParam(PARAM_SET_1);//configure the module
//		Echo(0);               //enable AT echo
//		setStatus(READY);
//		return(1);
//
//	}
//	else{
//		//just to try to fix some problems with 115200 baudrate
//		begin(115200);
//		delay(1000);
//		print("AT+IPR=");
//		print(baud_rate);
//		print("\r"); // send <CR>
//		return(0);
//	}
//}
//


void GSM::InitParam(char group)
{
	switch (group)
	{
	case PARAM_SET_0:
		DEBUG(F("Init Param: Set 0"));
		// check comm line
		if (CLS_FREE != getCOMStatus()) return;

		setCOMStatus(CLS_ATCMD);
		// Reset to the factory settings
		SendATCmdWaitResp("AT&F", 1000, 50, "OK", 5);      
		// switch off echo
		SendATCmdWaitResp("ATE0", 500, 50, "OK", 5);
		// setup fixed baud rate
		//SendATCmdWaitResp("AT+IPR=9600", 500, 50, "OK", 5);
		// setup mode
		//SendATCmdWaitResp("AT#SELINT=1", 500, 50, "OK", 5);
		// Switch ON User LED - just as signalization we are here
		//SendATCmdWaitResp("AT#GPIO=8,1,1", 500, 50, "OK", 5);
		// Sets GPIO9 as an input = user button
		//SendATCmdWaitResp("AT#GPIO=9,0,0", 500, 50, "OK", 5);
		// allow audio amplifier control
		//SendATCmdWaitResp("AT#GPIO=5,0,2", 500, 50, "OK", 5);
		// Switch OFF User LED- just as signalization we are finished
		//SendATCmdWaitResp("AT#GPIO=8,0,1", 500, 50, "OK", 5);
		setCOMStatus(CLS_FREE);
		break;

	case PARAM_SET_1:
		DEBUG(F("Init Param: Set 1"));
		// check comm line
		if (CLS_FREE != getCOMStatus()) return;
		setCOMStatus(CLS_ATCMD);
		// Request calling line identification
		SendATCmdWaitResp("AT+CLIP=1", 500, 50, "OK", 5);
		// Mobile Equipment Error Code
		SendATCmdWaitResp("AT+CMEE=0", 500, 50, "OK", 5);
		// Echo canceller enabled 
		//SendATCmdWaitResp("AT#SHFEC=1", 500, 50, "OK", 5);
		// Ringer tone select (0 to 32)
		//SendATCmdWaitResp("AT#SRS=26,0", 500, 50, "OK", 5);
		// Microphone gain (0 to 7) - response here sometimes takes 
		// more than 500msec. so 1000msec. is more safety
		//SendATCmdWaitResp("AT#HFMICG=7", 1000, 50, "OK", 5);
		// set the SMS mode to text 
		SendATCmdWaitResp("AT+CMGF=1", 500, 50, "OK", 5);
		// Auto answer after first ring enabled
		// auto answer is not used
		//SendATCmdWaitResp("ATS0=1", 500, 50, "OK", 5);
		// select ringer path to handsfree
		//SendATCmdWaitResp("AT#SRP=1", 500, 50, "OK", 5);
		// select ringer sound level
		//SendATCmdWaitResp("AT+CRSL=2", 500, 50, "OK", 5);
		// we must release comm line because SetSpeakerVolume()
		// checks comm line if it is free
		setCOMStatus(CLS_FREE);
		// select speaker volume (0 to 14)
		//SetSpeakerVolume(9);
		// init SMS storage
		//InitSMSMemory();
		// select phonebook memory storage
		SendATCmdWaitResp("AT+CPBS=\"SM\"", 1000, 50, "OK", 5);
		SendATCmdWaitResp("AT+CIPSHUT", 500, 50, "SHUT OK", 5);

		SendATCmdWaitResp("AT+CLTS=1", 500, 50, "OK", 5);
		break;
	}
}



GSM::RXstateRes GSM::WaitResp(unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout)
{
	RXstateRes status;

	RxInit(start_comm_tmout, max_interchar_tmout);
	DATA(F("wait until response is not finished"));
	// wait until response is not finished
	do {
		status = IsRxFinished();
	} while (status == RX_NOT_FINISHED);
	DATA(F("status != RX_NOT_FINISHED"));
	return (status);
}

GSM::RXstateRes GSM::WaitResp(unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout,
		char const *expected_resp_string)
{
	char status;
	RXstateRes ret_val;

	status = WaitResp(start_comm_tmout, max_interchar_tmout);

	if (status == RX_FINISHED) {
		// something was received but what was received?
		// ---------------------------------------------

		if(IsStringReceived(expected_resp_string)) {
			// expected string was received
			// ----------------------------
			ret_val = RX_FINISHED_STR_RECV;
		}
		else {
			ret_val = RX_FINISHED_STR_NOT_RECV;
		}
	}
	else {
		// nothing was received
		// --------------------
		ret_val = RX_TMOUT_ERR;
	}
	return (ret_val);
}

/**********************************************************
Method sends AT command and waits for response

return: 
      AT_RESP_ERR_NO_RESP = -1,   // no response received
      AT_RESP_ERR_DIF_RESP = 0,   // response_string is different from the response
      AT_RESP_OK = 1,             // response_string was included in the response
 **********************************************************/
GSM::ATresp GSM::SendATCmdWaitResp(char const *AT_cmd_string,
		unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout,
		char const *response_string,
		char no_of_attempts)
{
	DATA(F("SendATCmdWaitResp"));

	RXstateRes status;
	ATresp ret_val = AT_RESP_ERR_NO_RESP;
	char i;

	for (i = 0; i < no_of_attempts; i++)
	{
		// delay 500 msec. before sending next repeated AT command
		// so if we have no_of_attempts=1 tmout will not occurred
		if (i > 0)
		{
			delay(500);
			DEBUG(F("SendATCmdWaitResp: next attempts"));
		}
		DATA(AT_cmd_string);

		print(AT_cmd_string);
		print(F("\r\n"));

		status = WaitResp(start_comm_tmout, max_interchar_tmout);
		if (status == RX_FINISHED)
		{
			// something was received but what was received?
			// ---------------------------------------------

			if(IsStringReceived(response_string))
			{
				ret_val = AT_RESP_OK;
				DATA(response_string);
				break;  // response is OK => finish
			}
			else ret_val = AT_RESP_ERR_DIF_RESP;
		}
		else
		{
			DEBUG(F("SendATCmdWaitResp: error or nothing received"));
			// nothing was received
			// --------------------
			ret_val = AT_RESP_ERR_NO_RESP;
		}

	}
	return (ret_val);
}

/**********************************************************
Method checks received bytes

compare_string - pointer to the string which should be find

return: 0 - string was NOT received
        1 - string was received
 **********************************************************/
bool GSM::IsStringReceived(char const *compare_string)
{
	if(comm_buf_len > 0)
		if (strstr(comm_buf, compare_string) != NULL)
			return 1;
	return 0;
}

GSM::RXstateRes GSM::IsRxFinished(void)
{
	char num_of_bytes;
	RXstateRes ret_val = RX_NOT_FINISHED;  // default not finished

	// Rx state machine
	// ----------------

	if (rx_state == RX_NOT_STARTED)
	{
		// Reception is not started yet - check tmout
		if (available() == 0)
		{
			// still no character received => check timeout
			if ((unsigned long int)(millis() - prev_time) >= start_reception_tmout)
			{
				// timeout elapsed => GSM module didn't start with response
				// so communication is takes as finished
				comm_buf[comm_buf_len] = 0x00;
				ret_val = RX_TMOUT_ERR;
			}
		}
		else {
			// at least one character received => so init inter-character
			// counting process again and go to the next state
			prev_time = millis(); // init tmout for inter-character space
			rx_state = RX_ALREADY_STARTED;
		}
	}

	if (rx_state == RX_ALREADY_STARTED)
	{

		// Reception already started
		// check new received bytes
		// only in case we have place in the buffer
		num_of_bytes = available();
		// if there are some received bytes postpone the timeout
		if (num_of_bytes) prev_time = millis();

		// read all received bytes
		while (num_of_bytes > 0)
		{
			num_of_bytes--;
			if (comm_buf_len < COMM_BUF_LEN)
			{
				// we have still place in the GSM internal comm. buffer =>
				// move available bytes from circular buffer
				// to the rx buffer
				*p_comm_buf = getch();


				p_comm_buf++;
				comm_buf_len++;
				comm_buf[comm_buf_len] = 0x00;  // and finish currently received characters
				// so after each character we have
				// valid string finished by the 0x00
			}
			else {
				// comm buffer is full, other incoming characters
				// will be discarded
				// but despite of we have no place for other characters
				// we still must to wait until
				// inter-character tmout is reached

				// so just readout character from circular RS232 buffer
				// to find out when communication id finished(no more characters
				// are received in inter-char timeout)
				getch();
			}
		}

		// finally check the inter-character timeout
		if ((unsigned long int)(millis() - prev_time) >= interchar_tmout)
		{
			// timeout between received character was reached
			// reception is finished
			// ---------------------------------------------
			comm_buf[comm_buf_len] = 0x00;  // for sure finish string again
			// but it is not necessary
			DATA(F("Rx Finished"));
			DATA(comm_buf);
			ret_val = RX_FINISHED;
		}
	}


	return (ret_val);
}

void GSM::RxInit(unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout)
{
#ifdef LEVEL_DATA
	char buff[40];
	sprintf(buff, "RxInit: %d, %d", start_comm_tmout, max_interchar_tmout);
	DATA(buff);
#endif

	rx_state = RX_NOT_STARTED;
	start_reception_tmout = start_comm_tmout;
	interchar_tmout = max_interchar_tmout;
	prev_time = millis();
	comm_buf[0] = 0x00; // end of string
	p_comm_buf = comm_buf;
	comm_buf_len = 0;
	flush(); // erase rx circular buffer
}

/**********************************************************
Method checks if the GSM module is registered in the GSM net
- this method communicates directly with the GSM module
  in contrast to the method IsRegistered() which reads the
  flag from the module_status (this flag is set inside this method)

- must be called regularly - from 1sec. to cca. 10 sec.

return values:
      REG_NOT_REGISTERED  - not registered
      REG_REGISTERED      - GSM module is registered
      REG_NO_RESPONSE     - GSM doesn't response
      REG_COMM_LINE_BUSY  - comm line between GSM module and Arduino is not free
                            for communication
 **********************************************************/
GSM::RegistrationStatus GSM::CheckRegistration(void)
{
	RXstateRes status;
	RegistrationStatus ret_val = REG_NOT_REGISTERED;

	if (CLS_FREE != getCOMStatus()) return REG_COMM_LINE_BUSY;
	setCOMStatus(CLS_ATCMD);
	print(F("AT+CREG?"));
	print("\r");
	// 5 sec. for initial comm tmout
	// 50 msec. for inter character timeout
	status = WaitResp(5000, 50);

	if (status == RX_FINISHED) {
		// something was received but what was received?
		// ---------------------------------------------
		if(IsStringReceived("+CREG: 0,1")
				|| IsStringReceived("+CREG: 0,5")) {
			// it means module is registered
			// ----------------------------
			setRegistrationStatus(REG_REGISTERED);


			// in case GSM module is registered first time after reset
			// sets flag STATUS_INITIALIZED
			// it is used for sending some init commands which
			// must be sent only after registration
			// --------------------------------------------
			if (!isInitialized())
			{
				setGSMStatus(GSM_INITIALIZED);
				setCOMStatus(CLS_FREE);
				InitParam(PARAM_SET_1);
				DEBUG(F("Status: Initialized"));
			}
			ret_val = REG_REGISTERED;
			INFO(F("Status: Registered"));
		}
		else {
			// NOT registered
			// --------------
			setRegistrationStatus(REG_NOT_REGISTERED);
			ret_val = REG_NOT_REGISTERED;
			DATA(F("Status: Not registered"));
		}
	}
	else {
		// nothing was received
		// --------------------
		ret_val = REG_NO_RESPONSE;
		WARNING(F("Status: Not response"));
	}
	setCOMStatus(CLS_FREE);


	return (ret_val);
}

GSM::RegistrationStatus GSM::isRegistered(void)
{
#define NUM_OF_ATTEMPT 40
#define TIME_OF_ATTRMPT 10000

	DEBUG(F("Checking registration..."));
	RegistrationStatus retVal;
	for(int i = 0; i <= NUM_OF_ATTEMPT; i++)
	{
		retVal = CheckRegistration();
		if(retVal == REG_REGISTERED)
		{
			DEBUG(F("Time is"));
			DEBUG(i * (TIME_OF_ATTRMPT/NUM_OF_ATTEMPT));
			break;;
		}
		if(i < NUM_OF_ATTEMPT)
			delay(TIME_OF_ATTRMPT/NUM_OF_ATTEMPT);
		else WARNING(F("Not Registered"));
	}

#ifdef LEVEL_INFO
	debug(F("[INFO]: ")); debug(F("Signal level is: "));
	debug(getSignalLevel()); debug(F(" (0-31 or 99)\r\n"));
#endif

	return retVal;

}

void GSM::Echo(char state)
{
	if (state == 0 or state == 1)
	{
		setCOMStatus(CLS_ATCMD);

		print("ATE");
		print((int)state);
		print("\r");
		delay(500);
		setCOMStatus(CLS_FREE);
	}
}

int GSM::isIP(const char* cadena)
{
	for (unsigned int i=0; i<strlen(cadena); i++)
		if (!(cadena[i]=='.' || ( cadena[i]>=48 && cadena[i] <=57)))
			return 0;
	return 1;
}

char * GSM::readIPfromBuff()
{
	unsigned char pIPstr = 0;
	unsigned int pBuff = strlen(comm_buf);
	if(comm_buf[--pBuff] != '\n')//||(pBuff < 6))
		return 0;
	pBuff -= 2;
	while((comm_buf[--pBuff] != '\n')||(pBuff == 0));
	while(comm_buf[++pBuff] != '\r')
		ipaddr[pIPstr++] = comm_buf[pBuff];
	ipaddr[pIPstr] = '\0';
	return ipaddr;
}

char GSM::isInitialized(void)
{
	return (_registrationStatus & STATUS_INITIALIZED);
}


//void GSM::SimpleWrite(const char *comm)
//{
//	print(comm);
//}
//
//void GSM::SimpleWrite(int comm)
//{
//	print(comm);
//}

//int GSM::read(char *inbuf, unsigned int inlen)
//{
//	return ReadCOM(inlen, (unsigned char*)inbuf);
//}

volatile GSM::COMStatus GSM::getCOMStatus(void)
{
	if(_clsStatus != CLS_FREE) DEBUG(F("COMM LINE BUSY"));
	return _clsStatus;
}

void GSM::setGSMStatus(GSMStatus status)
{
	_gsmSstatus = status;
}

void GSM::setCOMStatus(COMStatus  new_status)
{
	_clsStatus = new_status;
};

void GSM::setRegistrationStatus(RegistrationStatus  new_status)
{
	_registrationStatus = new_status;
}

void GSM::setGPRSStatus(GPRSStatus status)
{
	_gprsStatus = status;
}


/*
 * Already need to have pre and post string
 * Length is max length
 */
int GSM::getString(const char *pre_string, const char *post_string, char *buffer, int length)
{
	char * pBegin = strstr(comm_buf, pre_string) + strlen(pre_string);
	char * pEnd = strstr(pBegin+1, post_string);

	int i = 0, retVal;
	if((pBegin != NULL) && (pEnd != NULL))
	{
		for( ;	((pBegin != pEnd)&&(i < length)); pBegin++, i++)
			buffer[i] = *pBegin;
		retVal = 1;
	}
	else retVal = 0;
	buffer[i] = '\0';
	return retVal;
}

int GSM::getSignalLevel()
{
	print(F("AT+CSQ\r"));
	WaitResp(2000, 300);
	char buf[7];
	getString(":", "\r", buf, 6);
	int signalLevel, RXQUAL;
	sscanf(buf, "%d,%d", &signalLevel, &RXQUAL);
	return signalLevel;
}
