/*
 * ROM.cpp
 *
 *  Created on: 31 серп. 2015 р.
 *      Author: sd
 */

#include "ROM.h"
#include "stdio.h"

ROM::ROM() {
	for(auto i = 0; i<8; i++)
		ROM_NO[i] = 0x00;
}

unsigned char& ROM::operator [](int i)
{
	if((i>=0)&&(i<8))
		return ROM_NO[i];
	else
	{
		static unsigned char null;
		null = 0;
		return null;
	}
}

bool ROM::isMathFamily(unsigned char family)
{
	return (family == ROM_NO[0]);
}

bool ROM::operator < (const ROM& cmp) const
{
	return (operator &() < &cmp);
}

const unsigned long long int& ROM::operator &() const
{
	return *((unsigned long long int*)ROM_NO);
}

const char * ROM::toString() const
{
	static char buf[17];
	for(auto i = 0; i<8; i++)
		sprintf(&buf[i*2], "%02X", ROM_NO[i]);
	return buf;
}
