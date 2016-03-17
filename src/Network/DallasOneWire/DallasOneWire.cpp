/*
 * DallasOneWire.cpp
 *
 *  Created on: 28 серп. 2015 р.
 *      Author: sd
 */

#include "DallasOneWire.h"
#include "util/delay.h"
#include "../LOG/debug.h"
#include "avr/io.h"
#include <stdio.h>

#define WriteCOM(x,y) write(y,x)
#define ReadCOM(x,y) read(y,x)
#define BreakCOM() doBreak()

DallasOneWire::DallasOneWire(volatile unsigned char & udr) :
UART(udr, UART_BASE_SPEED, UART_TX_BUFFER, UART_RX_BUFFER)
{
	LastDiscrepancy = 0;
	LastDeviceFlag = FALSE;
	LastFamilyDiscrepancy = 0;

	// reset modes
	UMode = MODSEL_COMMAND;
	UBaud = PARMSET_9600;
	USpeed = SPEEDSEL_FLEX;
	ULevel = MODE_NORMAL;

	crc8 = 0;
}


//---------------------------------------------------------------------------
//-------- Basic 1-Wire functions
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Reset all of the devices on the 1-Wire Net and return the result.
//
// Returns: TRUE(1):  presense pulse(s) detected, device(s) reset
//          FALSE(0): no presense pulses detected
//
// WARNING: Without setting the above global (FAMILY_CODE_04_ALARM_TOUCHRESET_COMPLIANCE)
//          to TRUE, this routine will not function correctly on some
//          Alarm reset types of the DS1994/DS1427/DS2404 with
//          Rev 1,2, and 3 of the DS2480/DS2480B.
//
//
int DallasOneWire::OWReset(void)
{
	DATA(F("OWReset"));
	unsigned char readbuffer[10],sendpacket[10];
	unsigned char sendlen=0;

	// make sure normal level
	OWLevel(MODE_NORMAL);

	// check for correct mode
	if (UMode != MODSEL_COMMAND)
	{
		UMode = MODSEL_COMMAND;
		sendpacket[sendlen++] = MODE_COMMAND;
	}

	// construct the command
	sendpacket[sendlen++] = (unsigned char)(CMD_COMM | FUNCTSEL_RESET | USpeed);

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the 1 byte response
		if (ReadCOM(1,readbuffer) == 1)
		{
			//_delay_ms(5); // delay 5 ms to give DS1994 enough time
			flush();
			//return TRUE;


			// make sure this byte looks like a reset byte
			if (((readbuffer[0] & RB_RESET_MASK) == RB_PRESENCE) ||
					((readbuffer[0] & RB_RESET_MASK) == RB_ALARMPRESENCE))
				return TRUE;
		}
		else DEBUG(F("Read error"));
	}

	WARNING(F("An error occurred so re-sync with DS2480B"));
	// an error occurred so re-sync with DS2480B
	DS2480B_Detect();

	return FALSE;
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net.
// The parameter 'sendbit' least significant bit is used.
//
// 'sendbit' - 1 bit to send (least significant byte)
//
void DallasOneWire::OWWriteBit(unsigned char sendbit)
{
	OWTouchBit(sendbit);
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// Returns:  8 bits read from 1-Wire Net
//
unsigned char DallasOneWire::OWReadBit(void)
{
	return OWTouchBit(0x01);
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and return the
// result 1 bit read from the 1-Wire Net.  The parameter 'sendbit'
// least significant bit is used and the least significant bit
// of the result is the return bit.
//
// 'sendbit' - the least significant bit is the bit to send
//
// Returns: 0:   0 bit read from sendbit
//          1:   1 bit read from sendbit
//
unsigned char DallasOneWire::OWTouchBit(unsigned char sendbit)
{
	DATA("OWTouchBit");
	unsigned char readbuffer[10],sendpacket[10];
	unsigned char sendlen=0;

	// make sure normal level
	OWLevel(MODE_NORMAL);

	// check for correct mode
	if (UMode != MODSEL_COMMAND)
	{
		UMode = MODSEL_COMMAND;
		sendpacket[sendlen++] = MODE_COMMAND;
	}

	// construct the command
	sendpacket[sendlen] = (sendbit != 0) ? BITPOL_ONE : BITPOL_ZERO;
	sendpacket[sendlen++] |= CMD_COMM | FUNCTSEL_BIT | USpeed;

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the response
		if (ReadCOM(1,readbuffer) == 1)
		{
			// interpret the response
			if (((readbuffer[0] & 0xE0) == 0x80) &&
					((readbuffer[0] & RB_BIT_MASK) == RB_BIT_ONE))
				return 1;
			else
				return 0;
		}
		else DEBUG(F("Read error"));
	}

	WARNING(F("OWTouchBit: An error occured so re-sync with DS2480B"));
	// an error occured so re-sync with DS2480B
	DS2480B_Detect();

	return 0;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.
//
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  TRUE: bytes written and echo was the same
//           FALSE: echo was not the same
//
void DallasOneWire::OWWriteByte(unsigned char sendbyte)
{
	OWTouchByte(sendbyte);
}

//--------------------------------------------------------------------------
// Send 8 bits of read communication to the 1-Wire Net and and return the
// result 8 bits read from the 1-Wire Net.
//
// Returns:  8 bits read from 1-Wire Net
//
unsigned char DallasOneWire::OWReadByte(void)
{
	return OWTouchByte(0xFF);
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and return the
// result 8 bits read from the 1-Wire Net.  The parameter 'sendbyte'
// least significant 8 bits are used and the least significant 8 bits
// of the result is the return byte.
//
// 'sendbyte' - 8 bits to send (least significant byte)
//
// Returns:  8 bits read from sendbyte
//
unsigned char DallasOneWire::OWTouchByte(unsigned char sendbyte)
{
	DATA(F("OWTouchByte"));
	unsigned char readbuffer[10],sendpacket[10];
	unsigned char sendlen=0;

	// make sure normal level
	OWLevel(MODE_NORMAL);

	// check for correct mode
	if (UMode != MODSEL_DATA)
	{
		UMode = MODSEL_DATA;
		sendpacket[sendlen++] = MODE_DATA;
	}

	// add the byte to send
	sendpacket[sendlen++] = (unsigned char)sendbyte;

	// check for duplication of data that looks like COMMAND mode
	if (sendbyte ==(int)MODE_COMMAND)
		sendpacket[sendlen++] = (unsigned char)sendbyte;

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the 1 byte response
		if (ReadCOM(1,readbuffer) == 1)
		{
			// return the response
			return readbuffer[0];
		}
		else DEBUG(F("Read error"));
	}

	WARNING(F("OWTouchByte: An error occured so re-sync with DS2480B"));
	// an error occured so re-sync with DS2480B
	DS2480B_Detect();

	return 0;
}

//--------------------------------------------------------------------------
// The 'OWBlock' transfers a block of data to and from the
// 1-Wire Net. The result is returned in the same buffer.
//
// 'tran_buf' - pointer to a block of unsigned
//              chars of length 'tran_len' that will be sent
//              to the 1-Wire Net
// 'tran_len' - length in bytes to transfer
//
// Returns:   TRUE (1) : If the buffer transfer was succesful.
//            FALSE (0): If an error occured.
//
//  The maximum tran_length is (160)
//
int DallasOneWire::OWBlock(unsigned char *tran_buf, int tran_len)
{
	DATA(F("OWBlock"));

	unsigned char sendpacket[320];
	unsigned char sendlen=0;

	// check for a block too big
	if (tran_len > 160)
		return FALSE;

	// construct the packet to send to the DS2480B
	// check for correct mode
	if (UMode != MODSEL_DATA)
	{
		UMode = MODSEL_DATA;
		sendpacket[sendlen++] = MODE_DATA;
	}

	// add the bytes to send
	for (int i = 0; i < tran_len; i++)
	{
		sendpacket[sendlen++] = tran_buf[i];

		// duplicate data that looks like COMMAND mode
		if (tran_buf[i] == MODE_COMMAND)
			sendpacket[sendlen++] = tran_buf[i];
	}

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the response
		if (ReadCOM(tran_len,tran_buf) == tran_len)
		{
			return TRUE;
		}
		else DEBUG(F("Read error"));
	}

	WARNING(F("OWBlock: An error occured so re-sync with DS2480B"));
	// an error occured so re-sync with DS2480B
	DS2480B_Detect();

	return FALSE;
}

//--------------------------------------------------------------------------
// Find the 'first' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : no device present
//
int DallasOneWire::OWFirst()
{
	DATA(F("OWFirst - reset the search state"));
	// reset the search state
	LastDiscrepancy = 0;
	LastDeviceFlag = FALSE;
	LastFamilyDiscrepancy = 0;

	return OWSearch();
}

//--------------------------------------------------------------------------
// Find the 'next' devices on the 1-Wire bus
// Return TRUE  : device found, ROM number in ROM_NO buffer
//        FALSE : device not found, end of search
//
int DallasOneWire::OWNext()
{
	DATA(F("OWNext - leave the search state alone"));
	// leave the search state alone
	return OWSearch();
}

//--------------------------------------------------------------------------
// Verify the device with the ROM number in ROM_NO buffer is present.
// Return TRUE  : device verified present
//        FALSE : device not present
//
int DallasOneWire::OWVerify()
{
	DATA(F("OWVerify"));
	unsigned char rom_backup[8];
	int i,rslt,ld_backup,ldf_backup,lfd_backup;

	// keep a backup copy of the current state
	for (i = 0; i < 8; i++)
		rom_backup[i] = ROM_NO[i];
	ld_backup = LastDiscrepancy;
	ldf_backup = LastDeviceFlag;
	lfd_backup = LastFamilyDiscrepancy;

	// set search to find the same device
	LastDiscrepancy = 64;
	LastDeviceFlag = FALSE;

	if (OWSearch())
	{
		// check if same device found
		rslt = TRUE;
		for (i = 0; i < 8; i++)
		{
			if (rom_backup[i] != ROM_NO[i])
			{
				rslt = FALSE;
				break;
			}
		}
	}
	else
	{

		DEBUG(F("Search error when verify"));
		rslt = FALSE;
	}

	// restore the search state
	for (i = 0; i < 8; i++)
		ROM_NO[i] = rom_backup[i];
	LastDiscrepancy = ld_backup;
	LastDeviceFlag = ldf_backup;
	LastFamilyDiscrepancy = lfd_backup;

	// return the result of the verify
	return rslt;
}

//--------------------------------------------------------------------------
// Setup the search to find the device type 'family_code' on the next call
// to OWNext() if it is present.
//
void DallasOneWire::OWTargetSetup(unsigned char family_code)
{
	DATA(F("OWTargetSetup"));
	DATA(family_code);

	// set the search state to find SearchFamily type devices
	ROM_NO[0] = family_code;
	for (auto i = 1; i < 8; i++)
		ROM_NO[i] = 0;
	LastDiscrepancy = 64;
	LastFamilyDiscrepancy = 0;
	LastDeviceFlag = FALSE;
}

//--------------------------------------------------------------------------
// Setup the search to skip the current device type on the next call
// to OWNext().
//
void DallasOneWire::OWFamilySkipSetup()
{
	// set the Last discrepancy to last family discrepancy
	LastDiscrepancy = LastFamilyDiscrepancy;

	// clear the last family discrpepancy
	LastFamilyDiscrepancy = 0;

	// check for end of list
	if (LastDiscrepancy == 0)
		LastDeviceFlag = TRUE;

}

//--------------------------------------------------------------------------
// The 'OWSearch' function does a general search.  This function
// continues from the previos search state. The search state
// can be reset by using the 'OWFirst' function.
// This function contains one parameter 'alarm_only'.
// When 'alarm_only' is TRUE (1) the find alarm command
// 0xEC is sent instead of the normal search command 0xF0.
// Using the find alarm command 0xEC will limit the search to only
// 1-Wire devices that are in an 'alarm' state.
//
// Returns:   TRUE (1) : when a 1-Wire device was found and it's
//                       Serial Number placed in the global ROM
//            FALSE (0): when no new device was found.  Either the
//                       last search was the last device or there
//                       are no devices on the 1-Wire Net.
//
int DallasOneWire::OWSearch(void)
{
	DATA(F("OWSearch"));

	unsigned char last_zero,pos;
	unsigned char tmp_rom[8];
	unsigned char readbuffer[20],sendpacket[40];
	unsigned char i,sendlen=0;

	// if the last call was the last one
	if (LastDeviceFlag)
	{
		DATA(F("OWSearch: the last call was the last one"));
		// reset the search
		LastDiscrepancy = 0;
		LastDeviceFlag = FALSE;
		LastFamilyDiscrepancy = 0;
		return FALSE;
	}

	// reset the 1-wire
	// if there are no parts on 1-wire, return FALSE
	if (!OWReset())
	{
		DEBUG(F("OWSearch: there are no parts on 1-wire, return FALSE"));
		// reset the search
		LastDiscrepancy = 0;
		LastFamilyDiscrepancy = 0;
		return FALSE;
	}

	// build the command stream
	// call a function that may add the change mode command to the buff
	// check for correct mode
	if (UMode != MODSEL_DATA)
	{
		DATA(F("OWSearch: UMode != MODSEL_DATA"));
		UMode = MODSEL_DATA;
		sendpacket[sendlen++] = MODE_DATA;
	}

	// search command
	sendpacket[sendlen++] = 0xF0;

	// change back to command mode
	UMode = MODSEL_COMMAND;
	sendpacket[sendlen++] = MODE_COMMAND;

	// search mode on
	sendpacket[sendlen++] = (unsigned char)(CMD_COMM | FUNCTSEL_SEARCHON | USpeed);

	// change back to data mode
	UMode = MODSEL_DATA;
	sendpacket[sendlen++] = MODE_DATA;

	// set the temp Last Discrepancy to 0
	last_zero = 0;

	// add the 16 bytes of the search
	pos = sendlen;
	for (i = 0; i < 16; i++)
		sendpacket[sendlen++] = 0;

	// only modify bits if not the first search
	if (LastDiscrepancy != 0)
	{
		DATA(F("OWSearch: LastDiscrepancy != 0"));
		// set the bits in the added buffer
		for (i = 0; i < 64; i++)
		{
			// before last discrepancy
			if (i < (LastDiscrepancy - 1))
				bitacc(WRITE_FUNCTION,
						bitacc(READ_FUNCTION,0,i,&ROM_NO[0]),
						(short)(i * 2 + 1),
						&sendpacket[pos]);
			// at last discrepancy
			else if (i == (LastDiscrepancy - 1))
				bitacc(WRITE_FUNCTION,1,(short)(i * 2 + 1),
						&sendpacket[pos]);
			// after last discrepancy so leave zeros
		}
	}

	// change back to command mode
	UMode = MODSEL_COMMAND;
	sendpacket[sendlen++] = MODE_COMMAND;

	// search OFF command
	sendpacket[sendlen++] = (unsigned char)(CMD_COMM | FUNCTSEL_SEARCHOFF | USpeed);

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the 1 byte response
		if (ReadCOM(17,readbuffer) == 17)
		{
			// interpret the bit stream
			for (i = 0; i < 64; i++)
			{
				// get the ROM bit
				bitacc(WRITE_FUNCTION,
						bitacc(READ_FUNCTION,0,(short)(i * 2 + 1),&readbuffer[1]),i,
						&tmp_rom[0]);
				// check LastDiscrepancy
				if ((bitacc(READ_FUNCTION,0,(short)(i * 2),&readbuffer[1]) == 1) &&
						(bitacc(READ_FUNCTION,0,(short)(i * 2 + 1),&readbuffer[1]) == 0))
				{
					last_zero = i + 1;
					// check LastFamilyDiscrepancy
					if (i < 8)
						LastFamilyDiscrepancy = i + 1;
				}
			}

			// do dowcrc
			crc8 = 0;
			for (i = 0; i < 8; i++)
				docrc8(tmp_rom[i]);

			// check results
			if ((crc8 != 0) || (LastDiscrepancy == 63) || (tmp_rom[0] == 0))
			{
				// error during search
				// reset the search
				LastDiscrepancy = 0;
				LastDeviceFlag = FALSE;
				LastFamilyDiscrepancy = 0;
				return FALSE;
			}
			// successful search
			else
			{
				// set the last discrepancy
				LastDiscrepancy = last_zero;

				// check for last device
				if (LastDiscrepancy == 0)
					LastDeviceFlag = TRUE;

				// copy the ROM to the buffer
				for (i = 0; i < 8; i++)
					ROM_NO[i] = tmp_rom[i];

				return TRUE;
			}
		}
	}

	WARNING(F("OWSearch: an error occured so re-sync with DS2480B"));
	// an error occured so re-sync with DS2480B
	DS2480B_Detect();

	// reset the search
	LastDiscrepancy = 0;
	LastDeviceFlag = FALSE;
	LastFamilyDiscrepancy = 0;

	return FALSE;
}

