#include <lpc24xx.h>

void runOS(void);

void wait ( int milliseconds );
void setPortPinValue ( int port, int pin, int value );
void enableLEDs(void);
void turnOffAllLEDs(void);
void turnOnAllLEDs(void);
void setLED ( int led, int value );

unsigned long PROGRAM_TICKS = 0;

int main ( void ) {
	
	enableLEDs();
	turnOffAllLEDs();
	
	while(1) {
		runOS();
		PROGRAM_TICKS += 1;
	}
	return 0;
};
 
void runOS() {
	setLED( 16, 1 );
	wait( 1000000 );
	turnOffAllLEDs();
	wait( 1000000 );
}
 
void wait ( int ticks ) {
	volatile int i;
	 
	for ( i = 0; i < ticks; i++) {
		//Do nothing
	}
};

void enableLEDs() {
	FIO3DIR = 0xFFFFFFFF;
};

void turnOffAllLEDs() {
	FIO3CLR = 0xFFFFFFFF;
};

void turnOnAllLEDs() {
	FIO3SET = 0xFFFFFFFF;
};
 
void setPortPinValue ( int port, int pin, int value ) {
	
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

void setLED ( int led, int value ) {
	setPortPinValue( 3, led, value );
};