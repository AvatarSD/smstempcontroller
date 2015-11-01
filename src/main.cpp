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
#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "LiquidCrystal_I2C/lcdpcf8574/lcdpcf8574.h"
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
	debug(F("---------INT---------\r\n"));
	static int ntcRefrwshPeriodCount = 0;
	if(!ntcRefrwshPeriodCount)
		_network->refreshTime();
	if(++ntcRefrwshPeriodCount == (NTC_REFRESH_PERIOD*60/TIMER_PERIOD))
		ntcRefrwshPeriodCount = 0;
	while(!_network->sendTemp());
	debug(F("-------INT END-------\r\n\r\n"));
	LED_OFF;
}


ISR(TIMER1_CAPT_vect)
{
	static bool mutex = false;
	static unsigned int breakCounter = 0;

#ifdef LEVEL_DATA
	debug(F("[DATA]: ")); debug(F("breakCounter is: ")); debug(breakCounter);
	debug(F("   ")); debug(F("Mutex is: ")); debug(mutex); debug(F("\r\n"));
#endif

	if((!breakCounter)&&(!mutex))
	{
		mutex = true;
		++breakCounter;
//		mainLoop();
		mutex = false;
	}
	else if((!breakCounter)&&(mutex))
	{

	}
	else
	{
		++breakCounter;
	}
	if(breakCounter == TIMER_PERIOD*60)
		breakCounter = 0;
}



int main(void)
{
	init();

	DallasOneWire iface(UDR1);
	_iface = &iface;

	DallasTemp  sensors(iface);

	HardwareData HWdata;

	NetworkWorker network(sensors, HWdata);
	_network = &network;

	for(auto i = 0; i<5; i++)
		debug(F("\r\n"));
	debug(F("---------Hello---------\r\n"));

	iface.DS2480B_Detect();

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
	TCCR1A=(0<<COM1A1) | (0<<COM1A0) | (0<<COM1B1) | (0<<COM1B0) | (0<<COM1C1) | (0<<COM1C0) | (0<<WGM11) | (0<<WGM10);
	TCCR1B=(0<<ICNC1) | (0<<ICES1) | (1<<WGM13) | (1<<WGM12) | (1<<CS12) | (0<<CS11) | (0<<CS10);
	TCNT1H=0x00;
	TCNT1L=0x00;
	ICR1H=0xF4;
	ICR1L=0x23;

	// Timer/Counter 1 Interrupt(s) initialization
	TIMSK1=(1<<ICIE1) | (0<<OCIE1C) | (0<<OCIE1B) | (0<<OCIE1A) | (0<<TOIE1);

	sei();

	delay(2000);
	//init lcd
	lcd_init(LCD_DISP_ON_CURSOR);
	//lcd go home
	//lcd_home();
	bool led = 0;

	while(1)
	{

		delay(1000);
		//		[](){
		/*for(int i = 0; (i < 5); i++)
			debug(F("\n\r"));

		debug(F("NetworkWorker: "));
		debug(sizeof(NetworkWorker));
		debug(F("\n\r"));
		debug(F("SIMCOM900: "));
		debug(sizeof(SIMCOM900));
		debug(F("\n\r"));
		debug(F("InetGSM: "));
		debug(sizeof(InetGSM));
		debug(F("\n\r"));
		debug(F("GSM: "));
		debug(sizeof(GSM));
		debug(F("\n\r"));
		debug(F("UART: "));
		debug(sizeof(UART));
		debug(F("\n\r"));
		debug(F("DallasTemp: "));
		debug(sizeof(DallasTemp));
		debug(F("\n\r"));
		debug(F("DallasOneWire: "));
		debug(sizeof(DallasOneWire));
		debug(F("\n\r"));
		debug(F("\n\r"));*/
		//
		//		for(int i = FIRST_ADC_INPUT; i <= LAST_ADC_INPUT; i++)
		//		{
		//			debug(i);
		//			debug(": ");
		//			debug(analog.dataAt(i));
		//			debug("\n\r");
		//		}
		//		};

		//LED_TRN;




		lcd_led(led); //set led
		led = !led; //invert led for next loop

		//test loop
		int i = 0;
		int line = 0;
		for(i=0; i<60; i++)
		{
			char buf[10];
			itoa(i, buf, 10);
			///lcd_gotoxy(i, line);
			//lcd_puts("i= ");
			//lcd_gotoxy(3, line);
			lcd_puts(buf);
			//line++;
			//line %= 2;
			delay(200);
		}

	};
}
