/*
 * freeram.cpp
 *
 *  Created on: 20 лист. 2015
 *      Author: sd
 */

#include "freeram.h"

int freeRam () {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}