//---------------------------------------------------------------------------
//-------- Extended 1-Wire functions
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Set the 1-Wire Net communucation speed.
//
// 'new_speed' - new speed defined as
//                MODE_NORMAL     0x00
//                MODE_OVERDRIVE  0x01
//
// Returns:  current 1-Wire Net speed
//
int DallasOneWire::OWSpeed(int new_speed)
{
	DATA("OWSpeed");
	unsigned char sendpacket[5];
	unsigned char sendlen=0;
	unsigned char rt = FALSE;

	// check if change from current mode
	if (((new_speed == MODE_OVERDRIVE) &&
			(USpeed != SPEEDSEL_OD)) ||
			((new_speed == MODE_NORMAL) &&
					(USpeed != SPEEDSEL_FLEX)))
	{
		if (new_speed == MODE_OVERDRIVE)
		{
			// if overdrive then switch to 115200 baud
			if (DS2480B_ChangeBaud(MAX_BAUD) == MAX_BAUD)
			{
				USpeed = SPEEDSEL_OD;
				rt = TRUE;
			}

		}
		else if (new_speed == MODE_NORMAL)
		{
			// else normal so set to 9600 baud
			if (DS2480B_ChangeBaud(PARMSET_9600) == PARMSET_9600)
			{
				USpeed = SPEEDSEL_FLEX;
				rt = TRUE;
			}

		}

		// if baud rate is set correctly then change DS2480B speed
		if (rt)
		{
			// check for correct mode
			if (UMode != MODSEL_COMMAND)
			{
				UMode = MODSEL_COMMAND;
				sendpacket[sendlen++] = MODE_COMMAND;
			}

			// proceed to set the DS2480B communication speed
			sendpacket[sendlen++] = CMD_COMM | FUNCTSEL_SEARCHOFF | USpeed;

			// send the packet
			if (!WriteCOM(sendlen,sendpacket))
			{
				rt = FALSE;
				WARNING(F("OWSpeed - lost communication with DS2480B then reset"));
				// lost communication with DS2480B then reset
				DS2480B_Detect();
			}
		}

	}

	// return the current speed
	return (USpeed == SPEEDSEL_OD) ? MODE_OVERDRIVE : MODE_NORMAL;
}

