/*
 * uarts_class.h
 *
 * Created: 30.07.2015 16:11:05
 *  Author: sd
 */ 


#ifndef UARTS_CLASS_H_
#define UARTS_CLASS_H_

#include "avr/pgmspace.h"

class __FlashStringHelper;
#define F(string_literal) (reinterpret_cast<const __FlashStringHelper *>(PSTR(string_literal)))

class UART
{
public:
	UART(volatile unsigned char & udr, unsigned long int baud, int txBuffersize, int rxBuffersize);
	virtual ~UART();

	void end();
	void begin();
	void begin(long baud_rate);
	void setBaud(unsigned long int baud);
	void flush(void);// erase rx circular buffer
	void BreakCOM(void);
	int  available();

	char getch();
	char peek();
	void putch(char);
	void print(const char * str);
	void print(const __FlashStringHelper * flashStr);
	void print(long int num);
	void print(int num);
	void print(unsigned long int num);
	void print(unsigned int num);
	void print(double num);
	int  WriteCOM(unsigned int outlen, unsigned char *outbuf);
	int  ReadCOM(unsigned int inlen, unsigned char *inbuf);
	template<class T>void (* operator()(T str)){print(str);return 0;}

	//please, connect it to math interrupt
	void rx_byte_int();
	void tx_byte_int();

private:

	//it call every incoming byte
	virtual void byteRecived(char rxByte);


	volatile unsigned char* _udr;
	volatile unsigned char* _ucsra;
	volatile unsigned char* _ucsrb;
	volatile unsigned char* _ucsrc;
	volatile unsigned char* _ubrr;

	char * tx_buffer;
	char * rx_buffer;

	unsigned int tx_buffer_size;
	unsigned int tx_wr_index;
	unsigned int tx_rd_index;
	volatile unsigned int tx_counter;

	unsigned int rx_buffer_size;
	unsigned int rx_wr_index;
	unsigned int rx_rd_index;
	volatile unsigned int rx_counter;
};


#endif /* UARTS_CLASS_H_ */
