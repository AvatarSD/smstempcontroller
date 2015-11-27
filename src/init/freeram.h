/*
 * freeram.h
 *
 *  Created on: 20 лист. 2015
 *      Author: sd
 */

#ifndef INIT_FREERAM_H_
#define INIT_FREERAM_H_

int freeRam();

#define SEE_FREE_MEM() {debug(F("Free RAM: "));\
						debug(freeRam());\
						debug(F("bytes\r\n"));}

#endif /* INIT_FREERAM_H_ */
