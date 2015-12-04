/*
 * ADC.cpp
 *
 *  Created on: 28 вер. 2015 р.
 *      Author: sd
 */

#include "Analog.h"
#include "avr/io.h"
#include "avr/interrupt.h"
#include "util/delay.h"


Analog analog;

ISR(ADC_vect)
{
	analog.ADCint();
}




Analog::Analog()
{
	begin();
}

void Analog::begin()
{
	// ADC initialization
	// ADC Clock frequency: 125,000 kHz
	// ADC Voltage Reference: 2.56V, cap. on AREF
	// ADC Auto Trigger Source: Free Running
	ADMUX=(FIRST_ADC_INPUT & 0x7) | ADC_VREF_TYPE;
	ADCSRA=(1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (0<<ADIF) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
	ADCSRB=(((FIRST_ADC_INPUT & 0x8)!=0)<<MUX5) | (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);

	// Digital input buffers on ADC0: On, ADC1: On, ADC2: On, ADC3: On
	// ADC4: On, ADC5: On, ADC6: On, ADC7: On
	DIDR0 = (0<<ADC7D) | (0<<ADC6D) | (0<<ADC5D) | (0<<ADC4D) | (0<<ADC3D) | (0<<ADC2D) | (0<<ADC1D) | (0<<ADC0D);

	// Digital input buffers on ADC8: Off, ADC9: Off, ADC10: Off, ADC11: Off
	// ADC12: Off, ADC13: Off, ADC14: Off, ADC15: Off
	DIDR2 =( 0<<ADC15D) | (0<<ADC14D) | (0<<ADC13D) | (0<<ADC12D) | (0<<ADC11D) | (0<<ADC10D) | (0<<ADC9D) | (0<<ADC8D);

}

// ADC interrupt service routine
// with auto input scanning
void Analog::ADCint()
{
	//todo repair reading
	static unsigned char input_index=0;
	unsigned char wr_index = input_index-1;
	if (wr_index < 0)
		wr_index=LAST_ADC_INPUT-FIRST_ADC_INPUT;

	// Read the AD conversion result
	adc_data[input_index]=ADCW;
	// Select next ADC input
	if (++input_index > (LAST_ADC_INPUT-FIRST_ADC_INPUT))
		input_index=0;
	ADMUX=((FIRST_ADC_INPUT+input_index) & 0x7) | ADC_VREF_TYPE;
	if ((FIRST_ADC_INPUT+input_index) & 0x08) ADCSRB|=(1<<MUX5);
	else ADCSRB&=~(1<<MUX5);
	// Delay needed for the stabilization of the ADC input voltage
	_delay_us(10);
	// Start the AD conversion
	ADCSRA|=(1<<ADSC);
}

unsigned int Analog::dataAt(char pin)
{
	if((pin >= FIRST_ADC_INPUT)&&(pin <= LAST_ADC_INPUT))
		return adc_data[pin - FIRST_ADC_INPUT];
	else return 0;
}

unsigned int Analog::operator[](char pin)
{
	return dataAt(pin);
}

