#include "ex3.h"

typedef struct recording {
	int samples[RECORDING_LENGTH*RECORDING_RATE];
	int length; // Length in samples
} Recording;

unsigned short STATE_volume = VOLUME_DEFAULT;
unsigned int STATE_selectedRecording = 0;

int main ( void )
{
	
	EnableDisplay();
	
	EnableADC();
	EnableDAC();
	
	InitUI();
	
	// Pipe input straight to output
	while(1) {
		unsigned int micInput = GetADCReading();
		
		SetDACOutput( micInput );
	}
	return 0;
}


void InitUI ( void )
{
	//Start with black fill
	lcd_fillScreen( BLACK );
	
	int y = 0;
	
	while ( y < DISPLAY_HEIGHT/2 ) {
		
		lcd_fillRect( 0, DISPLAY_HEIGHT/2 - y, DISPLAY_WIDTH, DISPLAY_HEIGHT/2+y, UI_BG );
		y += 5;
	}
	
	lcd_fillScreen( UI_BG );
	lcd_fontColor( UI_TEXT, UI_BG );
	
	DrawHeader();
	DrawRecordingList();
};


void DrawHeader ( void )
{
	lcd_fontColor( UI_BG, UI_C1 );
	lcd_fillRect( 0, 0, DISPLAY_WIDTH, UI_HEADER_HEIGHT, UI_C1 );
	lcd_putString( 8, 3, "chqx69               RTC Assignment 2" );
	lcd_fontColor( UI_TEXT, UI_BG );
}


// Draw the list of recordings
inline void DrawRecordingList()
{
	int i;
	int startY = UI_HEADER_HEIGHT*2;
	int rowY;
	char buffer[11] = "Recording 0";
	
	for( i = 0; i < NUM_RECORDINGS; i++ ) {
		
		rowY = startY + i*UI_ROW_HEIGHT;
		
		// Convert the number to a string
		itoa( i+1, &buffer[10], 10 );
		
		if ( i != STATE_selectedRecording ) {
			
			lcd_fillRect( 
				UI_ROW_PADDING,
				rowY,
				DISPLAY_WIDTH - UI_ROW_PADDING*2,
				rowY + UI_ROW_HEIGHT,
				even(i)?UI_C1:UI_C2
			);
			
			lcd_fontColor( UI_BG, even(i)?UI_C1:UI_C2 );
			
		} else {
			
			//Clear button area
			lcd_fillRect( 
				UI_ROW_PADDING,
				rowY,
				DISPLAY_WIDTH - UI_ROW_PADDING*2,
				rowY + UI_ROW_HEIGHT,
				UI_BG
			);
			
			// Draw highlighted option as a border
			lcd_drawRect( 
				UI_ROW_PADDING,
				rowY,
				DISPLAY_WIDTH - UI_ROW_PADDING*2,
				rowY + UI_ROW_HEIGHT,
				even(i)?UI_C1:UI_C2
			);
			
			lcd_fontColor( even(i)?UI_C1:UI_C2, UI_BG );

		}
		
		lcd_putString( 
			UI_ROW_PADDING*2,
			rowY + ( UI_ROW_HEIGHT/2 - 3 ),
			buffer
		);
		
	}
	
	lcd_fontColor( UI_TEXT, UI_BG );
}


// Turn on the ADC
void EnableADC ( void )
{
	
	// Set the ADC to be an input
	PINSEL1 = SetBitOff( SetBitOn( PINSEL1, 16 ), 17 );
	// Enable the ADC in the Peripheral Control Register
	PCONP = SetBitOn( PCONP, 12 );
}


// Instruct the ADC to take a new reading
inline void TakeADCReading ( void )
{
	//          SEL                CLKDIV              START  P
	AD0CR = (0b00000010) | ((0b00000000 | 25) << 8) | (0b00100100000 << 16);
}


// Block and return a voltage reading when done
inline unsigned int GetADCReading ( void )
{
	
	TakeADCReading();
	
	unsigned long hasNewValueMask = (1 << 31);
	
	// Wait until the ADC yields a new value
	while( !(hasNewValueMask & AD0DR1) );
	
	// Extract the voltage by shifting then masking
	return (unsigned int)((AD0DR1 >> 6 ) & 0x3FF);
}


// Turn on the DAC
void EnableDAC ( void )
{
	
	// Set the DAC to be an output
	PINSEL1 = SetBitOn( SetBitOff( PINSEL1, 20 ), 21 );
}

inline void SetDACOutput( unsigned int voltage )
{
	DACR = (voltage << 6);
}


// Enable GPIO interrupts to handle joystick presses
inline void SetupButtonHandlers()
{
	// Enable GPIO interrupt to VIC
	VICIntSelect &= ~(1<<17);
	// Setup button interrupt handler
	VICVectAddr17 = (unsigned int)OnButtonPress;
	// Enable the handler
	VICIntEnable |= (1<<17);
	// Set to interrupt on rising edge
	IO0_INT_EN_R |= (1<<BUTTON_UP);
	IO0_INT_EN_R |= (1<<BUTTON_DOWN);
	IO0_INT_EN_R |= (1<<BUTTON_LEFT);
	IO0_INT_EN_R |= (1<<BUTTON_RIGHT);
	IO0_INT_EN_R |= (1<<BUTTON_CENTRE);
	// Clear EINT3
	EXTINT = (1<<3);
}


// Handle pressing of the joystick buttons
void OnButtonPress()
{	
	if ( IO0_INT_STAT_R & (1<<BUTTON_CENTRE) ) {
		
	}
	
	// Clear the interrupt bits and ignore system pin interrupts
	EXTINT = (1<<3);
	IO0_INT_CLR |= (1<<BUTTON_UP);
	IO0_INT_CLR |= (1<<BUTTON_DOWN);
	IO0_INT_CLR |= (1<<BUTTON_LEFT);
	IO0_INT_CLR |= (1<<BUTTON_RIGHT);
	IO0_INT_CLR |= (1<<BUTTON_CENTRE);
	VICVectAddr = 0;

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
	
	// Set the timer to stop and reset when it reaches the count value
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
	
	lcd_init();
	
	// Call the library's display init function
	textInit();
	
	textClear();
	
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