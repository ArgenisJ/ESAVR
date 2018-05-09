#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h> // delays
#include <avr/interrupt.h> //interrupts

//calc baud rate
#define BAUD 9600

// PRE-DECLARED MOTOR VARIABLES
#define motorPosA PORTB5
#define motorNegA PORTB4
#define motorPosB PORTD3
#define motorNegB PORTD4

// PRE-DECLARED LED VARIABLES
#define LEDPORT PORTC
#define LED_PIN_LEFT PORTC4
#define LED_PIN_RIGHT PORTC5
#define LED_DDR DDRC

unsigned int n; // determines if left turn signal is on
unsigned int m; // determines if right turn signal is on
char* str; // string containing chars to print on terminal

// FUNCTION PROTOTYPES
void Brake(); // STOPS BOTH MOTORS
void Forward(); // MOVES CAR FORWARD
void Reverse(); // MOVES CAR BACKWARD
void TurnLeft(); // ROTATES CAR LEFT
void TurnRight(); // ROTATES CAR RIGHT
void LeftSignal(); // TOGGLE LEFT TURN SIGNAL
void RightSignal(); // TOGGLE RIGHT TURN SIGNAL

int main(void)
{
	n = 0; // INITIALIZE LEFT TURN SIGNAL AS OFF
	m = 0; // INITIALIZE RIGHT TURN SIGNAL AS OFF
	DDRB = 0xFF; // FIRST MOTOR
	DDRD = 0xFF; // SECOND MOTOR
	LED_DDR |= (1 << LED_PIN_LEFT) | (1 <<LED_PIN_RIGHT);	// Set LED to output
	LEDPORT &= ~(1 << LED_PIN_LEFT);	// Initial state of OFF
	LEDPORT &= ~(1 << LED_PIN_RIGHT);	// Initial state of OFF
	ICR1 = 2000; // TOP, PWM MODE
	OCR1A = 2000; // 100% duty cycle for left motor, MAX SPEED
	OCR1B = 2000; // 100% duty cycle for right motor, MAX SPEED
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1); // None-inverted mode.

	// UART SETTINGS
	// Set baud rate: UBRR = [F_CPU/(16*BAUD)] - 1
	unsigned int baudrate;
	baudrate = ((F_CPU/16)/BAUD) - 1;
	UBRR0H = (unsigned char) (baudrate >> 8);
	UBRR0L = (unsigned char) baudrate;

	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);		// Enable receiver and transmitter
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);	// Set data frame: 8 data bits, 1 stop bit, no parity
	UCSR0B |= (1 << RXCIE0);					// Enable receiver interrupt
	sei();										// Enable global interrupts
	
	// Fast PWM, ICR1= TOP
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12) | (1 << WGM13);
	
	TCCR1B |= (1 << CS11); // prescaler 8
	
	while (1)
	{
		while(n == 1)
		{
			// toggle left turn signal
			LEDPORT ^= (1 << LED_PIN_LEFT);
			_delay_ms(500); // delay 1/2 sec
		}
		while(m == 1)
		{
			// toggle right turn signal
			LEDPORT ^= (1 << LED_PIN_RIGHT);
			_delay_ms(500); // delay 1/2 sec
		}
	}
}