//--------------------------------------------------------------------------
// Set the 1-Wire Net line level.  The values for new_level are
// as follows:
//
// 'new_level' - new level defined as
//                MODE_NORMAL     0x00
//                MODE_STRONG5    0x02
//
// Returns:  current 1-Wire Net level
//
int DallasOneWire::OWLevel(int new_level)
{
	DATA("OWLevel");
	DATA(new_level);

	unsigned char sendpacket[10],readbuffer[10];
	unsigned char sendlen=0;
	unsigned char rt=FALSE;//,docheck=FALSE;

	// check if need to change level
	if (new_level != ULevel)
	{
		// check for correct mode
		if (UMode != MODSEL_COMMAND)
		{
			UMode = MODSEL_COMMAND;
			sendpacket[sendlen++] = MODE_COMMAND;
		}

		// check if just putting back to normal
		if (new_level == MODE_NORMAL)
		{
			//			// check for disable strong pullup step
			//			if (ULevel == MODE_STRONG5)
			//				docheck = TRUE;

			// stop pulse command
			sendpacket[sendlen++] = MODE_STOP_PULSE;

			// add the command to begin the pulse WITHOUT prime
			sendpacket[sendlen++] = CMD_COMM | FUNCTSEL_CHMOD | SPEEDSEL_PULSE | BITPOL_5V | PRIME5V_FALSE;

			// stop pulse command
			sendpacket[sendlen++] = MODE_STOP_PULSE;

			// flush the buffers
			flush();

			// send the packet
			if (WriteCOM(sendlen,sendpacket))
			{
				// read back the 1 byte response
				if (ReadCOM(2,readbuffer) == 2)
				{
					// check response byte
					if (((readbuffer[0] & 0xE0) == 0xE0) && ((readbuffer[1] & 0xE0) == 0xE0))
					{
						rt = TRUE;
						ULevel = MODE_NORMAL;

					}

					else DEBUG(F("Read error"));
				}
			}
		}
		// set new level
		else
		{
			// set the SPUD time value
			sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_5VPULSE | PARMSET_infinite;
			// add the command to begin the pulse
			sendpacket[sendlen++] = CMD_COMM | FUNCTSEL_CHMOD | SPEEDSEL_PULSE | BITPOL_5V;

			// flush the buffers
			flush();

			// send the packet
			if (WriteCOM(sendlen,sendpacket))
			{
				// read back the 1 byte response from setting time limit
				if (ReadCOM(1,readbuffer) == 1)
				{
					// check response byte
					if ((readbuffer[0] & 0x81) == 0)
					{
						ULevel = new_level;
						rt = TRUE;
					}
					else DEBUG(F("Read error"));
				}
			}
		}

		// if lost communication with DS2480B then reset
		if (rt != TRUE)
		{
			WARNING(F("OWLevel - lost communication with DS2480B then reset"));
			DS2480B_Detect();
		}
	}

	// return the current level
	return ULevel;
}

