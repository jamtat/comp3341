#include "ex2.h"

int STATE_selectableSpeeds[NUM_SELECTABLE_SPEEDS];
int STATE_selectedSpeed = 0;
int STATE_lastRevs;
float STATE_revsPerSecond;
int STATE_test = 0;
int STATE_pulseWidth;
int STATE_showHint = 1;
int STATE_DEBUG = DEBUG;
int UI_listOffset = DISPLAY_HEIGHT/2;

// Pre compute this float to save doing division during control loop
float measureIntervalSecs = ((float)MEASURE_INTERVAL)/1000.0;

int main ( void ) {
	
	
	
	int i;
	// Create a list of selectable speeds
	for ( i = 0; i < NUM_SELECTABLE_SPEEDS; i++ ) {
		STATE_selectableSpeeds[i] = i*(MAX_SPEED - MIN_SPEED)/(NUM_SELECTABLE_SPEEDS-1)+MIN_SPEED;
	}
	
	EnableMotor();
	
	EnableDisplay();
	
	SetPulseWidth( STATE_selectableSpeeds[0] );
	
	EnableRevCounter();
	
	SetupButtonHandlers();
	
	InitUI();
	
	while (1) {	
	
		MeasureSpeed();
		
		DrawRevs();
		
		ControlSpeed();
		
	}
	
	return 0;
}


// Take a measurement in revs/sec of the current speed
void MeasureSpeed()
{
	
	// Save number of revolutions since last measure
	STATE_lastRevs = T1TC;
	
	wait( MEASURE_INTERVAL );
	
	// Calculate revs per second based on revs since last measure
	STATE_revsPerSecond = ((float)( T1TC - STATE_lastRevs )) / (measureIntervalSecs);

}


// Use proportional control to attempt to reach the desired motor speed
void ControlSpeed() 
{
	
	int diff = STATE_selectableSpeeds[STATE_selectedSpeed] - STATE_revsPerSecond;

	// scale the gain factor based on the magnitude of the difference
	int gainFactor = abs(diff);
	
	int nextPulseWidth = gainFactor * diff + STATE_pulseWidth;
	
	if ( STATE_DEBUG ) {
		DrawDebug( diff, gainFactor, nextPulseWidth, STATE_pulseWidth );
	}
	
	SetPulseWidth( nextPulseWidth );
	
}


// Refresh the UI
void InitUI()
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
	DrawRevs();
	DrawDesiredSpeed();
	
	while ( UI_listOffset > 0 ) {
		
		DrawOptions();
		UI_listOffset = max( UI_listOffset - 5, 0 );
		
	}
	
	DrawOptions();
	
	DrawHeader();
	
}


//Draw the assignment header
inline void DrawHeader()
{
	lcd_fontColor( UI_BG, UI_C1 );
	lcd_fillRect( 0, 0, DISPLAY_WIDTH, 13, UI_C1 );
	lcd_putString( 8, 3, "chqx69               RTC Assignment 1" );
	lcd_fontColor( UI_TEXT, UI_BG );
}


// Draw the options for motor speeds
inline void DrawOptions()
{
	int i;
	int startY = UI_listOffset + DISPLAY_HEIGHT - NUM_SELECTABLE_SPEEDS * UI_ROW_HEIGHT;
	int rowY;
	int fontX;
	char buffer[5];
	
	// Clear the hint
	lcd_fillRect(
		DISPLAY_WIDTH/2 - 101,
		startY - 15,
		DISPLAY_WIDTH,
		startY - 1,
		UI_BG
	);
	
	if ( STATE_showHint ) {
		lcd_putString( DISPLAY_WIDTH/2 - 101, startY - 15, "Use the joystick to select a speed" );
	}
	
	for( i = 0; i < NUM_SELECTABLE_SPEEDS; i++ ) {
		
		// Centre the text
		fontX = DISPLAY_WIDTH/2 - (STATE_selectableSpeeds[i] < 100 ? 6 : 10);
		
		rowY = startY + i*UI_ROW_HEIGHT;
		
		// Convert the number to a string
		itoa( STATE_selectableSpeeds[i], buffer, 10 );
		
		if ( i != STATE_selectedSpeed ) {
			
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
			fontX,
			rowY + ( UI_ROW_HEIGHT/2 - 3 ),
			buffer
		);
		
	}
	
	lcd_fontColor( UI_TEXT, UI_BG );
}


// Draw current speed
inline void DrawRevs()
{
	textSetCursor( 0, 1 );	
	simplePrintf( "Current speed: %d revs/sec    ", (int)STATE_revsPerSecond );
}


// Draw current selected pulse width/speed
inline void DrawDesiredSpeed()
{
	textSetCursor( 0, 3 );
	simplePrintf( "Desired speed: %d revs/sec    ", STATE_selectableSpeeds[STATE_selectedSpeed] );
}


// Draw debug information
inline void DrawDebug( int diff, int gainFactor, int nextPulseWidth, int pulseWidth )
{
	textSetCursor( 0, 5 );
	simplePrintf( "Diff: %d       \n", diff );
	simplePrintf( "Gain Factor: %d      \n", gainFactor );
	simplePrintf( "Current PW: %d      \n", pulseWidth );
	simplePrintf( "Diff PW: %d      \n", nextPulseWidth - pulseWidth );
	simplePrintf( "Next PW: %d      \n", nextPulseWidth );
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
	IO0_INT_EN_R |= (1<<BUTTON_CENTRE);
	// Clear EINT3
	EXTINT = (1<<3);
}


// Handle pressing of the joystick buttons
void OnButtonPress()
{	
	// Increment desired speed
	if ( IO0_INT_STAT_R & (1<<BUTTON_DOWN) ) {
		STATE_selectedSpeed = min( STATE_selectedSpeed + 1, NUM_SELECTABLE_SPEEDS - 1 );
	}
	
	// Decrement desired speed
	if ( IO0_INT_STAT_R & (1<<BUTTON_UP) ) {
		STATE_selectedSpeed = max( STATE_selectedSpeed - 1, 0 );
	}
	
	// Show extra debug info
	if ( IO0_INT_STAT_R & (1<<BUTTON_CENTRE) ) {
		STATE_DEBUG = 1;
	}
	
	// Clear the interrupt bits and ignore system pin interrupts
	EXTINT = (1<<3);
	IO0_INT_CLR |= (1<<BUTTON_UP);
	IO0_INT_CLR |= (1<<BUTTON_DOWN);
	IO0_INT_CLR |= (1<<BUTTON_CENTRE);
	VICVectAddr = 0;
	
	STATE_showHint = 0;
	
	DrawDesiredSpeed();
	DrawOptions();
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
	STATE_pulseWidth = min( max( pulseWidth, 100 ), PULSE_PERIOD );
	PWM0MR2 = STATE_pulseWidth;
	
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