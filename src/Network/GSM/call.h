#ifndef _CALL_H_
#define _CALL_H_

#include "GSM.h"

class CallGSM
{
public:
	CallGSM(GSM& _gsm);

	// finds out the status of call
	char CallStatus(void);
	char CallStatusWithAuth(char *phone_number,
			char first_authorized_pos, char last_authorized_pos);
	// picks up an incoming call
	void PickUp(void);
	// hangs up an incomming call
	void HangUp(void);
	// calls the specific number
	void Call(char *number_string);
	// makes a call to the number stored at the specified SIM position
	void Call(int sim_position);
	void SendDTMF(char *number_string, int time);

	void SetDTMF(int DTMF_status);
	char DetDTMF();
	
	enum call_ret_val_enum
	{
		CALL_NONE = 0,
		CALL_INCOM_VOICE,
		CALL_ACTIVE_VOICE,
		CALL_INCOM_VOICE_AUTH,
		CALL_INCOM_VOICE_NOT_AUTH,
		CALL_INCOM_DATA_AUTH,
		CALL_INCOM_DATA_NOT_AUTH,
		CALL_ACTIVE_DATA,
		CALL_OTHERS,
		CALL_NO_RESPONSE,
		CALL_COMM_LINE_BUSY,

		CALL_LAST_ITEM
	};

private:
	GSM& gsm;

};

#endif