//--------------------------------------------------------------------------
// This procedure creates a fixed 480 microseconds 12 volt pulse
// on the 1-Wire Net for programming EPROM iButtons.
//
// Returns:  TRUE  successful
//           FALSE program voltage not available
//
int DallasOneWire::OWProgramPulse(void)
{
	DATA(F("OWProgramPulse"));

	unsigned char sendpacket[10],readbuffer[10];
	unsigned char sendlen=0;

	// make sure normal level
	OWLevel(MODE_NORMAL);

	// check for correct mode
	if (UMode != MODSEL_COMMAND)
	{
		UMode = MODSEL_COMMAND;
		sendpacket[sendlen++] = MODE_COMMAND;
	}

	// set the SPUD time value
	sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_12VPULSE | PARMSET_512us;

	// pulse command
	sendpacket[sendlen++] = CMD_COMM | FUNCTSEL_CHMOD | BITPOL_12V | SPEEDSEL_PULSE;

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the 2 byte response
		if (ReadCOM(2,readbuffer) == 2)
		{
			// check response byte
			if (((readbuffer[0] | CMD_CONFIG) ==
					(CMD_CONFIG | PARMSEL_12VPULSE | PARMSET_512us)) &&
					((readbuffer[1] & 0xFC) ==
							(0xFC & (CMD_COMM | FUNCTSEL_CHMOD | BITPOL_12V | SPEEDSEL_PULSE))))
				return TRUE;
		}
		else DEBUG(F("Read error"));
	}

	// an error occured so re-sync with DS2480B
	WARNING(F("OWProgramPulse - lost communication with DS2480B then reset"));
	DS2480B_Detect();

	return FALSE;
}

