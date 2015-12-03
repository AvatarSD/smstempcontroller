/*
 * _1wireTempNetworkController.cpp
 *
 * Created: 30.07.2015 2:51:47
 *  Author: sd
 */

#include "avr/interrupt.h"
#include "stdio.h"
#include "init/init.h"
#include "LOG/debug.h"
#include "Network/NetworkWorker.h"
#include "ADC/Analog.h"
#include "config.h"
#include "LOG/SDCardLogger.h"

/*Additional for debug*/
#include "LiquidCrystal_I2C/lcdTerminal/lcdterminal.h"
#include "sdcard/ff.h"
#include "init/freeram.h"
/**********************/

#define LED_ON PORTB |= (1<<PORTB7)
#define LED_OFF PORTB &=~ (1<<PORTB7)
#define LED_TRN PORTB ^= (1<<PORTB7)



UART * _iface;
NetworkWorker * _network;

ISR(USART1_RX_vect)
{
	_iface->rx_byte_int();
}
ISR(USART1_TX_vect)
{
	_iface->tx_byte_int();
}

void mainLoop()
{
	LED_ON;
	debugSDcardLog.begin();
	debug(F("--------INT---------"));
	static int ntcRefrwshPeriodCount = 0;
	if (!ntcRefrwshPeriodCount)
		_network->refreshTime();
	if (++ntcRefrwshPeriodCount == (NTC_REFRESH_PERIOD * 60 / TIMER_PERIOD))
		ntcRefrwshPeriodCount = 0;
	while (!_network->sendTemp())
		;
	debug(F("-------INTEND-------"));
	debugSDcardLog.end();
	LED_OFF;
}

void screensaver(int time)
{
	char buff[50];
	sprintf(buff, "\nTime to next cycle:\n     %5d:%02d\n\n", time / 60,
			time % 60);
	debug(buff);
}

ISR(TIMER1_CAPT_vect)
{
	static bool mutex = false;
	static unsigned int breakCounter = 0;

#ifdef LEVEL_DATA
	debug(F("[DATA]: ")); debug(F("breakCounter is: ")); debug(breakCounter);
	debug(F("   ")); debug(F("Mutex is: ")); debug(mutex); debug(F("\r\n"));
#endif

	if ((breakCounter == 0) && (!mutex))
	{
		mutex = true;
		mainLoop();
		mutex = false;
	}
	else if ((breakCounter != 0) && (!mutex))
		screensaver(TIMER_PERIOD * 60 - breakCounter);

	++breakCounter;
	if ((breakCounter >= TIMER_PERIOD * 60) && (!mutex))
		breakCounter = 0;
}



int main(void)
{
	init();

	debugSDcardLog.begin();

	lcd_init(LCD_DISP_ON);
	lcd_led(0);

	DallasOneWire iface(UDR1);
	_iface = &iface;

	DallasTemp sensors(iface);

	HardwareData HWdata;

	NetworkWorker network(sensors, HWdata);
	_network = &network;

	//for(auto i = 0; i<5; i++)
	//debug(F("\r\n"));
	debug(F("-------Hello-------\r\n"));
	//debugSDcardLog.begin();
	iface.DS2480B_Detect();

	//Just for see temp in debug
	//while(true)
	//	sensors.readAllTempSerial();

	// Timer/Counter 1 initialization
	// Clock source: System Clock
	// Clock value: 62,500 kHz
	// Mode: CTC top=ICR1
	// OC1A output: Disconnected
	// OC1B output: Disconnected
	// OC1C output: Disconnected
	// Noise Canceler: Off
	// Input Capture on Falling Edge
	// Timer Period: 1 s
	// Timer1 Overflow Interrupt: Off
	// Input Capture Interrupt: On
	// Compare A Match Interrupt: Off
	// Compare B Match Interrupt: Off
	// Compare C Match Interrupt: Off
	TCCR1A = (0 << COM1A1) | (0 << COM1A0) | (0 << COM1B1) | (0 << COM1B0)
					| (0 << COM1C1) | (0 << COM1C0) | (0 << WGM11) | (0 << WGM10);
	TCCR1B = (0 << ICNC1) | (0 << ICES1) | (1 << WGM13) | (1 << WGM12)
					| (1 << CS12) | (0 << CS11) | (0 << CS10);
	TCNT1H = 0x00;
	TCNT1L = 0x00;
	ICR1H = 0xF4;
	ICR1L = 0x23;

	debugSDcardLog("fuking shit!\r\n");

	debugSDcardLog.end();
	// Timer/Counter 1 Interrupt(s) initialization
	TIMSK1=(1<<ICIE1) | (0<<OCIE1C) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);


	//delay(500);

//
//	FATFS FatFs;	// FatFs work area
//	FIL logFile, *fp;	// file object
//
//
//	// init sdcard
//	UINT bw;
//	debug(f_mount(&FatFs, "", 1));		// Give a work area to the FatFs module
//	debug("\r\n");
//	FRESULT res;
//	//fp = (FIL *) malloc(sizeof(FIL));
//
//
//
//
//	SEE_FREE_MEM();
//
//	if(1)
//
//		// read from file
//		fp = (FIL *) malloc(sizeof(FIL));
//	res = f_open(fp, "pj-kk.txt", FA_WRITE | FA_CREATE_ALWAYS);
//	if (res == FR_OK)
//	{	// Create a file
//		debug(F("file opened to read.., res: "));
//		debug(res);
//		debug("\r\n");
//		SEE_FREE_MEM();
//
//		UINT br;
///*
//		debug(F("File size: "));
//		debug(f_size(fp));
//		debug("\n\r");
//		SEE_FREE_MEM();
//
//		char text[255];
//		memset(text, 0, 255);
//		debug(F("Reading res: "));
//		debug(f_read(fp, text, 255, &br));
//		debug("\r\n");
//		SEE_FREE_MEM();
//
//		debug(F("Byte readed: "));
//		debug(br);
//		debug("\r\n");
//		SEE_FREE_MEM();
//
//		debug(F("Text: "));
//		debug(text);
//		debug("\r\n");
//		SEE_FREE_MEM();
//
//		debug(F("lseek res: "));
//		debug(f_lseek(fp, f_size(fp)));
//		debug("\r\n");
//		SEE_FREE_MEM();*/
//
//		debug(F("write res: "));
//		for(int i = 0; i < 100; i++)
//		{
//			char buf[1000];
//			sprintf(buf, "Its my striiiiing --- S.D.!: %d\r\n", i);
//			f_write(fp, buf, strlen(buf), &br);
//		}
//		debug("\r\n");
//		SEE_FREE_MEM();
//
//		debug(F("Byte writen: "));
//		debug(br);
//		debug("\r\n");
//		SEE_FREE_MEM();
//
//		f_close(fp);	// Close the file
//		SEE_FREE_MEM();
//	}
//	else
//	{
//		debug("file not open to read: ");
//		debug(res);
//		debug("\r\n")
//	}
//	free(fp);



	while (1)
		_delay_ms(500);
}
