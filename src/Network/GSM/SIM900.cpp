#include "SIM900.h"
#include "../../LOG/debug.h"

#include <string.h>
#include <stdio.h>
//#define NULL 0

#define _GSM_CONNECTION_TOUT_ 5
#define _TCP_CONNECTION_TOUT_ 20
#define _GSM_DATA_TOUT_ 10

#include "avr/io.h"
#define PWR_PORT PORTH
#define PWR_DDR DDRH
#define PWR_PIN 5

#define NUM_OF_ACK_ATTEMPT 2


SIMCOM900::SIMCOM900(volatile unsigned char & port) : GSM(port)
{

};

SIMCOM900::~SIMCOM900(){};

bool SIMCOM900::forceON()
{
	DEBUG(F("Modem forceON"));
	if(isResponse())
	{
		setGSMStatus(GSM::GSM_READY);
		return 1;
	}

	for(char i = 0; i < NUM_OF_ACK_ATTEMPT; i++)
	{
		turnPWR();

		if(WaitResp(3000, 100))
			if(IsStringReceived("+CPIN: SIM PIN"))
				pinProcedure();

		delay(3000);

		if(isResponse())
		{
			INFO(F("Modem enabled"));
			setGSMStatus(GSM::GSM_READY);
			return 1;
		}

	}
	setGSMStatus(GSM::GSM_ERROR);
	CRITICAL(F("Modem was not enabled"));
	WARNING(F("Maybe it not connected or it no have supply?"));

	return 0;

}

bool SIMCOM900::forceOFF()
{
	DEBUG(F("Modem forceOFF"));

	turnPWR();

	WaitResp(5000, 5000);
	_delay_ms(500);
	if(IsStringReceived("NORMAL POWER DOWN"))
	{
		INFO(F("Modem: NORMAL POWER DOWN"));
		setGSMStatus(GSM::GSM_IDLE);
		return 1;
	}
	else
	{
		if(isResponse())
		{
			WARNING(F("Modem not answered when forceOFF and still answered OK, maybe PWRKEY not connected?"));
			setGSMStatus(GSM::GSM_ERROR);
			return 0;
		}
		else
		{
			WARNING(F("Modem not answered when forceOFF and still not answer OK"));
			setGSMStatus(GSM::GSM_IDLE);
			return 2;
		}
	}
}

void SIMCOM900::turnPWR()
{
	DEBUG(F("Turn PWR Key while 1 second"));
	PWR_DDR |= (1<<PWR_PIN);
	_delay_us(10);
	PWR_PORT |= (1<<PWR_PIN);
	_delay_ms(1200);
	PWR_PORT &= ~(1<<PWR_PIN);
	_delay_ms(10);
	PWR_DDR &= ~(1<<PWR_PIN);
}

int SIMCOM900::readCellData(int &mcc, int &mnc, long &lac, long &cellid)
{
	if(getCOMStatus() != GSM::CLS_FREE)
		return 0;

	flush();
	SimpleWrite("AT+QENG=1,0");
	print("\r");
	SimpleWrite("AT+QENG?");
	print("\r");
	if(WaitResp(5000, 50, "+QENG")!=RX_FINISHED_STR_NOT_RECV)
		return 0;

	mcc = getch();
	mcc = getch();
	mnc = getch();
	lac = getch();
	cellid = getch();

	WaitResp(5000, 50, "+OK");
	SimpleWrite("AT+QENG=1,0");
	print("\r");
	WaitResp(5000, 50, "+OK");
	return 1;
}

