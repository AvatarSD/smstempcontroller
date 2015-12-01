#include "inetGSM.h"

#include "../../LOG/debug.h"

#include <string.h>
#include <stdlib.h>

//#define _GSM_CONNECTION_TOUT_ 5
//#define _TCP_CONNECTION_TOUT_ 20
//#define _GSM_DATA_TOUT_ 10


InetGSM::InetGSM(GSM& _gsm) : gsm(_gsm)
{

}

int InetGSM::httpGET(const char* server, int port, const char* path, char* result, int resultlength)
{
	bool connected=false;
	int n_of_at=0;
	//int length_write;
	char end_c[2];
	end_c[0]=0x1a;
	end_c[1]='\0';

	/*
  Status = ATTACHED.
  if(gsm.getStatus()!=GSM::ATTACHED)
    return 0;
	 */
	while(n_of_at<3){
		if(!connectTCP(server, port)){
#ifdef DEBUG_ON
			Serial.println("DB:NOT CONN");
#endif
			n_of_at++;
		}
		else{
			connected=true;
			n_of_at=3;
		}
	}

	if(!connected) return 0;

	gsm.SimpleWrite("GET ");
	gsm.SimpleWrite(path);
	gsm.SimpleWrite(" HTTP/1.0\nHost: ");
	gsm.SimpleWrite(server);
	gsm.SimpleWrite("\n");
	gsm.SimpleWrite("User-Agent: Arduino");
	gsm.SimpleWrite("\n\n");
	gsm.print(end_c);

	switch(gsm.WaitResp(10000, 10, "SEND OK")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0; 
		break;
	}


	delay(50);
#ifdef DEBUG_ON
	Serial.println("DB:SENT");
#endif
	int res= gsm.read((unsigned char*)result, resultlength);

	//gsm.disconnectTCP();

	//int res=1;
	return res;
}

int InetGSM::httpPOST(const char* server, int port, const char* path, const char* parameters, char* result, int resultlength)
{
	bool connected=false;
	int n_of_at=0;
	char itoaBuffer[8];
	//int num_char;
	char end_c[2];
	end_c[0]=0x1a;
	end_c[1]='\0';

	while(n_of_at<3){
		if(!connectTCP(server, port)){
#ifdef DEBUG_ON
			Serial.println("DB:NOT CONN");
#endif
			n_of_at++;
		}
		else{
			connected=true;
			n_of_at=3;
		}
	}

	if(!connected) return 0;

	gsm.SimpleWrite("POST ");
	gsm.SimpleWrite(path);
	gsm.SimpleWrite(" HTTP/1.1\nHost: ");
	gsm.SimpleWrite(server);
	gsm.SimpleWrite("\n");
	gsm.SimpleWrite("User-Agent: AVR\n");
	gsm.SimpleWrite("Content-Type: application/x-www-form-urlencoded\n");
	gsm.SimpleWrite("Content-Length: ");
	itoa(strlen(parameters),itoaBuffer,10);
	gsm.print(itoaBuffer);
	gsm.SimpleWrite("\n\n");
	gsm.SimpleWrite(parameters);
	gsm.SimpleWrite("\n\n");
	gsm.print(end_c);

	switch(gsm.WaitResp(10000, 10, "SEND OK")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0; 
		break;
	}

	delay(50);
#ifdef DEBUG_ON
	Serial.println("DB:SENT");
#endif

	int res= gsm.read((unsigned char*)result, resultlength);
	//gsm.disconnectTCP();
	return res;
}

