#ifndef _SMS_H_
#define _SMS_H_

#include "GSM.h"

class SMSGSM
{
public:
	SMSGSM(GSM& _gsm);

	// SMS type
	// use by method IsSMSPresent()
	enum sms_type_enum
	{
		SMS_UNREAD, SMS_READ, SMS_ALL,

		SMS_LAST_ITEM
	};

	enum getsms_ret_val_enum
	{
		GETSMS_NO_RESPONSE = -2, GETSMS_ERROR = -1,

		GETSMS_NO_SMS, GETSMS_UNREAD_SMS, GETSMS_READ_SMS, GETSMS_OTHER_SMS,

		GETSMS_NOT_AUTH_SMS, GETSMS_AUTH_SMS,

		GETSMS_LAST_ITEM
	};

	// SMS's methods
	char SendSMS(const char *number_str, const char *message_str);
	char SendSMS(const char sim_phonebook_position, const char *message_str);
	char IsSMSPresent(sms_type_enum required_status);
	getsms_ret_val_enum GetSMS(char position, char *phone_number,
			char *SMS_text, unsigned char max_SMS_len);

	getsms_ret_val_enum GetAuthorizedSMS(char position, char *phone_number,
			char *SMS_text, char max_SMS_len, char first_authorized_pos,
			char last_authorized_pos);
	char DeleteSMS(char position);

	char InitSMSMemory(void);

private:
	GSM& gsm;
};

#endif

