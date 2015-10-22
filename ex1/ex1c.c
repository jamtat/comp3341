#include <lpc24xx.h>

#define BUTTON_UP 10
#define BUTTON_DOWN 11
#define BUTTON_LEFT 12
#define BUTTON_RIGHT 13
#define BUTTON_CENTRE 22

void handleInputs(void);
void updateDisplay(void);

void wait ( unsigned int ticks );
inline unsigned long toggleBit ( unsigned long number, unsigned int toggleBit );
inline unsigned long setBitOn ( unsigned long number, unsigned int onBit );
inline unsigned long setBitOff ( unsigned long number, unsigned int offBit );
inline unsigned int isBitOn ( unsigned long number, unsigned int testBit );
void setPortPinValue ( unsigned int port, unsigned int pin, int value );
unsigned int getPortPinValue ( unsigned int port, unsigned int pin );
inline unsigned int buttonPressed( int button );
void enableLEDs(void);
void turnOffAllLEDs(void);
void turnOnAllLEDs(void);
void setLED ( unsigned int led, int value );

static unsigned long PROGRAM_TICKS = 0;

static unsigned int STATE_LED_SELECTED = 15;

static unsigned int STATE_BLEEP_BLOOP_TICKS = 1000000/2;

static unsigned long STATE_LED_MASK = 0x00103002;

static unsigned int STATE_SETMODE = 1;

int main ( void ) {
	
	enableLEDs();
	turnOffAllLEDs();
	
	while(1) {
		updateDisplay();
		PROGRAM_TICKS += 1;
	}
	return 0;
};

void handleInputs() {
	
	if ( buttonPressed( BUTTON_RIGHT ) ) {
		STATE_LED_SELECTED = (STATE_LED_SELECTED - 1)%32;
	}
	
	if ( buttonPressed( BUTTON_LEFT ) ) {
		STATE_LED_SELECTED = (STATE_LED_SELECTED + 1)%32;
	}
	
	if ( buttonPressed( BUTTON_CENTRE ) ) {
		STATE_SETMODE = STATE_SETMODE ? 0 : 1;
	}
	
	if ( buttonPressed( BUTTON_UP ) || buttonPressed( BUTTON_DOWN ) ) {
		STATE_LED_MASK = toggleBit( STATE_LED_MASK, STATE_LED_SELECTED );
	}
};

void updateDisplay() {
	
	int i;
	
	for ( i = 0; i < 32; i++ ) {
		setLED( i, isBitOn( STATE_LED_MASK, i ) );
	}
	
	if ( STATE_SETMODE ) {
		setLED( STATE_LED_SELECTED, 1 );
	}
	
	handleInputs();
	
	wait( STATE_BLEEP_BLOOP_TICKS );
	turnOffAllLEDs();
	
	if ( STATE_SETMODE ) {
		setLED( STATE_LED_SELECTED, 1 );
	}
	
	handleInputs();
	
	wait( STATE_BLEEP_BLOOP_TICKS );
};



void wait ( unsigned int ticks ) {
	volatile int i;
	 
	for ( i = 0; i < ticks; i++) {
		//Do nothing
	}
};

inline unsigned long toggleBit ( unsigned long number, unsigned int toggleBit ) {
	
	unsigned long mask = (1<<toggleBit);
	
	return number ^ mask;
};

inline unsigned long setBitOn ( unsigned long number, unsigned int onBit ) {
	
	unsigned long mask = (1<<onBit);
	
	return number | mask;
}

inline unsigned long setBitOff ( unsigned long number, unsigned int offBit ) {
	
	unsigned long mask = ~(1<<offBit);
	
	return number & mask;
}

inline unsigned int isBitOn ( unsigned long number, unsigned int testBit ) {
	
	int mask = (1<<testBit);
	
	return (number & mask) > 0;
};

inline void enableLEDs() {
	FIO3DIR = 0xFFFFFFFF;
};

inline void turnOffAllLEDs() {
	FIO3CLR = 0xFFFFFFFF;
};

inline void turnOnAllLEDs() {
	FIO3SET = 0xFFFFFFFF;
};
 
void setPortPinValue ( unsigned int port, unsigned int pin, int value ) {
	
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

unsigned int getPortPinValue ( unsigned int port, unsigned int pin ) {
	
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

inline unsigned int buttonPressed( int button ) {
	return getPortPinValue( 0, button );
};

inline void setLED ( unsigned int led, int value ) {
	setPortPinValue( 3, led, value );
};