//bool SIMCOM900::readSMS(char* msg, int msglength, char* number, int nlength)
//{
//	Serial.println("This method is deprecated! Please use GetSMS in the SMS class.");
//	long index;
//	char *p_char;
//	char *p_char1;
//
//	/*
//  if (getStatus()==IDLE)
//    return false;
//	 */
//#ifdef UNO
//	_tf.setTimeout(_GSM_DATA_TOUT_);
//#endif
//	//_cell.flush();
//	WaitResp(500, 500);
//	SimpleWriteln("AT+CMGL=\"REC UNREAD\",1");
//
//	WaitResp(5000, 500);
//	if(IsStringReceived("+CMGL"))
//	{
//
//		//index
//		p_char = strchr((char *)(comm_buf),'+CMGL');
//		p_char1 = p_char+3;  //we are on the first char of string
//		p_char = p_char1+1;
//		*p_char = 0;
//		index=atoi(p_char1);
//
//		p_char1 = p_char+1;
//		p_char = strstr((char *)(p_char1), "\",\"");
//		p_char1 = p_char+3;
//		p_char = strstr((char *)(p_char1), "\",\"");
//		if (p_char != NULL) {
//			*p_char = 0;
//		}
//		strcpy(number, (char *)(p_char1));
//		//////
//
//		p_char1 = p_char+3;
//		p_char = strstr((char *)(p_char1), "\",\"");
//		p_char1 = p_char+3;
//
//		p_char = strstr((char *)(p_char1), "\n");
//		p_char1 = p_char+1;
//		p_char = strstr((char *)(p_char1), "\n");
//		if (p_char != NULL) {
//			*p_char = 0;
//		}
//		strcpy(msg, (char *)(p_char1));
//
//		// #ifdef UNO
//		// index=_tf.getValue();
//		// #endif
//		// #ifdef MEGA
//		//index=_cell.read();
//		// #endif
//		// Serial.println("DEBUG");
//		// #ifdef UNO
//		// _tf.getString("\",\"", "\"", number, nlength);
//		// #endif
//		// Serial.println("PRIMA");
//		// #ifdef MEGA
//		// _cell.getString("\",\"", "\"", number, nlength);
//		// #endif
//		// Serial.println("DEBUG");
//		// #ifdef UNO
//		// _tf.getString("\n", "\nOK", msg, msglength);
//		// #endif
//		// #ifdef MEGA
//		// _cell.getString("\n", "\nOK", msg, msglength);
//		// #endif
//
//		SimpleWrite("AT+CMGD=");
//		SimpleWriteln(index);
//		// Serial.print("VAL= ");
//		// Serial.println(index);
//		WaitResp(5000, 50, "OK");
//		return true;
//	};
//	return false;
//};

bool SIMCOM900::readCall(char* number, int nlength)
{
	//int index;

	if (getGSMStatus()<=GSM::GSM_IDLE)
		return false;

	//_tf.setTimeout(_GSM_DATA_TOUT_);
	if(WaitResp(5000, 50, "+CLIP: \"")!=RX_FINISHED_STR_RECV)
		//if(_tf.find("+CLIP: \""))
	{
		getString("", "\"", number, nlength);

		SimpleWrite("ATH");
		print("\r");
		delay(1000);
		//_cell.flush();
		return true;
	};
	return false;
};

bool SIMCOM900::call(char* number, unsigned int milliseconds)
{ 
	if (getGSMStatus()<=GSM::GSM_IDLE)
		return false;

	//_tf.setTimeout(_GSM_DATA_TOUT_);

	SimpleWrite("ATD");
	print(number);
	SimpleWrite(";");
	print("\r");
	for(int i = milliseconds / 100; i > 0; i--)
		delay(100);
	SimpleWrite("ATH");
	print("\r");

	return true;

}

int SIMCOM900::setPIN(char *pin)
{
	if((getGSMStatus() != GSM::GSM_READY))
		return 2;

	//_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

	//_cell.flush();

	//AT command to set PIN.
	SimpleWrite(F("AT+CPIN="));
	SimpleWrite(pin);
	print("\r");

	//Expect "OK".

	if(WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV)
		return 0;
	else
		return 1;
}

int SIMCOM900::changeNSIPmode(char mode)
{
	//_tf.setTimeout(_TCP_CONNECTION_TOUT_);

	//if (getStatus()!=ATTACHED)
	//    return 0;

	//_cell.flush();

	SimpleWrite("AT+QIDNSIP=");
	SimpleWrite(mode);
	print("\r");
	if(WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV) return 0;
	//if(!_tf.find("OK")) return 0;

	return 1;
}

