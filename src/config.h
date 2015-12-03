/*
 * config.h
 *
 *  Created on: 1 лист. 2015
 *      Author: sd
 */

#ifndef CONFIG_H_
#define CONFIG_H_


/******************** Inet **********************/
#define PWR_PORT PORTH
#define PWR_DDR DDRH
#define PWR_PIN 4
#define NUM_OF_ACK_ATTEMPT 2


/******************* NTC ************************/
#define TIMER_PERIOD 5			//in minutes
#define NTC_REFRESH_PERIOD 24 	//in hours


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


/********** Dallas User Definitions *************/
#define DETECT_ERROR_COUNT 3

#define DALL_PWR_PORT PORTA
#define DALL_PWR_DDR DDRA
#define DALL_PWR_PIN 0


/************ Network Settings ************/
#define NETWORK_TXINT USART2_TX_vect
#define NETWORK_RXINT USART2_RX_vect
#define NETWORK_PORT    UDR2

#define NETWORK_AP "www.kyivstar.net"
#define NTP_ADDR "pool.ntp.org"

#define NETWORK_SERVER_ADDR "termo.big-gps.com"//"avatarsd.com"
#define NETWORK_SERVER_PORT 3050

#define NUM_ATTEMP_TO_COMNNECT 10
/*****************************************/


#endif /* CONFIG_H_ */
