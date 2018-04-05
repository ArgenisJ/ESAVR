// Argenis Jimenez Aguirre
// CPE 301
// Assignment 3

#include <avr/io.h>
#include <stdint.h> // needed for uint8_t
#include <avr/interrupt.h>
#define FOSC 16000000 // Clock Speed
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD -1

volatile uint8_t ADCvalue; // Global variable, set to volatile if used with ISR

ISR(TIMER1_OVF_vect) {
	TCNT1 = 49911; // Reset starting count
	TIFR1 = (1 << TOV1); // Clear overflow flag
	while(!(UCSR0A & (1 << UDRE0)));// UART
	UDR0 = ADCvalue;// Send value to UDR0
}

ISR(ADC_vect)
{
	ADCvalue = ADCH * 2; // from high bit value, F = Vout * 2 
}

int main( void )
{
	ADMUX = 0; // use ADC0
	ADMUX |= (1 << REFS0); // use AVcc as the reference
	ADMUX |= (1 << ADLAR); // Right adjust for 8 bit resolution
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
	ADCSRA |= (1 << ADATE); // Set ADC Auto Trigger Enable
	ADCSRB = 0; // 0 for free running mode
	ADCSRA |= (1 << ADEN); // Enable the ADC
	ADCSRA |= (1 << ADIE); // Enable Interrupts
	ADCSRA |= (1 << ADSC); // Start the ADC conversion
	
	/*Set baud rate */
	UBRR0H = ((MYUBRR) >> 8);
	UBRR0L = MYUBRR;

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0); // Enable receiver and transmitter
	//	UCSR0B |= (1 << RXCIE0); // Enable reciever interrupt
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Set frame: 8data, 1 stp
	
	TCNT1 = 49911; // Set starting count
	TIMSK1 = (1 << TOIE1); // Enable Timer 1
	TCCR1A = 0; // disable PWM
	TCCR1B = (1 << CS12) | (1 << CS10); // Set prescaler to divide by 1024
	
	sei(); 
	while(1)
	{
		; // Main loop
	}
}