int SIMCOM900::getCCI(char *cci)
{
	//Status must be READY
	if((getGSMStatus() != GSM::GSM_READY))
		return 2;

	//_tf.setTimeout(_GSM_DATA_TOUT_);	//Timeout for expecting modem responses.

	//_cell.flush();

	//AT command to get CCID.
	SimpleWrite("AT+QCCID");
	print("\r");
	//Read response from modem
	getString("AT+QCCID\r\r\r\n","\r\n",cci, 21);

	//Expect "OK".
	if(WaitResp(5000, 50, "OK")!=RX_FINISHED_STR_NOT_RECV)
		return 0;
	else
		return 1;
}

const char * SIMCOM900::getIMEI()
{
	static char imei[16];

	//AT command to get IMEI.
	SimpleWrite("AT+GSN\r\n");

	WaitResp(5000, 500);
	//Read response from modem
	if(getString("\r\n\r\n","\r\n",imei, 15))
		if(IsStringReceived("OK"))
			return imei;
	return 0;
}

//---------------------------------------------
/**********************************************************
Turns on/off the speaker

off_on: 0 - off
        1 - on
 **********************************************************/
void SIMCOM900::SetSpeaker(char off_on)
{
	if (CLS_FREE != getCOMStatus()) return;
	setCOMStatus(CLS_ATCMD);
	if (off_on) {
		SendATCmdWaitResp("AT#GPIO=5,1,2", 500, 50, "#GPIO:", 1);
	}
	else {
		SendATCmdWaitResp("AT#GPIO=5,0,2", 500, 50, "#GPIO:", 1);
	}
	setCOMStatus(CLS_FREE);
}




/**********************************************************
Method sets speaker volume

speaker_volume: volume in range 0..14

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module did not answer in timeout
        -3 - GSM module has answered "ERROR" string

        OK ret val:
        -----------
        0..14 current speaker volume 
 **********************************************************/
/*
char GSM::SetSpeakerVolume(byte speaker_volume)
{

  char ret_val = -1;

  if (CLS_FREE != GetCommLineStatus()) return (ret_val);
  SetCommLineStatus(CLS_ATCMD);
  // remember set value as last value
  if (speaker_volume > 14) speaker_volume = 14;
  // select speaker volume (0 to 14)
  // AT+CLVL=X<CR>   X<0..14>
  _cell.print("AT+CLVL=");
  _cell.print((int)speaker_volume);    
  _cell.print("\r"); // send <CR>
  // 10 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  if (RX_TMOUT_ERR == WaitResp(10000, 50)) {
    ret_val = -2; // ERROR
  }
  else {
    if(IsStringReceived("OK")) {
      last_speaker_volume = speaker_volume;
      ret_val = last_speaker_volume; // OK
    }
    else ret_val = -3; // ERROR
  }

  SetCommLineStatus(CLS_FREE);
  return (ret_val);
}
 */
/**********************************************************
Method increases speaker volume

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module did not answer in timeout
        -3 - GSM module has answered "ERROR" string

        OK ret val:
        -----------
        0..14 current speaker volume 
 **********************************************************/
/*
char GSM::IncSpeakerVolume(void)
{
  char ret_val;
  byte current_speaker_value;

  current_speaker_value = last_speaker_volume;
  if (current_speaker_value < 14) {
    current_speaker_value++;
    ret_val = SetSpeakerVolume(current_speaker_value);
  }
  else ret_val = 14;

  return (ret_val);
}
 */
/**********************************************************
Method decreases speaker volume

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module did not answer in timeout
        -3 - GSM module has answered "ERROR" string

        OK ret val:
        -----------
        0..14 current speaker volume 
 **********************************************************/