//--------------------------------------------------------------------------
// Send 8 bits of communication to the 1-Wire Net and verify that the
// 8 bits read from the 1-Wire Net is the same (write operation).
// The parameter 'sendbyte' least significant 8 bits are used.  After the
// 8 bits are sent change the level of the 1-Wire net.
//
// 'sendbyte' - 8 bits to send (least significant bit)
//
// Returns:  TRUE: bytes written and echo was the same, strong pullup now on
//           FALSE: echo was not the same
//
int DallasOneWire::OWWriteBytePower(int sendbyte)
{
	DATA(F("OWWriteBytePower"));

	unsigned char sendpacket[10],readbuffer[10];
	unsigned char sendlen=0;
	unsigned char rt=FALSE;
	unsigned char i, temp_byte;

	// check for correct mode
	if (UMode != MODSEL_COMMAND)
	{
		UMode = MODSEL_COMMAND;
		sendpacket[sendlen++] = MODE_COMMAND;
	}

	// set the SPUD time value
	sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_5VPULSE | PARMSET_infinite;

	// construct the stream to include 8 bit commands with the last one
	// enabling the strong-pullup
	temp_byte = sendbyte;
	for (i = 0; i < 8; i++)
	{
		sendpacket[sendlen++] = ((temp_byte & 0x01) ? BITPOL_ONE : BITPOL_ZERO)
                            																						  | CMD_COMM | FUNCTSEL_BIT | USpeed |
																													  ((i == 7) ? PRIME5V_TRUE : PRIME5V_FALSE);
		temp_byte >>= 1;
	}

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the 9 byte response from setting time limit
		if (ReadCOM(9,readbuffer) == 9)
		{
			// check response
			if ((readbuffer[0] & 0x81) == 0)
			{
				// indicate the port is now at power delivery
				ULevel = MODE_STRONG5;

				// reconstruct the echo byte
				temp_byte = 0;
				for (i = 0; i < 8; i++)
				{
					temp_byte >>= 1;
					temp_byte |= (readbuffer[i + 1] & 0x01) ? 0x80 : 0;
				}

				if (temp_byte == sendbyte)
					rt = TRUE;
			}
			else DEBUG(F("response error"));
		}
		else DEBUG(F("Read error"));
	}

	// if lost communication with DS2480B then reset
	if (rt != TRUE)
	{
		WARNING(F("OWWriteBytePower - lost communication with DS2480B then reset"));
		DS2480B_Detect();
	}

	return rt;
}