int InetGSM::openmail(char* server, char* loginbase64, char* passbase64, char* from, char* to, char* subj)
{
	bool connected=false;
	int n_of_at=0;
	char end_c[2];
	end_c[0]=0x1a;
	end_c[1]='\0';

	while(n_of_at<3){
		if(!connectTCP(server, 25)){

			DEBUG("DB:NOT CONN");

			n_of_at++;
		}
		else{
			connected=true;
			n_of_at=3;
		}
	}

	if(!connected) return 0;

	delay(100);   
	gsm.SimpleWrite("HELO ");  gsm.print(server);  gsm.SimpleWrite("\n");
	gsm.print(end_c);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "OK");
	if(!gsm.IsStringReceived("SEND OK"))
		return 0;
	delay(500);
	gsm.WaitResp(5000, 100);

	delay(100);   
	gsm.SimpleWrite("AT+CIPSEND");
	gsm.print("\r");
	switch(gsm.WaitResp(5000, 200, ">")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0;
		break;
	}
	gsm.SimpleWrite("AUTH LOGIN\n");
	gsm.print(end_c);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "OK");
	if(!gsm.IsStringReceived("OK"))
		return 0;
	delay(500);
	gsm.WaitResp(5000, 100);

	delay(100);   
	gsm.SimpleWrite("AT+CIPSEND");
	gsm.print("\r");
	switch(gsm.WaitResp(5000, 200, ">")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0;
		break;
	}
	gsm.print(loginbase64);gsm.SimpleWrite("\n");
	gsm.print(end_c);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "OK");
	if(!gsm.IsStringReceived("OK"))
		return 0;
	delay(500);
	gsm.WaitResp(5000, 100);

	delay(100);   
	gsm.SimpleWrite("AT+CIPSEND");
	gsm.print("\r");
	switch(gsm.WaitResp(5000, 200, ">")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0;
		break;
	}	
	gsm.print(passbase64);
	gsm.SimpleWrite("\n");
	gsm.print(end_c);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "OK");
	if(!gsm.IsStringReceived("OK"))
		return 0;
	delay(500);
	gsm.print("\r");
	gsm.WaitResp(5000, 100);


	delay(100);   
	gsm.SimpleWrite("AT+CIPSEND");
	gsm.print("\r");
	switch(gsm.WaitResp(5000, 200, ">")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0;
		break;
	}
	gsm.SimpleWrite("MAIL From: <");
	gsm.print(from);
	gsm.SimpleWrite(">\n");
	gsm.print(end_c);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "OK");
	if(!gsm.IsStringReceived("OK"))
		return 0;
	delay(500);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "");

	delay(100);   
	gsm.SimpleWrite("AT+CIPSEND");
	gsm.print("\r");
	switch(gsm.WaitResp(5000, 200, ">")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0;
		break;
	}
	gsm.SimpleWrite("RCPT TO: <");gsm.print(to);gsm.SimpleWrite(">\n");
	gsm.print(end_c);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "OK");
	if(!gsm.IsStringReceived("OK"))
		return 0;
	delay(500);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "");

	delay(100);
	gsm.SimpleWrite("AT+CIPSEND");
	gsm.print("\r");
	switch(gsm.WaitResp(5000, 200, ">")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0;
		break;
	}
	gsm.SimpleWrite("Data\n");
	gsm.print(end_c);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "OK");
	if(!gsm.IsStringReceived("OK"))
		return 0;
	delay(500);
	gsm.print("\r");
	gsm.WaitResp(5000, 100, "");

	delay(100);   
	gsm.SimpleWrite("AT+CIPSEND");
	gsm.print("\r");
	switch(gsm.WaitResp(5000, 200, ">")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0;
		break;
	}
	gsm.SimpleWrite("Subject: ");gsm.print(subj);gsm.SimpleWrite("\n\n");
	gsm.print("\r");
	return 1;
}

int InetGSM::closemail()
{
	char end_c[2];
	end_c[0]=0x1a;
	end_c[1]='\0';

	gsm.SimpleWrite("\n.\n");
	gsm.print(end_c);
	gsm.print("\r");
	disconnectTCP();
	return 1;
}

