/*
 * config.h
 *
 *  Created on: 1 лист. 2015
 *      Author: sd
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include <avr/pgmspace.h>

/************ Debug Settings ********************/
#define DEBUG_TXINT USART3_TX_vect
#define DEBUG_RXINT USART3_RX_vect
#define DEBUG_PORT 	  UDR3
#define DEBUG_SPEED 115200
#define DEBUG_TXBUF 64
#define DEBUG_RXBUF 8

/**************** Level *************************/
#define DEBUG_LEVEL_PORT 	DEBUG_LEVEL_DEBUG
#define DEBUG_LEVEL_LCD 	DEBUG_LEVEL_INFO
#define DEBUG_LEVEL_SDCARD 	DEBUG_LEVEL_DEBUG

//#define LEVEL_DEBUG
//#define LEVEL_INFO // todo template, need to fix later
//#define LEVEL_WARNING
//#define LEVEL_CRITICAL

/********** Dallas User Definitions *************/
#define DALL_TXINT USART1_TX_vect
#define DALL_RXINT USART1_RX_vect
#define DALL_PORT    UDR1

#define DALL_PWR_PORT PORTA
#define DALL_PWR_DDR DDRA
#define DALL_PWR_PIN 0

#define DETECT_ERROR_COUNT 3
#define NUM_OF_READING_ATEMPT 3
#define DEFAULT_SEARCH_ATTEMPTS 10
#define MAX_SEARCH_ATTEMPTS 100

/************ Network Settings ************/
#define NETWORK_TXINT USART2_TX_vect
#define NETWORK_RXINT USART2_RX_vect
#define NETWORK_PORT    UDR2

#define NETWORK_AP "www.kyivstar.net"
#define NTP_ADDR "pool.ntp.org"

//#define NETWORK_SERVER_ADDR "termo.big-gps.com"//"avatarsd.com"
//#define NETWORK_SERVER_PORT 3050

#define NUM_ATTEMP_TO_NTP_COMNNECT 3
#define NUM_OF_FORCEON_ACK_ATTEMPT 2
#define NUM_ATTEMP_TO_SMS_SETUP 2

#define PWR_PORT PORTH
#define PWR_DDR DDRH
#define PWR_PIN 4

/*************** SD card ******************/
// Set DQ as AVR MISO
#define DO_DQ		PB6//PB3
#define DO_INIT()	DDRB  &= ~(1<<DO_DQ) /* Initialize port MMC DO as input */
#define DO			(PINB &	(1<<DO_DQ))	 /* Test for MMC DO ('H':true, 'L':false) */

// Set DQ as AVR MOSI
#define DI_DQ		PB5//PB2
#define DI_INIT()	DDRB  |= (1<<DI_DQ)	 /* Initialize port MMC DI as output */
#define DI_H()		PORTB |= (1<<DI_DQ)	 /* Set MMC DI "high" */
#define DI_L()		PORTB &= ~(1<<DI_DQ) /* Set MMC DI "low" */

// Set DQ as AVR SCK
#define CK_DQ		PB7//PB1
#define CK_INIT()	DDRB  |= (1<<CK_DQ)	 /* Initialize port MMC SCLK as output */
#define CK_H()		PORTB |= (1<<CK_DQ)	 /* Set MMC SCLK "high" */
#define	CK_L()		PORTB &= ~(1<<CK_DQ) /* Set MMC SCLK "low" */

// Use a pin for CS
#define CS_DQ		PB4//PB0
#define CS_INIT()	DDRB  |= (1<<CS_DQ)	/* Initialize port MMC CS as output */
#define	CS_H()		PORTB |= (1<<CS_DQ)	/* Set MMC CS "high" */
#define CS_L()		PORTB &= ~(1<<CS_DQ)	/* Set MMC CS "low" */

/*********** HARDWARE Settings ************/
#define VOLTAGE_MULTIPLY_RATIO 7.25
#define VOLTAGE_ANALOG_PIN 0

#define POWER_PIN_NUM 1
#define POWER_PIN PINF
#define POWER_DDR DDRF
#define POWER_PORT PORTF

#define CASE_PIN_NUM 2
#define CASE_PIN PINF
#define CASE_DDR DDRF
#define CASE_PORT PORTF

#define LED_ON //PORTB |= (1<<PORTB7) //now we no have LED indicator
#define LED_OFF //PORTB &=~ (1<<PORTB7)
#define LED_TRN //PORTB ^= (1<<PORTB7)

/************* MainBuf Size ***************/
#define ROM_MAINBUFF_SIZE 32 //max sensor size
#define RULENODE_BUFF_SIZE 64

/***************** Alarm ******************/
#define ALARM_PERID 5 // in minutes

/**************** WachDog *****************/
#define WDT_SEC 30

/***************** Help *******************/
const char help[] PROGMEM =
{ 		"Usage:\r\n"
		"search\r\n"
		"status\r\r"
		"setnode <num>,<min>,<max>,<phone>  "
		"where <num> is number of sensor in \"status\" or \"search\" command output, "
		"<min> and <max> is temperature range, <phone> is not required parameter\r\n"
		"shownode <num_or_phone_or_all_or_my>  where <num> is similar to \"setnode\"\r\n"
		"deletenode <num_or_all>  where <num> is node number in \"shownode\" output\r\n"
		"disablenode <num>  where <num> is similar to last\r\n"
		"enablenode <num>"
};



#endif /* CONFIG_H_ */
