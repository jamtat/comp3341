#include "ex2.h"

int main ( void ) {
	
	int lastRevs;
	int revsPerSecond;
	
	EnableMotor();
	
	EnableDisplay();
	
	SetPulseWidth( 2000 );
	
	// Set up state vars
	
	// Draw interface
	
	EnableRevCounter();
	
	while (1) {	
		
		lastRevs = T1TC;
		
		wait( 1000 );
		
		revsPerSecond = (T1TC - lastRevs);
		
		textClear();
		
		simplePrintf( "Total revs: %d\n%d revs/sec", T1TC, revsPerSecond );

	}
	
	return 0;
}

// Wait a number of milliseconds using T2
void wait ( unsigned int milliseconds )
{
	int waitCycles = MillisecondsToCycles( milliseconds );
	
	EnableTimer( TIMER_ENABLE_2 );
	
	// Reset the timer to 0
	T2TCR = 2;
	
	// Clear all the T2 interrupt bits
	T2IR |= 0x0F;
	
	// Set the timer target value
	T2MR0 = waitCycles;
	
	// Set the timer to stop and reset when it reaches the count value and allow interrupt
	T2MCR = 3;
	
	// Start the timer
	T2TCR = 1;
	
	// Block execution while interrupt flag is not set
	while ( !IsBitOn( T2IR, 0 ) );
	
}


// Convert milliseconds to cycles
inline unsigned int MillisecondsToCycles( unsigned int milliseconds )
{
	return 1000 * milliseconds * CPU_MHZ;
}


// Convert cycles to milliseconds
inline unsigned int CyclesToMilliseconds( unsigned int cycles )
{
	return ( cycles / CPU_MHZ ) / 1000;
}


// Enable a particular timer (TIMER_X)
void EnableTimer ( int timer ) 
{
	
	// Enable the timer bit in PCONP
	PCONP = SetBitOn( PCONP, timer );
}


// Handle display initialisation
void EnableDisplay ( void )
{
	
	// Call the library's display init function
	textInit();
	
	textClear();
	
}


// Set the correct bits to enable PWM
void EnableMotor ( void )
{
	
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


// Enable the rev counter with T1
void EnableRevCounter ( void )
{
	
	// Enable Timer 1
	EnableTimer( TIMER_ENABLE_1 );
	
	// Enable timer and reset the timer count
	T1TCR = SetBitOn( SetBitOn( T1TCR, 1 ), 0 );
	
	// Set the rev counter input pin to counter mode and stop/reset the timer
	PINSEL3 |= (3<<6);
	
	// Set timer to count pulses on P1.19 and start
	T1CTCR = 5;
	T1TCR = 1;
}


// Set the pulse width for the PWM
void SetPulseWidth ( int pulseWidth )
{
	
	// Clamp the pulse width to the period
	PWM0MR2 = pulseWidth > PULSE_PERIOD ? PULSE_PERIOD : pulseWidth;
	
	// Update PWM0MR2 on next cycle
	PWM0LER = SetBitOn( EMPTY_MASK, 2 );
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