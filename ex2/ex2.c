#include "ex2.h"

int main ( void ) {
	
	int pulseWidth;
	
	EnableMotor();
	
	EnableDisplay();
	
	while ( 1 ) {
		
		for ( pulseWidth = 1; pulseWidth < PULSE_PERIOD; pulseWidth += 100 ) {
			
			wait( 100000 );
			
			SetPulseWidth( pulseWidth );
			
		}
		
		for ( pulseWidth = pulseWidth; pulseWidth > 100; pulseWidth -= 100 ) {
			
			wait( 100000 );
			
			SetPulseWidth( pulseWidth );
			
		}
		
	}
	
	return 0;
}


// Crude for loop based wait
void wait ( unsigned int ticks )
{
	volatile int i;
	 
	for ( i = 0; i < ticks; i++);
}


// Handle display initialisation
void EnableDisplay ( void ) {
	
	// Call the library's display init function
	textInit();
	
	textClear();
	
}

// Set the correct bits to enable PWM
void EnableMotor ( void ) {
	
	// Set bits 6 and 7 in PINSEL2
	PINSEL2 = SetBitOn( SetBitOn( PINSEL2, 6 ), 7 );
	// Set bit 10 in PWM0PCR 
	PWM0PCR = SetBitOn( PWM0PCR, 10 );
	
	// Pulse period in clock cycles
	PWM0MR0 = PULSE_PERIOD;
	
	// Pulse width in clock cycles
	// Set to low enough that it has no effect (appears off)
	PWM0MR2 = 10;
	
	// Instruct to reset counter to previous value
	PWM0MCR = SetBitOn( PWM0MCR, 1 );
	
	// Enable counting and PWM
	PWM0TCR = 0x00000009;
	
}


// Set the pulse width for the PWM
void SetPulseWidth ( int pulseWidth ) {
	
	// Clamp the pulse width to the period
	PWM0MR2 = pulseWidth > PULSE_PERIOD ? PULSE_PERIOD : pulseWidth;
	
	// Update PWM0MR2 on next cycle
	PWM0LER = SetBitOn( EMPTY_MASK, 2 );
	
	textClear();
	simplePrintf( "%d", pulseWidth );
}


// Bit manipulation methods
// Make use of inline functions for increased readability over macros
inline unsigned long ToggleBit ( unsigned long number, unsigned int toggleBit )
{
	
	unsigned long mask = (1<<toggleBit);
	
	return number ^ mask;
}


inline unsigned long SetBitOn ( unsigned long number, unsigned int onBit )
{
	
	unsigned long mask = (1<<onBit);
	
	return number | mask;
}


inline unsigned long SetBitOff ( unsigned long number, unsigned int offBit )
{
	
	unsigned long mask = ~(1<<offBit);
	
	return number & mask;
}


inline unsigned int IsBitOn ( unsigned long number, unsigned int testBit )
{
	
	int mask = (1<<testBit);
	
	return (number & mask) > 0;
}


// Port pin setters and getters
void SetPortPinValue ( unsigned int port, unsigned int pin, int value )
{
	
	int mask = (1<<pin);
	
	if ( value == 1 ) {
		
		switch ( port ) {
			case 0:
				FIO0SET = mask;
				break;
			case 1:
				FIO1SET = mask;
				break;
			case 2:
				FIO2SET = mask;
				break;
			case 3:
				FIO3SET = mask;
				break;
			case 4:
				FIO4SET = mask;
				break;
		}
		
	} else {
		
		switch ( port ) {
			case 0:
				FIO0CLR = mask;
				break;
			case 1:
				FIO1CLR = mask;
				break;
			case 2:
				FIO2CLR = mask;
				break;
			case 3:
				FIO3CLR = mask;
				break;
			case 4:
				FIO4CLR = mask;
				break;
		}
		
	}
}


unsigned int GetPortPinValue ( unsigned int port, unsigned int pin )
{
	int mask = (1<<pin);
		
	switch ( port ) {
		case 0:
			return (FIO0PIN & mask) > 0;
		case 1:
			return (FIO1PIN & mask) > 0;
		case 2:
			return (FIO2PIN & mask) > 0;
		case 3:
			return (FIO3PIN & mask) > 0;
		case 4:
			return (FIO4PIN & mask) > 0;
		default:
			return 0;
	}
}


// Button helpers
inline unsigned int IsButtonPressed( int button )
{
	return !GetPortPinValue( 0, button );
}