/*
char GSM::DecSpeakerVolume(void)
{
  char ret_val;
  byte current_speaker_value;

  current_speaker_value = last_speaker_volume;
  if (current_speaker_value > 0) {
    current_speaker_value--;
    ret_val = SetSpeakerVolume(current_speaker_value);
  }
  else ret_val = 0;

  return (ret_val);
}
 */
/**********************************************************
Method sends DTMF signal
This function only works when call is in progress

dtmf_tone: tone to send 0..15

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module didn't answer in timeout
        -3 - GSM module has answered "ERROR" string

        OK ret val:
        -----------
        0.. tone
 **********************************************************/
/*
char GSM::SendDTMFSignal(byte dtmf_tone)
{
  char ret_val = -1;

  if (CLS_FREE != GetCommLineStatus()) return (ret_val);
  SetCommLineStatus(CLS_ATCMD);
  // e.g. AT+VTS=5<CR>
  _cell.print("AT+VTS=");
  _cell.print((int)dtmf_tone);    
  _cell.print("\r");
  // 1 sec. for initial comm tmout
  // 50 msec. for inter character timeout
  if (RX_TMOUT_ERR == WaitResp(1000, 50)) {
    ret_val = -2; // ERROR
  }
  else {
    if(IsStringReceived("OK")) {
      ret_val = dtmf_tone; // OK
    }
    else ret_val = -3; // ERROR
  }

  SetCommLineStatus(CLS_FREE);
  return (ret_val);
}
 */

/**********************************************************
Method returns state of user button
return: 0 - not pushed = released
        1 - pushed
 **********************************************************/
char SIMCOM900::IsUserButtonPushed(void)
{
	char ret_val = 0;
	if (CLS_FREE != getCOMStatus()) return(0);
	setCOMStatus(CLS_ATCMD);
	if (AT_RESP_OK == SendATCmdWaitResp("AT#GPIO=9,2", 500, 50, "#GPIO: 0,0", 1)) {
		//user button is pushed
		ret_val = 1;
	}
	else ret_val = 0;
	setCOMStatus(CLS_FREE);
	return (ret_val);
}



/**********************************************************
Method reads phone number string from specified SIM position

position:     SMS position <1..20>

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module didn't answer in timeout
        -3 - position must be > 0
        phone_number is empty string

        OK ret val:
        -----------
        0 - there is no phone number on the position
        1 - phone number was found
        phone_number is filled by the phone number string finished by 0x00
                     so it is necessary to define string with at least
                     15 bytes(including also 0x00 termination character)

an example of usage:
        GSM gsm;
        char phone_num[20]; // array for the phone number string

        if (1 == GetPhoneNumber(1, phone_num)) {
          // valid phone number on SIM pos. #1 
          // phone number string is copied to the phone_num array
          #ifdef DEBUG_PRINT
            DebugPrint("DEBUG phone number: ", 0);
            DebugPrint(phone_num, 1);
          #endif
        }
        else {
          // there is not valid phone number on the SIM pos.#1
          #ifdef DEBUG_PRINT
            DebugPrint("DEBUG there is no phone number", 1);
          #endif
        }
 **********************************************************/


char SIMCOM900::GetPhoneNumber(char position, char *phone_number)
{
	char ret_val = -1;

	char *p_char;
	char *p_char1;

	if (position == 0) return (-3);
	if (CLS_FREE != getCOMStatus()) return (ret_val);
	setCOMStatus(CLS_ATCMD);
	ret_val = 0; // not found yet
	phone_number[0] = 0; // phone number not found yet => empty string

	//send "AT+CPBR=XY" - where XY = position
	print(F("AT+CPBR="));
	print((int)position);
	print("\r");

	// 5000 msec. for initial comm tmout
	// 50 msec. for inter character timeout
	switch (WaitResp(5000, 50, "+CPBR")) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		ret_val = -2;
		break;

	case RX_FINISHED_STR_RECV:
		// response in case valid phone number stored:
		// <CR><LF>+CPBR: <index>,<number>,<type>,<text><CR><LF>
		// <CR><LF>OK<CR><LF>

		// response in case there is not phone number:
		// <CR><LF>OK<CR><LF>
		p_char = strstr((char *)(comm_buf),",\"");
		if (p_char != NULL) {
			p_char++;
			p_char++;       // we are on the first phone number character
			// find out '"' as finish character of phone number string
			p_char1 = strchr((char *)(p_char),'"');
			if (p_char1 != NULL) {
				*p_char1 = 0; // end of string
			}
			// extract phone number string
			strcpy(phone_number, (char *)(p_char));
			// output value = we have found out phone number string
			ret_val = 1;
		}
		break;

	case RX_FINISHED_STR_NOT_RECV:
		// only OK or ERROR => no phone number
		ret_val = 0;
		break;
	}

	setCOMStatus(CLS_FREE);
	return (ret_val);
}