//--------------------------------------------------------------------------
// Send 1 bit of communication to the 1-Wire Net and verify that the
// response matches the 'applyPowerResponse' bit and apply power delivery
// to the 1-Wire net.  Note that some implementations may apply the power
// first and then turn it off if the response is incorrect.
//
// 'applyPowerResponse' - 1 bit response to check, if correct then start
//                        power delivery
//
// Returns:  TRUE: bit written and response correct, strong pullup now on
//           FALSE: response incorrect
//
int DallasOneWire::OWReadBitPower(int applyPowerResponse)
{
	DATA(F("OWReadBitPower"));
	unsigned char sendpacket[3],readbuffer[3];
	unsigned char sendlen=0;
	unsigned char rt=FALSE;

	// check for correct mode
	if (UMode != MODSEL_COMMAND)
	{
		UMode = MODSEL_COMMAND;
		sendpacket[sendlen++] = MODE_COMMAND;
	}

	// set the SPUD time value
	sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_5VPULSE | PARMSET_infinite;

	// enabling the strong-pullup after bit
	sendpacket[sendlen++] = BITPOL_ONE
			| CMD_COMM | FUNCTSEL_BIT | USpeed |
			PRIME5V_TRUE;
	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the 2 byte response from setting time limit
		if (ReadCOM(2,readbuffer) == 2)
		{
			// check response to duration set
			if ((readbuffer[0] & 0x81) == 0)
			{
				// indicate the port is now at power delivery
				ULevel = MODE_STRONG5;

				// check the response bit
				if ((readbuffer[1] & 0x01) == applyPowerResponse)
					rt = TRUE;
				else
					OWLevel(MODE_NORMAL);

				return rt;
			}
			else DEBUG(F("response error"));
		}
		else DEBUG(F("Read error"));
	}

	// if lost communication with DS2480B then reset
	if (rt != TRUE)
	{
		WARNING(F("OWReadBitPower - lost communication with DS2480B then reset"));
		DS2480B_Detect();
	}

	return rt;
}