int InetGSM::attachGPRS(const char* domain, const char* dom1, const char* dom2)
{


	DEBUG(F("enabling GPRS..."));
	DEBUG(domain);

	delay(100);
	DEBUG(F("ask local ip adders"));
	gsm.SimpleWrite(F("AT+CIFSR\r"));
	if(gsm.WaitResp(5000, 50, "ERROR") == GSM::RX_FINISHED_STR_RECV)
		//	{
		//		INFO(gsm.comm_buf);
		//		DEBUG(F("close all the TCP or UDP connection"));
		//		gsm.SimpleWrite("AT+CIPCLOSE");
		//		gsm.print("\r");
		//		gsm.WaitResp(5000, 50, "ERROR");
		//		delay(2000);
		//		DEBUG(F("configures the module as the client"));
		//		gsm.SimpleWrite("AT+CIPSERVER=0");
		//		gsm.print("\r");
		//		gsm.WaitResp(5000, 50, "ERROR");
		//	}
		//	else
	{

		DEBUG(F("now not connect"));
		DEBUG(F("closing the GPRS PDP context"));
		gsm.SimpleWrite(F("AT+CIPSHUT\r"));
		if(gsm.WaitResp(5000, 50, "SHUT OK") != GSM::RX_FINISHED_STR_RECV)
		{
			WARNING(F("GPRS PDP context not been closed, modem error"));
			return 0;
		}


		delay(4000);

		gsm.SimpleWrite(F("AT+CSTT=\""));
		gsm.print(domain);
		gsm.SimpleWrite(F("\",\""));
		gsm.print(dom1);
		gsm.SimpleWrite(F("\",\""));
		gsm.print(dom2);
		gsm.SimpleWrite(F("\"\r"));
		gsm.print(F("\r"));

		if(gsm.WaitResp(500, 50, "OK") != GSM::RX_FINISHED_STR_RECV)
		{
			WARNING(F("configuration set by the AT+CSTT command wasn't set"));
			return 0;
		}

		delay(5000);

		gsm.SimpleWrite(F("AT+CIICR\r"));
		if(gsm.WaitResp(10000, 5000, "OK") != GSM::RX_FINISHED_STR_RECV)
		{
			WARNING(F("GPRS didn't bring up"));
			return 0;
		}

		delay(5000);

		gsm.SimpleWrite(F("AT+CIFSR\r"));
		if(gsm.WaitResp(5000, 50, "ERROR") == GSM::RX_FINISHED_STR_RECV)

		{
			WARNING(F("GPRS NOT enabled\n\r"));
			return 0;
		}
	}
	INFO(F("GPRS enabled"));
	INFO(gsm.readIPfromBuff());
	gsm.setGPRSStatus(GSM::GPRS_ATTACHED);
	delay(4000);
	return 1;
}

int InetGSM::dettachGPRS()
{
	//TODO  repair == to <=
	if (gsm.getGSMStatus() <= GSM::GSM_IDLE) return 0;

	DEBUG(F("disabling GPRS"));

	gsm.SimpleWrite(F("AT+CGATT=0"));
	gsm.print("\r");
	if(gsm.WaitResp(5000, 1500, "OK") == GSM::RX_FINISHED_STR_RECV)
	{
		INFO(F("GPRS disabled"));
		DEBUG(F("OK"));
		gsm.setGPRSStatus(GSM::GPRS_DETACHED);
		gsm.setGSMStatus(GSM::GSM_READY);
		return 1;
	}
	WARNING(F("some error with disabling GPRS"));
	DEBUG(F("ERROR"));
	delay(500);
	gsm.setGSMStatus(GSM::GSM_ERROR);
	return 0;
}


int InetGSM::connectTCP(const char* server, unsigned int port)
{
	//Visit the remote TCP server.
	gsm.SimpleWrite(F("AT+CIPSTART=\"TCP\",\""));
	gsm.SimpleWrite(server);
	gsm.SimpleWrite("\",");
	gsm.SimpleWrite((long int)port);
	gsm.print("\r");

	gsm.WaitResp(5000, 15000);
	//	gsm.WaitResp(15000, 5000);

	/*
	if(gsm.WaitResp(1000, 200, "OK") != GSM::RX_FINISHED_STR_RECV)
		DEBUG(F("Not answer OK"))
	else DEBUG(F("Answer OK"));*/

	if(gsm.IsStringReceived("OK"))
	{
		if (gsm.IsStringReceived("CLOSED"))
		{
			WARNING(F("PORT CLOSED"));

			//			gsm.setGSMStatus(GSM::GSM_READY);
			//			WARNING(F("Modem: Didn't connect to server"))
			//			return -1;
		}
		//gsm.WaitResp(15000, 500);
		if(gsm.IsStringReceived("CONNECT OK"))
		{
			INFO(F("Connected to server"));
			delay(400);
			gsm.setGSMStatus(GSM::GSM_TCPCONNECTEDCLIENT);

			return 1;
		}
		else
		{

			do
			{
				gsm.print("AT+CIPSTATUS\r");
			}while(gsm.WaitResp(8000, 500, "TCP CONNECTING")== GSM::RX_FINISHED_STR_RECV);

			if(gsm.IsStringReceived("CONNECT OK"))
			{
				INFO(F("Connected to server"));
				delay(400);
				gsm.setGSMStatus(GSM::GSM_TCPCONNECTEDCLIENT);

				return 1;
			}

			if (gsm.IsStringReceived("CLOSED"))
				WARNING(F("PORT CLOSED"));
		}
	}

	if(gsm.IsStringReceived("ERROR"))
	{
		//gsm.WaitResp(3000, 500);
		if (gsm.IsStringReceived("ALREADY CONNECT"))
		{
			INFO(F("Modem says: Already connected."));

			delay(10000);
			disconnectTCP();
			delay(1000);
			return connectTCP(server, port);
		}
	}

	gsm.setGSMStatus(GSM::GSM_READY);
	WARNING(F("Modem: Didn't connect to server"))
	return -1;
}