ISR (USART_RX_vect)
{
	unsigned char receivedChar; // declare char being received from keyboard

	receivedChar = UDR0;	// Read data from the RX buffer
	if(receivedChar == 'w')
	{
		Forward();	// moves car forward
		str = "Forward\n\r"; // prints "Forward" on terminal
	}
	else if(receivedChar == 's')
	{
		Reverse(); // moves car backward
		str = "Backward\n\r"; // prints "Backward" on terminal
	}
	else if(receivedChar == 'a')
	{
		TurnLeft(); // rotates car left
		str = "Left\n\r"; // prints "Left" on terminal
	}
	else if(receivedChar == 'd')
	{
		TurnRight(); // rotates car right
		str = "Right\n\r"; // prints "Right" on terminal
	}
	else if(receivedChar == ' ')
	{
		Brake(); // stops car
		str = "Brake\n\r"; // prints "Brake" on terminal
	}
	else if(receivedChar == 'q')
	{
		LeftSignal(); // activates left turn signal
		str = "LeftSig\n\r"; // prints "LeftSig" on terminal
	}
	else if(receivedChar == 'e')
	{
		RightSignal(); // activates right turn signal
		str = "RightSig\n\r"; // prints "RightSig" on terminal
	}
	
	else if(receivedChar == 'r')
	{
		if(OCR1A < 2000) // if car is not at max speed (TOP)
		{
			OCR1A += 500; // speeds up left motor
			OCR1B += 500; // speeds up right motor
			str = "Speeding up!!!\n\r"; // prints "Speeding up!!!" on terminal
		}
		else // otherwise, doesn't speed up
			str = "Cannot go faster!!\n\r"; // prints "Cannot go faster!!" on terminal
	}
	else if(receivedChar == 'f')
	{
		if(OCR1A > 1000) //if car PWM variables are greater than 1000 (50% duty cycle)
		{
			OCR1A -= 500; // slows down left motor
			OCR1B -= 500; // slows down right motor
			str = "Slowing down...\n\r"; // prints "Slowing down..." on terminal
		}
		else // otherwise, doesn't slow down
			str = "Cannot go slower...\n\r"; // prints "Cannot go slower..." on terminal
	}
	
	else
		str = "Not a command.\n\r"; // prints "Not a command" on terminal
	
	unsigned int i = 0; // keeps track of which char to print
	while(str[i] != 0) // runs until NULL is reached
	{
		while (!(UCSR0A & (1<<UDRE0))); // wait until UART is ready
		UDR0 = str[i];	// Write the data to the TX buffer
		i++; // increment to next char
	}
}

void Brake()
{
	// configuration to stop car
	PORTB |= (1 << motorPosA) | (1 << motorNegA);
	PORTD |= (1 << motorPosB) | (1 << motorNegB);
}

void Forward()
{
	// configuration to move car forward
	PORTB |= (1 << motorPosA);
	PORTB &= ~(1 << motorNegA);
	PORTD |= (1 << motorPosB);
	PORTD &= ~(1 << motorNegB);
}

void Reverse()
{
	// configuration to move car backward
	PORTB |= (1 << motorNegA);
	PORTB &= ~(1 << motorPosA);
	PORTD |= (1 << motorNegB);
	PORTD &= ~(1 << motorPosB);
}

void TurnRight()
{
	// configuration to rotate car right
	PORTB |= (1 << motorNegA);
	PORTB &= ~(1 << motorPosA);
	PORTD |= (1 << motorPosB);
	PORTD &= ~(1 << motorNegB);
}

void TurnLeft()
{
	// configuration to rotate car left
	PORTB |= (1 << motorPosA);
	PORTB &= ~(1 << motorNegA);
	PORTD |= (1 << motorNegB);
	PORTD &= ~(1 << motorPosB);
}

void LeftSignal()
{
	if(n == 0) // if left signal is currently set as off,
	{
		n = 1; // turn on left signal 
		m = 0; // turn on right signal, just in case
		LEDPORT &= ~(1 << LED_PIN_LEFT);	// turn left signal LED off
		LEDPORT &= ~(1 << LED_PIN_RIGHT);	// turn right signal LED off
	}
	else // otherwise,
	{
		n = 0; // turn off left signal
		LEDPORT &= ~(1 << LED_PIN_LEFT);	// turn left signal LED off
		LEDPORT &= ~(1 << LED_PIN_RIGHT);	// turn right signal LED off
	}
}

void RightSignal()
{
	if(m == 0) // if right signal is currently set as off,
	{
		m = 1; // turn on right signal
		n = 0; // turn off left signal, just in case
		LEDPORT &= ~(1 << LED_PIN_LEFT);	// turn left signal LED off
		LEDPORT &= ~(1 << LED_PIN_RIGHT);	// turn right signal LED off
	}
	else // otherwise, 
	{
		m = 0; // turn off right signal
		LEDPORT &= ~(1 << LED_PIN_LEFT);	// turn left signal LED off
		LEDPORT &= ~(1 << LED_PIN_RIGHT);	// turn right signal LED off
	}
}