//---------------------------------------------------------------------------
//-------- DS2480B functions
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Attempt to resyc and detect a DS2480B and set the FLEX parameters
//
// Returns:  TRUE  - DS2480B detected successfully
//           FALSE - Could not detect DS2480B
//
int DallasOneWire::DS2480B_Detect(void)
{
	INFO(F("DS2480B Detecting..."));

	unsigned char sendpacket[10],readbuffer[10];
	unsigned char sendlen=0;

	static char errorCount = 0;
	static bool firstInit = true;

	// reset modes
	UMode = MODSEL_COMMAND;
	UBaud = PARMSET_9600;
	USpeed = SPEEDSEL_FLEX;

	// set the baud rate to 9600
	SetBaudCOM((unsigned char)UBaud);

	// send a break to reset the DS2480B
	BreakCOM();

	// delay to let line settle
	_delay_ms(20);

	// flush the buffers
	flush();

	// send the timing byte
	sendpacket[0] = 0xC5;//0xC1;
	if (WriteCOM(1,sendpacket) != 1)
	{
		CRITICAL(F("DS2480B_Detect: DS2480B Not Answer"));
		return FALSE;
	}

	// delay to let line settle
	_delay_ms(150);

	// set the FLEX configuration parameters
	// default PDSRC = 1.37Vus
	sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_SLEW | PARMSET_Slew1p37Vus; //0x17
	// default W1LT = 10us
	sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_WRITE1LOW | PARMSET_Write10us; //0x45
	// default DSO/WORT = 8us
	sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_SAMPLEOFFSET | PARMSET_SampOff8us; //0x5B

	// construct the command to read the baud rate (to test command block)
	sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_PARMREAD | (PARMSEL_BAUDRATE >> 3); //0x0F

	// also do 1 bit operation (to test 1-Wire block)
	sendpacket[sendlen++] = CMD_COMM | FUNCTSEL_BIT | UBaud | BITPOL_ONE; //0x91

	// flush the buffers
	flush();

	// send the packet
	if (WriteCOM(sendlen,sendpacket))
	{
		// read back the response
		int readLen = ReadCOM(5,readbuffer);
		if (readLen == 5)
		{
			// look at the baud rate and bit operation
			// to see if the response makes sense
			if (((readbuffer[3] & 0xF1) == 0x00) && //3
					((readbuffer[3] & 0x0E) == UBaud) && //3
					((readbuffer[4] & 0xF0) == 0x90) && //4
					((readbuffer[4] & 0x0C) == UBaud)) //4
			{
				DEBUG(F("DS2480B Detect: OK"));
				errorCount = 0;
				return TRUE;
			}
			else DEBUG(F("DS2480B Detect: Read not math"));
		}
		else
		{
			DEBUG(F("DS2480B Detect: Read length not math"));
			DATA(readLen);
		}
	}

#ifdef LEVEL_DATA
	char buff[40];
	sprintf(buff, "Recived msg is: %X, %X, %X, %X, %X", readbuffer[0],readbuffer[0],readbuffer[0],readbuffer[0],readbuffer[0]);
	DATA(buff);
#endif

	if(++errorCount == DETECT_ERROR_COUNT)
	{
		errorCount = 0;
		DS2480B_HardwareReset();
		firstInit = true;
	}
	if(firstInit) INFO(F("DS2480B: First initialize always without answer"));
	else WARNING(F("DS2480B not detect or not response... :-("));
	firstInit = false;
	return FALSE;
}

//---------------------------------------------------------------------------
// Change the DS2480B from the current baud rate to the new baud rate.
//
// 'newbaud' - the new baud rate to change to, defined as:
//               PARMSET_9600     0x00
//               PARMSET_19200    0x02
//               PARMSET_57600    0x04
//               PARMSET_115200   0x06
//
// Returns:  current DS2480B baud rate.
//
int DallasOneWire::DS2480B_ChangeBaud(unsigned char newbaud)
{
	unsigned char rt=FALSE;
	unsigned char readbuffer[5],sendpacket[5],sendpacket2[5];
	unsigned char sendlen=0,sendlen2=0;

	// see if diffenent then current baud rate
	if (UBaud == newbaud)
		return UBaud;
	else
	{
		// build the command packet
		// check for correct mode
		if (UMode != MODSEL_COMMAND)
		{
			UMode = MODSEL_COMMAND;
			sendpacket[sendlen++] = MODE_COMMAND;
		}
		// build the command
		sendpacket[sendlen++] = CMD_CONFIG | PARMSEL_BAUDRATE | newbaud;

		// flush the buffers
		flush();

		// send the packet
		if (!WriteCOM(sendlen,sendpacket))
			rt = FALSE;
		else
		{
			// make sure buffer is flushed
			//_delay_ms(4);

			// change our baud rate
			SetBaudCOM(newbaud);
			UBaud = newbaud;

			// wait for things to settle
			//_delay_ms(5);

			// build a command packet to read back baud rate
			sendpacket2[sendlen2++] = CMD_CONFIG | PARMSEL_PARMREAD | (PARMSEL_BAUDRATE >> 3);

			// flush the buffers
			flush();

			// send the packet
			if (WriteCOM(sendlen2,sendpacket2))
			{
				//_delay_ms(5);
				// read back the 1 byte response
				if (ReadCOM(1,readbuffer) == 1)
				{
					// verify correct baud
					if (((readbuffer[0] & 0x0E) == (sendpacket[sendlen-1] & 0x0E)))
						rt = TRUE;

					else DEBUG(F("response error"));
				}
				else DEBUG(F("Read error"));
			}
		}
	}

	// if lost communication with DS2480B then reset
	if (rt != TRUE)
		DS2480B_Detect();

	return UBaud;
}