/**********************************************************
Method writes phone number string to the specified SIM position

position:     SMS position <1..20>
phone_number: phone number string for the writing

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module didn't answer in timeout
        -3 - position must be > 0

        OK ret val:
        -----------
        0 - phone number was not written
        1 - phone number was written
 **********************************************************/
char SIMCOM900::WritePhoneNumber(char position, char *phone_number)
{
	char ret_val = -1;

	if (position == 0) return (-3);
	if (CLS_FREE != getCOMStatus()) return (ret_val);
	setCOMStatus(CLS_ATCMD);
	ret_val = 0; // phone number was not written yet

	//send: AT+CPBW=XY,"00420123456789"
	// where XY = position,
	//       "00420123456789" = phone number string
	print(F("AT+CPBW="));
	print((int)position);
	print(",\"");
	print(phone_number);
	print("\"\r");

	// 5000 msec. for initial comm tmout
	// 50 msec. for inter character timeout
	switch (WaitResp(5000, 50, "OK")) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		break;

	case RX_FINISHED_STR_RECV:
		// response is OK = has been written
		ret_val = 1;
		break;

	case RX_FINISHED_STR_NOT_RECV:
		// other response: e.g. ERROR
		break;
	}

	setCOMStatus(CLS_FREE);
	return (ret_val);
}


/**********************************************************
Method del phone number from the specified SIM position

position:     SMS position <1..20>

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module didn't answer in timeout
        -3 - position must be > 0

        OK ret val:
        -----------
        0 - phone number was not deleted
        1 - phone number was deleted
 **********************************************************/
char SIMCOM900::DelPhoneNumber(char position)
{
	char ret_val = -1;

	if (position == 0) return (-3);
	if (CLS_FREE != getCOMStatus()) return (ret_val);
	setCOMStatus(CLS_ATCMD);
	ret_val = 0; // phone number was not written yet

	//send: AT+CPBW=XY
	// where XY = position
	print("AT+CPBW=");
	print((int)position);
	print("\r");

	// 5000 msec. for initial comm tmout
	// 50 msec. for inter character timeout
	switch (WaitResp(5000, 50, "OK")) {
	case RX_TMOUT_ERR:
		// response was not received in specific time
		break;

	case RX_FINISHED_STR_RECV:
		// response is OK = has been written
		ret_val = 1;
		break;

	case RX_FINISHED_STR_NOT_RECV:
		// other response: e.g. ERROR
		break;
	}

	setCOMStatus(CLS_FREE);
	return (ret_val);
}





/**********************************************************
Function compares specified phone number string 
with phone number stored at the specified SIM position

position:       SMS position <1..20>
phone_number:   phone number string which should be compare

return: 
        ERROR ret. val:
        ---------------
        -1 - comm. line to the GSM module is not free
        -2 - GSM module didn't answer in timeout
        -3 - position must be > 0

        OK ret val:
        -----------
        0 - phone numbers are different
        1 - phone numbers are the same


an example of usage:
        if (1 == ComparePhoneNumber(1, "123456789")) {
          // the phone num. "123456789" is stored on the SIM pos. #1
          // phone number string is copied to the phone_num array
          #ifdef DEBUG_PRINT
            DebugPrint("DEBUG phone numbers are the same", 1);
          #endif
        }
        else {
          #ifdef DEBUG_PRINT
            DebugPrint("DEBUG phone numbers are different", 1);
          #endif
        }
 **********************************************************/
