/*
 * ROM.h
 *
 *  Created on: 31 серп. 2015 р.
 *      Author: sd
 */

#ifndef DALLASONEWIRE_ROM_H_
#define DALLASONEWIRE_ROM_H_

class ROM {
public:
	ROM();

	bool isMathFamily(unsigned char family);
	unsigned char& operator [](int i);
	bool operator < (const ROM& cmp) const;
	const unsigned long long int& operator &() const;
	const char* toString() const;
private:
	unsigned char ROM_NO[8];
};

#endif /* DALLASONEWIRE_ROM_H_ */
