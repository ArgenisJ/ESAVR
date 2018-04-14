#define F_CPU 8000000L

#include <avr/io.h>
#include <util/delay.h>

int main(void) {

	DDRB = 0xFF; // Set port B as output

	ADMUX = 0; // Select ADC0 for input
	ADCSRA = 0x87; // ADC enable, Set division factor to 128
	
	TCCR1B = 0x1B; // Set fast PWM mode and prescaler 64
	TCCR1A = 0x82; // Set Non-inverted PWM
	ICR1 = 2500; // Top
	
	while (1)
	{
		ADCSRA |= (1 << ADSC); // ADC start conversion
		while((!ADCSRA) &(1<<ADIF)); // Wait for conversion
		OCR1A = ADC / 3; // Rotate 180 to 180 degrees by reaching potentiometer value
	}
}