// RIGHT IN THE MOTOR LAD SON R U READY TO SMELL WHAT THE ROCK IS COOKING?

#include <lpc24xx.h>

void wait ( unsigned int ticks );
inline unsigned long ToggleBit ( unsigned long number, unsigned int toggleBit );
inline unsigned long SetBitOn ( unsigned long number, unsigned int onBit );
inline unsigned long SetBitOff ( unsigned long number, unsigned int offBit );
inline unsigned int IsBitOn ( unsigned long number, unsigned int testBit );
void SetPortPinValue ( unsigned int port, unsigned int pin, int value );
unsigned int GetPortPinValue ( unsigned int port, unsigned int pin );
inline unsigned int IsButtonPressed( int button );



void wait ( unsigned int ticks )
{
	volatile int i;
	 
	for ( i = 0; i < ticks; i++);
};


// Bit manipulation methods
// Make use of inline functions for increased readability over macros
inline unsigned long ToggleBit ( unsigned long number, unsigned int toggleBit )
{
	
	unsigned long mask = (1<<toggleBit);
	
	return number ^ mask;
};


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
};


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
};


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
};


// Button helpers
inline unsigned int IsButtonPressed( int button )
{
	return GetPortPinValue( 0, button );
};