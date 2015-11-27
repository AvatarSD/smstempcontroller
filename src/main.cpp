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

/**/
#include "LiquidCrystal_I2C/lcdTerminal/lcdterminal.h"
#include "sdcard/ff.h"
/**/

/**/
#include "init/freeram.h"
/**/

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
	debug(F("--------INT---------"));
	static int ntcRefrwshPeriodCount = 0;
	if (!ntcRefrwshPeriodCount)
		_network->refreshTime();
	if (++ntcRefrwshPeriodCount == (NTC_REFRESH_PERIOD * 60 / TIMER_PERIOD))
		ntcRefrwshPeriodCount = 0;
	while (!_network->sendTemp())
		;
	debug(F("-------INTEND-------"));
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

FATFS FatFs;	// FatFs work area
FIL *fp, file;		// fpe object

/*---------------------------------------------------------*/
/* User Provided RTC Function called by FatFs module       */
/* Used to provide a Timestamp for SDCard files and folders*/

DWORD get_fattime(void)
{
	// Returns current time packed into a DWORD variable
	return ((DWORD) (2015 - 1980) << 25)	// Year 2013
			| ((DWORD) 8 << 21)				// Month 7
			| ((DWORD) 2 << 16)				// Mday 28
			| ((DWORD) 20 << 11)				// Hour 0..24
			| ((DWORD) 30 << 5)				// Min 0
			| ((DWORD) 0 >> 1);				// Sec 0
}

int main(void)
{
	init();

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

	//iface.DS2480B_Detect();

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

	// Timer/Counter 1 Interrupt(s) initialization
	//TIMSK1=(1<<ICIE1) | (0<<OCIE1C) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);


	//delay(500);

	// init sdcard
	UINT bw;
	debug(f_mount(&FatFs, "", 1));		// Give a work area to the FatFs module
	debug("\r\n");
	FRESULT res;
	fp = (FIL *) malloc(sizeof(FIL));





	/*// open file
	 res = f_open(fp, "xx2x-SD.txt", FA_CREATE_ALWAYS|FA_WRITE);
	 if (res == FR_OK) {	// Create a file
	 debug(F("file opened to write\r\n"));
	 char text[255] = {"klsjfhgsdklfjgskdfjgskdjfhgsdkfgskd!\r\n\0"};
	 debug(F("Write Res: "));
	 debug(f_write(fp, text, strlen(text), &bw));	// Write data to the file
	 debug("\r\n");
	 //
	 //if (bw == strlen(text)) { //we wrote the entire string
	 //led1.Set(1); // Lights LED if data written well (D4 led on atmega128 board)
	 //lcd.goto_xy(0,0); lcd.send_format_string("Created:%dB\n", f_size(fp));
	 debug(F("File size: ")); debug(f_size(fp)); debug("\n\r");
	 //}
	 //else led2.Set(1);
	 debug(F("Close Res: "));
	 debug(f_close(fp));// Close the file
	 debug(F("\r\nfile closed to write\r\n"));
	 }
	 else{ debug("file not open: ");
	 debug(res);debug("\r\n")}
	 free(fp);
	 */
	/*

	 // test append
	 if (f_open(fp, "file.txt", FA_WRITE | FA_OPEN_ALWAYS) == FR_OK) {	// Open existing or create new file
	 if (f_lseek(fp, f_size(fp)) == FR_OK)
	 {
	 char *text2 = "This is a new line, appended to existing file!\r\n";
	 f_write(fp, text2, strlen(text2), &bw);	// Write data to the file
	 if (bw == strlen(text2)) { //we wrote the entire string
	 //lcd.send_format_string("Appended:%dB\n", f_size(fp));
	 debug(f_size(fp)); debug("\n");
	 }

	 }
	 f_close(fp);// Close the file
	 }
	 char str[12];


	 // get card volume
	 char szCardLabel[12] = {0};
	 DWORD sn = 0;
	 if (f_getlabel("", szCardLabel, &sn) == FR_OK) {
	 //lcd.send_format_string("%s SN:%X\n", szCardLabel, sn);
	 debug(szCardLabel); debug("\n");
	 debug(sn); debug("\n");
	 }
	 */
	SEE_FREE_MEM();

	if(1)

		// read from file
		fp = (FIL *) malloc(sizeof(FIL));
	res = f_open(fp, "pj-kk.txt", FA_WRITE | FA_CREATE_ALWAYS);
	if (res == FR_OK)
	{	// Create a file
		debug(F("file opened to read.., res: "));
		debug(res);
		debug("\r\n");
		SEE_FREE_MEM();

		UINT br;
/*
		debug(F("File size: "));
		debug(f_size(fp));
		debug("\n\r");
		SEE_FREE_MEM();

		char text[255];
		memset(text, 0, 255);
		debug(F("Reading res: "));
		debug(f_read(fp, text, 255, &br));
		debug("\r\n");
		SEE_FREE_MEM();

		debug(F("Byte readed: "));
		debug(br);
		debug("\r\n");
		SEE_FREE_MEM();

		debug(F("Text: "));
		debug(text);
		debug("\r\n");
		SEE_FREE_MEM();

		debug(F("lseek res: "));
		debug(f_lseek(fp, f_size(fp)));
		debug("\r\n");
		SEE_FREE_MEM();*/

		debug(F("write res: "));
		for(int i = 0; i < 100; i++)
		{
			char buf[1000];
			sprintf(buf, "Its my striiiiing --- S.D.!: %d\r\n", i);
			f_write(fp, buf, strlen(buf), &br);
		}
		debug("\r\n");
		SEE_FREE_MEM();

		debug(F("Byte writen: "));
		debug(br);
		debug("\r\n");
		SEE_FREE_MEM();

		f_close(fp);	// Close the file
		SEE_FREE_MEM();
	}
	else
	{
		debug("file not open to read: ");
		debug(res);
		debug("\r\n")
	}
	free(fp);

	while (1)
	{
		//SEE_FREE_MEM();
		_delay_ms(500);
		/*
		 debug(F("Free RAM: "));
		 debug(freeRam());
		 debug("\n");*/

	};
}