int InetGSM::disconnectTCP()
{
	gsm.SendATCmdWaitResp("AT+CIPSHUT", 5000, 200, "OK", 1);

	//Close TCP client and deact.
	gsm.print(F("AT+CIPCLOSE\r\n"));
	DEBUG(F("Close current connection.."));
	if(gsm.WaitResp(8000, 1000, "CLOSE OK") != GSM::RX_FINISHED_STR_RECV)
		gsm.setGSMStatus(GSM::GSM_ERROR);

	if(gsm.getGSMStatus()==GSM::GSM_TCPCONNECTEDSERVER)
		gsm.setGSMStatus(GSM::GSM_TCPSERVERWAIT);
	else gsm.setGSMStatus(GSM::GSM_READY);
	delay(2500);
	INFO("Connection closed");
	return 1;
}






int InetGSM::connectTCPServer(int port)
{
	/*
  if (getStatus()!=ATTACHED)
     return 0;
	 */
	//gsm._tf.setTimeout(_GSM_CONNECTION_TOUT_);

	//_cell.flush();

	// Set port

	gsm.SimpleWrite(F("AT+CIPSERVER=1,"));
	gsm.SimpleWrite(port);
	gsm.print("\r");

	switch(gsm.WaitResp(5000, 50, "OK")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0; 
		break;
	}

	if(gsm.WaitResp(5000, 50, "SERVER") != GSM::RX_FINISHED_STR_RECV)
		return 0; 


	delay(200);

	return 1;

}

bool InetGSM::connectedClient()
{
	/*
  if (getStatus()!=TCPSERVERWAIT)
     return 0;
	 */

	gsm.SimpleWrite(F("AT+CIPSTATUS"));
	// Alternative: AT+QISTAT, although it may be necessary to call an AT
	// command every second,which is not wise
	gsm.print("\r");

	switch(gsm.WaitResp(1000, 200, "OK")){
	case GSM::RX_TMOUT_ERR:
		return 0;
		break;
	case GSM::RX_FINISHED_STR_NOT_RECV:
		return 0; 
		break;
	}
	//gsm._tf.setTimeout(1);
	if(gsm.WaitResp(5000, 50, "CONNECT OK")!=GSM::RX_FINISHED_STR_RECV)
	{
		gsm.setGSMStatus(GSM::GSM_TCPCONNECTEDSERVER);
		return true;
	}
	else
		return false;
}

int InetGSM::WriteINET(unsigned int outlen, char *outbuf)
{

	if(!beginWriteInet())
		return -1;

	unsigned int i = 0;
	while(((gsm.getGSMStatus() == GSM::GSM_TCPCONNECTEDSERVER)||
			(gsm.getGSMStatus() == GSM::GSM_TCPCONNECTEDCLIENT))&&(i < outlen))
	{
		gsm.putch(outbuf[i]);
		i++;
	}
	DATA(F("Data wrote to modem"));
	DATA(i);
	if(endWriteInet())
		return i;
	else return -2;
}

int  InetGSM::ReadINET(unsigned int inlen, char *inbuf)
{
	if((gsm.getGSMStatus() == GSM::GSM_TCPCONNECTEDSERVER)||
			(gsm.getGSMStatus() == GSM::GSM_TCPCONNECTEDCLIENT))
		return gsm.read((unsigned char *)inbuf, inlen);
	else
	{
		WARNING(F("Reading: modem not have connection with server"));
		return -1;
	}
}