char SIMCOM900::ComparePhoneNumber(char position, char *phone_number)
{
	char ret_val = -1;
	char sim_phone_number[20];


	ret_val = 0; // numbers are not the same so far
	if (position == 0) return (-3);
	if (1 == GetPhoneNumber(position, sim_phone_number)) {
		//		Serial.print("CHIAMANTE ");
		//		Serial.println(phone_number);
		//		Serial.print("SALVATO ");
		//		Serial.println(sim_phone_number);

		// there is a valid number at the spec. SIM position
		// -------------------------------------------------
		if (0 == strcmp(phone_number, sim_phone_number)) {
			// phone numbers are the same
			// --------------------------
			ret_val = 1;
		}
	}
	return (ret_val);
}

void SIMCOM900::pinProcedure()
{
	CRITICAL(F("SIM say need pin"));

	//TODO sim pin procedure
}


void SIMCOM900::byteRecived(char rxByte)
{
	//TODO gsm port rx byte handler
	/*
	 * запрос симки 			+CPIN: SIM PIN
	 * отключение гпрс 			+PDP: DEACT
	 * отключение модема 		NORMAL POWER DOWN
	 * отконекчинье от сервака 	CLOSED
	 */

}

const unsigned long int  SIMCOM900::getUNIXdate()
{
#define SEC_IN_MIN	60ul
#define MIN_IN_HH 	60ul
#define HH_IN_DD	24ul
#define DD_IN_YY	365ul
#define SEC_IN_HH	SEC_IN_MIN*MIN_IN_HH
#define SEC_IN_DD	SEC_IN_HH*HH_IN_DD
#define SEC_IN_YY 	SEC_IN_DD*DD_IN_YY

#define YEAR0 1970

	unsigned long int UNIXdate = 0;
	print(F("AT+CCLK?\r"));
	WaitResp(5000,500);
	char buf[21];
	getString("\"", "\"", buf, 20);
	INFO(buf);
	int yy, mm, dd, hh, min, sec, zz;
	sscanf(buf, "%d/%d/%d,%d:%d:%d+%d", &yy, &mm, &dd, &hh, &min, &sec, &zz);
	yy += 2000;
/*
#ifdef LEVEL_DEBUG
	debug(F("Year: ")); debug(yy); debug(F("\r\n"));
	debug(F("Mounts: ")); debug(mm); debug(F("\r\n"));
	debug(F("Day: ")); debug(dd); debug(F("\r\n"));
	debug(F("Hour: ")); debug(hh); debug(F("\r\n"));
	debug(F("Minute: ")); debug(min); debug(F("\r\n"));
	debug(F("Second: ")); debug(sec); debug(F("\r\n"));
	debug(F("Zone: ")); debug(zz); debug(F("\r\n"));
#endif*/

	// if the current year is a leap one -> add one day (86400 sec)
	if ((!(yy%4)) && (mm>2))
		UNIXdate+=SEC_IN_DD;

	// Next, add to s variable: (the number of days from each year (even leap years)) * 86400 sec,
	// the number of days from the current month
	// the each hour & minute & second from the current day
	UNIXdate += (yy-YEAR0)*SEC_IN_YY
			+((unsigned long)((yy-YEAR0)/4))*SEC_IN_DD
			+(dd-1)*SEC_IN_DD
			+hh*SEC_IN_HH
			+min*SEC_IN_MIN
			+sec;

	const char calendar[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	mm-- ; // dec the current month (find how many months have passed from the current year)
	while (mm > 0) // sum the days from January to the current month
		UNIXdate += calendar[--mm]*SEC_IN_DD; // add the number of days from a month * 86400 sec

	DEBUG(UNIXdate);
	return UNIXdate;
}



//-----------------------------------------------------
