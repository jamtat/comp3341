#include <lpc24xx.h>

void wait ( unsigned int ticks );

void EnablePWM ( void );

inline unsigned long ToggleBit ( unsigned long number, unsigned int toggleBit );
inline unsigned long SetBitOn ( unsigned long number, unsigned int onBit );
inline unsigned long SetBitOff ( unsigned long number, unsigned int offBit );
inline unsigned int IsBitOn ( unsigned long number, unsigned int testBit );

void SetPortPinValue ( unsigned int port, unsigned int pin, int value );
unsigned int GetPortPinValue ( unsigned int port, unsigned int pin );
inline unsigned int IsButtonPressed( int button );



int main ( void ) {
	
	EnablePWM();
	
	
	
	return 0;
}


// Crude for loop based wait
void wait ( unsigned int ticks )
{
	volatile int i;
	 
	for ( i = 0; i < ticks; i++);
}

// Set the correct bits to enable PWM
void EnablePWM ( void ) {
	
	// Set bits 6 and 7 in PINSEL2
	PINSEL2 = SetBitOn( SetBitOn( PINSEL2, 6 ), 7 );
	// Set bit 10 in PWM0PCR 
	PWM0PCR = SetBitOn( PWM0PCR, 10 );
	
	// Cycle width in clock cycles
	PWM0MR0 = 6000;
	
	// Instruct to reset counter to previous value
	PWM0MCR = SetBitOn( PWM0MCR, 1 );
	
	// Pulse width in clock cycles
	PWM0MR2 = 3000;
	
	// Enable counting and PWM
	PWM0TCR = 0x00000009;
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
			return (FIO0PIN & mask) == 0;
		case 1:
			return (FIO1PIN & mask) == 0;
		case 2:
			return (FIO2PIN & mask) == 0;
		case 3:
			return (FIO3PIN & mask) == 0;
		case 4:
			return (FIO4PIN & mask) == 0;
		default:
			return 0;
	}
}


// Button helpers
inline unsigned int IsButtonPressed( int button )
{
	return GetPortPinValue( 0, button );
}