void DallasOneWire::DS2480B_HardwareReset()
{
	CRITICAL(F("DS2480B HardwareReset, because it not answered several times"));

	DALL_PWR_DDR |= (1<<DALL_PWR_PIN);
	DALL_PWR_PORT &= ~(1<<DALL_PWR_PIN);
	_delay_ms(1000);
	DALL_PWR_DDR &= ~(1<<DALL_PWR_PIN);
	_delay_ms(30);
}

//--------------------------------------------------------------------------
// Set the baud rate on the com port.
//
// 'new_baud'  - new baud rate defined as
//                PARMSET_9600     0x00
//                PARMSET_19200    0x02
//                PARMSET_57600    0x04
//                PARMSET_115200   0x06
//
void DallasOneWire::SetBaudCOM(unsigned char new_baud)
{
	// change just the baud rate
	switch (new_baud)
	{
	case PARMSET_115200:
		setBaud(115200);
		break;
	case PARMSET_57600:
		setBaud(57600);
		break;
	case PARMSET_19200:
		setBaud(19200);
		break;
	case PARMSET_9600:
	default:
		setBaud(UART_BASE_SPEED);
		break;
	}

}

//---------------------------------------------------------------------------
//-------- Utility functions
//---------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Bit utility to read and write a bit in the buffer 'buf'.
//
// 'op'    - operation (1) to set and (0) to read
// 'state' - set (1) or clear (0) if operation is write (1)
// 'loc'   - bit number location to read or write
// 'buf'   - pointer to array of bytes that contains the bit
//           to read or write
//
// Returns: 1   if operation is set (1)
//          0/1 state of bit number 'loc' if operation is reading
//
int DallasOneWire::bitacc(int op, int state, int loc, unsigned char *buf)
{
	int nbyt,nbit;

	nbyt = (loc / 8);
	nbit = loc - (nbyt * 8);

	if (op == WRITE_FUNCTION)
	{
		if (state)
			buf[nbyt] |= (0x01 << nbit);
		else
			buf[nbyt] &= ~(0x01 << nbit);

		return 1;
	}
	else
		return ((buf[nbyt] >> nbit) & 0x01);
}



//--------------------------------------------------------------------------
// Calculate the CRC8 of the byte value provided with the current
// global 'crc8' value.
// Returns current global crc8 value
//
unsigned char DallasOneWire::docrc8(unsigned char value)
{
	// See Application Note 27
	// TEST BUILD
	static unsigned char dscrc_table[] = {
			0, 94,188,226, 97, 63,221,131,194,156,126, 32,163,253, 31, 65,
			157,195, 33,127,252,162, 64, 30, 95,  1,227,189, 62, 96,130,220,
			35,125,159,193, 66, 28,254,160,225,191, 93,  3,128,222, 60, 98,
			190,224,  2, 92,223,129, 99, 61,124, 34,192,158, 29, 67,161,255,
			70, 24,250,164, 39,121,155,197,132,218, 56,102,229,187, 89,  7,
			219,133,103, 57,186,228,  6, 88, 25, 71,165,251,120, 38,196,154,
			101, 59,217,135,  4, 90,184,230,167,249, 27, 69,198,152,122, 36,
			248,166, 68, 26,153,199, 37,123, 58,100,134,216, 91,  5,231,185,
			140,210, 48,110,237,179, 81, 15, 78, 16,242,172, 47,113,147,205,
			17, 79,173,243,112, 46,204,146,211,141,111, 49,178,236, 14, 80,
			175,241, 19, 77,206,144,114, 44,109, 51,209,143, 12, 82,176,238,
			50,108,142,208, 83, 13,239,177,240,174, 76, 18,145,207, 45,115,
			202,148,118, 40,171,245, 23, 73,  8, 86,180,234,105, 55,213,139,
			87,  9,235,181, 54,104,138,212,149,203, 41,119,244,170, 72, 22,
			233,183, 85, 11,136,214, 52,106, 43,117,151,201, 74, 20,246,168,
			116, 42,200,150, 21, 75,169,247,182,232, 10, 84,215,137,107, 53};


	// TEST BUILD
	crc8 = dscrc_table[crc8 ^ value];
	return crc8;
}

