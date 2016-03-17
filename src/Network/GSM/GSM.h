#ifndef GSM_H
#define GSM_H


#include "../../UART/UART.h"
#include "util/delay.h"



#define delay(x) _delay_ms(x)


#define ctrlz 26 //Ascii character for ctr+z. End of a SMS.
#define cr    13 //Ascii character for carriage return. 
#define lf    10 //Ascii character for line feed.
#define GSM_LIB_VERSION 400 // library version X.YY (e.g. 1.00)

// length for the internal communication buffer
#define COMM_BUF_LEN        200



// some constants for the InitParam() method
#define PARAM_SET_0   0
#define PARAM_SET_1   1

// status bits definition
#define STATUS_NONE                 0
#define STATUS_INITIALIZED          1
#define STATUS_REGISTERED           2
#define STATUS_USER_BUTTON_ENABLE   4

// GPRS status
#define CHECK_AND_OPEN    0
#define CLOSE_AND_REOPEN  1

#define DEC 10



#define SimpleWrite(str) print(str)



class GSM : public UART
{
public:
	GSM(volatile unsigned char & port);
	virtual ~GSM(){}

	//int begin(long baud_rate);
	bool isResponse();
	// returns whether complete initialization was made
	char isInitialized(void);

	void InitParam (char group);

	int getSignalLevel();

	//-----------------------
	void Echo(char state);



	// User button methods
	//	inline char IsUserButtonEnable(void) {return (module_status & STATUS_USER_BUTTON_ENABLE);};
	//	inline void DisableUserButton(void) {module_status &= ~STATUS_USER_BUTTON_ENABLE;};
	//	inline void EnableUserButton(void) {module_status |= STATUS_USER_BUTTON_ENABLE;};
	//	char IsUserButtonPushed(void);

	// Phonebook's methods
	char GetPhoneNumber(char position, char *phone_number);
	char WritePhoneNumber(char position, char *phone_number);
	char DelPhoneNumber(char position);
	char ComparePhoneNumber(char position, char *phone_number);


	//------------------------
	// RX state machine
	enum RXstateRes
	{
		RX_NOT_FINISHED,      // not finished yet
		RX_FINISHED,              // finished, some character was received
		RX_FINISHED_STR_RECV,     // finished and expected string received
		RX_FINISHED_STR_NOT_RECV, // finished, but expected string not received
		RX_TMOUT_ERR,             // finished, no character received

	};
	enum ATresp
	{
		AT_RESP_ERR_NO_RESP = -1,   // nothing received
		AT_RESP_ERR_DIF_RESP = 0,   // response_string is different from the response
		AT_RESP_OK = 1,             // response_string was included in the response

		AT_RESP_LAST_ITEM
	};
	void RxInit(unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout);
	RXstateRes IsRxFinished(void);
	bool IsStringReceived(char const *compare_string);
	RXstateRes WaitResp(unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout);
	RXstateRes WaitResp(unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout,
			char const *expected_resp_string);
	ATresp SendATCmdWaitResp(char const *AT_cmd_string,
			unsigned long int start_comm_tmout, unsigned long int max_interchar_tmout,
			char const *response_string,char no_of_attempts);
	//-----------------------

	enum GSMStatus
	{
		//GSM status
		GSM_ERROR,
		GSM_IDLE,
		GSM_READY,
		GSM_INITIALIZED,
		GSM_TCPSERVERWAIT,
		GSM_TCPCONNECTEDSERVER,
		GSM_TCPCONNECTEDCLIENT
	};
	enum COMStatus
	{
		// CLS like CommunicationLineStatus
		CLS_FREE,   // line is free - not used by the communication and can be used
		CLS_ATCMD,  // line is used by AT commands, includes also time for response
		CLS_DATA,   // line is used in the CSD or GPRS communication
	};
	enum RegistrationStatus
	{
		REG_NOT_REGISTERED,
		REG_REGISTERED,
		REG_NO_RESPONSE,
		REG_COMM_LINE_BUSY,
	};
	enum GPRSStatus
	{
		GPRS_ATTACHED,
		GPRS_DETACHED
	};

	void setGSMStatus(GSMStatus status);
	void setCOMStatus(COMStatus  new_status);
	void setRegistrationStatus(RegistrationStatus  new_status);
	void setGPRSStatus(GPRSStatus status);

	volatile GSMStatus getGSMStatus(){ return _gsmSstatus; };
	volatile COMStatus  getCOMStatus(void);
	volatile RegistrationStatus  getRegistrationStatus(void){ return _registrationStatus; };
	volatile GPRSStatus getGPRSStatus(){ return _gprsStatus; };


	// checks if module is registered in the GSM network
	// must be called regularly
	RegistrationStatus CheckRegistration(void);
	// returns registration state with wait
	RegistrationStatus isRegistered(void);

	int isIP(const char* cadena);
	char * readIPfromBuff();
	int getString(const char *pre_string, const char *post_string, char *buffer, int length);

	char comm_buf[COMM_BUF_LEN+1];  // communication buffer +1 for 0x00 termination


private:

	// global status - bits are used for representation of states
	GSMStatus _gsmSstatus;
	COMStatus _clsStatus;
	RegistrationStatus _registrationStatus;
	GPRSStatus _gprsStatus;

	//ip-addr buff
	char ipaddr[18];

	// variables connected with communication buffer
	enum RXstate
	{
		RX_NOT_STARTED,
		RX_ALREADY_STARTED
	};
	char *p_comm_buf;               // pointer to the communication buffer
	unsigned long int comm_buf_len;              // num. of characters in the buffer
	RXstate rx_state;                  // internal state of rx state machine
	unsigned long int start_reception_tmout; // max tmout for starting reception
	unsigned long int interchar_tmout;       // previous time in msec.
	unsigned long long int prev_time;        // previous time in msec.

	// last value of speaker volume
	char last_speaker_volume;
};


/**************************************************
	if (CLS_FREE != getCOMStatus()) return false;
	setCOMStatus(CLS_ATCMD);

	setCOMStatus(CLS_FREE);
**************************************************/

#endif
