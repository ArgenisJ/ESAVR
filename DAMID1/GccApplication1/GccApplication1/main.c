// Argenis Jimenez Aguirre
// CPE 301
// Midterm 1

#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h> // needed for uint8_t
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#define FOSC 16000000				// Clock speed
#define BAUD 115200
#define MYUBRR FOSC/8/BAUD-1

// Global variables
volatile uint8_t ADCvalue;
volatile unsigned char temp[5];

// ESP AT Commands
volatile unsigned char AT[] = "AT\r\n"; // Test
volatile unsigned char CIPMUX[] = "AT+CIPMUX=0\r\n"; // Enable connection
volatile unsigned char CIPSTART[] = "AT+CIPSTART=\"TCP\",\"184.106.153.149\",80\r\n"; // Establish connection with ThingSpeak
volatile unsigned char SEND_DATA[] = "GET /update?key=R8838WF4BJJ0RVS2&field1="; // Get Write Key

volatile unsigned char CIPSEND[] = "AT+CIPSEND=45\r\n"; // Send Temperature
volatile unsigned char CWMODE[] = "AT+CWMODE=3\r\n"; // Set Wi-Fi mode
volatile unsigned char CONNECTWIFI[] = "AT+CWJAP=\"CAMP NOU\",\"@neW.olD!sTar*Si\"\r\n"; // Get Wi-Fi info
volatile unsigned char FIRMWARE[] = "AT+GMR\r\n"; // Get AT Firmware info
volatile unsigned char BREAK[] = "\r\n\r\n"; // end of temperature transmission

void usart_init();
void send_AT(volatile unsigned char AT[]);

void usart_init() {
	/*Set baud rate */
	UBRR0H = ((MYUBRR) >> 8);
	UBRR0L = MYUBRR;

	UCSR0A |= (1<< U2X0); // Reduce divisor baud rate to 8
	UCSR0B |= (1 << TXEN0); // Enable USART transmitter
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00); // Set the data to 8 bits
}
// ADC value Interrupt subroutine
ISR(ADC_vect) {
	unsigned char i;
	char tmptemp[5];
	
	ADCvalue = (ADCH << 1) * 1.8 + 32; // Convert Celsius to Fahrenheit
	itoa(ADCvalue, tmptemp, 10); //convert char to ascii
	for(i = 0 ; i < 5 ; i++)
	temp[i] = tmptemp[i]; //move converted ascii
}

void send_AT(volatile unsigned char AT[]) {
	volatile unsigned char a;
	volatile unsigned char ATlength = 0;
	
	while(AT[ATlength] != 0)
	ATlength++; // find length
	
	for(a = 0 ; a < ATlength ; a++) 
	{
		while(!(UCSR0A & (1<<UDRE0)));	// UART
		UDR0 = AT[a];					// send char
	}
}

int main(void)
{
	ADMUX = 0;
	ADMUX |= (1 << REFS0); // use AVcc as the reference
	ADMUX |= (1 << ADLAR); // Right adjust for 8 bit resolution
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 128 prescale for 16Mhz
	ADCSRA |= (1 << ADATE); // Set ADC Auto Trigger Enable
	ADCSRB = 0; // 0 for free running mode
	ADCSRA |= (1 << ADEN); // Enable the ADC
	ADCSRA |= (1 << ADIE); // Enable Interrupts
	ADCSRA |= (1 << ADSC); // Start the ADC conversion
	
	usart_init(); // initialize usart
	
	_delay_ms(1000);
	send_AT(AT); // Test
	_delay_ms(1000);
	send_AT(FIRMWARE); // Get AT firmware info
	_delay_ms(1000);
	send_AT(CWMODE); // Set to WiFi mode
	_delay_ms(1000);
	send_AT(CONNECTWIFI); //connect to Wi-Fi
	_delay_ms(2000);
	send_AT(CIPMUX); // Enable Connection
	
	sei();
	while (1)
	{
		_delay_ms(500);
		send_AT(CIPSTART); // Connect to ThingSpeak
		_delay_ms(500);
		send_AT(CIPSEND); // Enable send
		_delay_ms(500);
		send_AT(SEND_DATA); // Activate Write key to send 
		send_AT(temp); // Temperature
		send_AT(BREAK); // End of transmission
	}
	return 0;
}