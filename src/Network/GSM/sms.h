#ifndef _SMS_H_
#define _SMS_H_

#include "GSM.h"

class SMSGSM
{
public:
	SMSGSM(GSM& _gsm);
	// SMS's methods
	char SendSMS(char *number_str, char *message_str);
	char SendSMS(char sim_phonebook_position, char *message_str);
	char IsSMSPresent(char required_status);
	char GetSMS(char position, char *phone_number, char *SMS_text, unsigned char max_SMS_len);

	char GetAuthorizedSMS(char position, char *phone_number, char *SMS_text, char max_SMS_len,
			char first_authorized_pos, char last_authorized_pos);
	char DeleteSMS(char position);

	char InitSMSMemory(void);

	// SMS type
	// use by method IsSMSPresent()
	enum sms_type_enum
	{
		SMS_UNREAD,
		SMS_READ,
		SMS_ALL,

		SMS_LAST_ITEM
	};

	enum getsms_ret_val_enum
	{
		GETSMS_NO_SMS   = 0,
		GETSMS_UNREAD_SMS,
		GETSMS_READ_SMS,
		GETSMS_OTHER_SMS,

		GETSMS_NOT_AUTH_SMS,
		GETSMS_AUTH_SMS,

		GETSMS_LAST_ITEM
	};

private:
	GSM& gsm;
};

#endif