bool InetGSM::beginWriteInet()
{
	gsm.SimpleWrite(F("AT+CIPSEND\r"));
	if(gsm.WaitResp(500, 200, ">") != GSM::RX_FINISHED_STR_RECV)
	{
		WARNING(F("Error entering data mode"));
		return false;
	}
	DEBUG(F("Sending data to inet.."));
	gsm.setCOMStatus(GSM::CLS_DATA);
	return true;
}

bool InetGSM::endWriteInet()
{
	gsm.putch(ctrlz);
	//	gsm.putch(ctrlz);
	gsm.setCOMStatus(GSM::CLS_FREE);
	if(gsm.WaitResp(20000, 10000, "SEND OK") == GSM::RX_FINISHED_STR_RECV)
	{
		DEBUG(F("Data sends"));
		return true;
	}

	CRITICAL(F("Error: modem not say SEND OK after transmit data"));
	return false;
}

bool InetGSM::refreshTime(const char* domain, const char* ntp)
{
	if (GSM::CLS_FREE != gsm.getCOMStatus()) return false;
	gsm.setCOMStatus(GSM::CLS_ATCMD);


	INFO(F("Refreshing time via NTP"));
	INFO(ntp);

	char buf[18];

	gsm.print(F("AT+SAPBR=0,1\r"));

	if(gsm.WaitResp(3000, 200, "OK") != GSM::RX_FINISHED_STR_RECV)
		DEBUG(F("Bearer profile not closed"));

	gsm.print(F("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r"));
	if(gsm.WaitResp(3000, 200, "OK") != GSM::RX_FINISHED_STR_RECV)
	{
		WARNING(F("Configure bearer profile: error while set CONTYPE"));
		gsm.setCOMStatus(GSM::CLS_FREE);
		return false;
	}

	gsm.print(F("AT+SAPBR=3,1,\"APN\",\""));
	gsm.print(domain);
	gsm.print(F("\"\r"));
	if(gsm.WaitResp(3000, 200, "OK") != GSM::RX_FINISHED_STR_RECV)
	{
		WARNING(F("Configure bearer profile: error while set APN"));
		gsm.setCOMStatus(GSM::CLS_FREE);
		return false;
	}

	gsm.print(F("AT+SAPBR=1,1\r"));
	if(gsm.WaitResp(3000, 2000, "OK") != GSM::RX_FINISHED_STR_RECV)
	{
		WARNING(F("Configure bearer profile: error while set connecting"));
		gsm.setCOMStatus(GSM::CLS_FREE);
		return false;
	}
	else INFO(F("SAPBR connected"));

	gsm.print(F("AT+SAPBR=2,1\r"));
	if(gsm.WaitResp(3000, 2000, "OK") == GSM::RX_FINISHED_STR_RECV)
	{
		gsm.getString("\"", "\"", buf, 16);
		DEBUG(buf);
	}

	gsm.print(F("AT+CNTP=\"pool.ntp.org\",0,1,2\r"));
	if(gsm.WaitResp(3000, 2000, "OK") != GSM::RX_FINISHED_STR_RECV)
	{
		WARNING(F("Set NTP parameter: error while set NTP server"));
		gsm.setCOMStatus(GSM::CLS_FREE);
		return false;
	}

	gsm.print(F("AT+CNTP\r"));
	if(gsm.WaitResp(3000, 2000, "OK") != GSM::RX_FINISHED_STR_RECV)
	{
		WARNING(F("Set NTP parameter: error while refreshing time via NTP"));
		gsm.setCOMStatus(GSM::CLS_FREE);
		return false;
	}
	else
	{
		INFO(F("Time refreshed via NTP"));
		gsm.getString("\"", "\"", buf, 17);
		INFO(buf);
	}

	gsm.print(F("AT+SAPBR=0,1\r"));
	if(gsm.WaitResp(3000, 200, "OK") != GSM::RX_FINISHED_STR_RECV)
		DEBUG(F("Bearer profile not closed"));

	gsm.setCOMStatus(GSM::CLS_FREE);
	return true;
}
