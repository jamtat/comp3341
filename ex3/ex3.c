#include "ex3.h"

// State used on all screens
Recording STATE_recordings[NUM_RECORDINGS];
int STATE_selectedRecording = 0;
ScreenState STATE_screen = HOME;
unsigned int MAX_RECORDING_LENGTH = RECORDING_LENGTH*RECORDING_RATE;
unsigned int WAVE_REDRAW_INTERVAL = (RECORDING_LENGTH*RECORDING_RATE)/DISPLAY_WIDTH;

// Home screen state
enum homeActions {PLAY, RECORD};
enum homeActions STATE_selectedActionHome = RECORD;

// Recording screen state
int STATE_recordingInProgress = 0;
int STATE_recordingInterrupted = 0;
enum recordingActions {RECORD_CLEAR, RECORD_RECORD};
enum recordingActions STATE_selectedActionRecording = RECORD_RECORD;

// Playback screen state
int STATE_playbackInProgress = 0;
float STATE_playbackSpeed = 1.0;
int STATE_playbackPosition = 0;
int STATE_playbackWaitCycles = 250;
int STATE_playbackIncrement = 1;
int STATE_playbackInterrupted = 0;


int main ( void )
{
	
	EnableDisplay();
	
	EnableADC();
	EnableDAC();
	
	// Initialise the recordings
	int i = 0;
	for ( i = 0; i < NUM_RECORDINGS; i++ ) {
		STATE_recordings[i].length = 0;
	}
	
	
	InitUI();
	
	SetupButtonHandlers();
	
	// Pipe input straight to output
	/*while(1) {
		unsigned int micInput = GetADCReading();
		
		SetDACOutput( micInput );
	}*/
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
	DrawScreenHome();
};


void ClearScreenContents ( void )
{
	lcd_fillScreen( UI_BG );
	DrawHeader();
}


void DrawScreen ( void )
{
	
	ClearScreenContents();
	
	switch ( STATE_screen ) {
		
		case HOME:
			DrawScreenHome();
			break;
		case RECORDING:
			DrawScreenRecording();
			break;
		case PLAYBACK:
			DrawScreenPlayback();
			break;
	}
}
	
	
void DrawScreenHome ( void )
{
	DrawRecordingList();
	DrawHomeScreenButtons();
}


void DrawScreenPlayback ( void )
{
	
	char recordingLength[6] = "/00:00";
	int seconds = STATE_recordings[STATE_selectedRecording].length/RECORDING_RATE;
	
	// Draw the top panel
	lcd_fillRect(
		0,
		UI_HEADER_HEIGHT*1.5,
		DISPLAY_WIDTH,
		UI_HEADER_HEIGHT*1.5 + 115,
		BLACK
	);
	
	lcd_fontColor( WHITE, BLACK );
	
	
	if ( seconds < 10 ) {
		itoa( 0, &recordingLength[4], 10 );
		itoa( seconds, &recordingLength[5], 10 );
	} else {
		itoa( seconds, &recordingLength[4], 10 );
	}
	
	lcd_putString( 
		DISPLAY_WIDTH - 46,
		UI_HEADER_HEIGHT*1.5 + 4,
		recordingLength
	);
	
	
	// Print the instructions
	lcd_fontColor( UI_TEXT, UI_BG );
	lcd_putString( 
		(DISPLAY_WIDTH - 138)/2,
		DISPLAY_HEIGHT - UI_HEADER_HEIGHT,
		"Press UP to return home"
	);
	
	lcd_putString( 
		(DISPLAY_WIDTH - 198)/2,
		UI_HEADER_HEIGHT*2.5 + 115,
		"Press L/R to alter playback speed"
	);
	
	
	DrawWholePlaybackWaveform();
	DrawPlaybackProgress();
	DrawPlaybackSpeedBar();
	
	DrawPlaybackButtons();
	
}


void DrawScreenRecording ( void )
{
	
	char maxRecordingLength[6] = "/00:00";
	// Draw the top panel
	lcd_fillRect(
		0,
		UI_HEADER_HEIGHT*1.5,
		DISPLAY_WIDTH,
		UI_HEADER_HEIGHT*1.5 + 115,
		BLACK
	);
	
	lcd_fontColor( WHITE, BLACK );
	
	// Draw the maximum duration
	
	itoa( RECORDING_LENGTH, &maxRecordingLength[4], 10 );
	
	lcd_putString( 
		DISPLAY_WIDTH - 46,
		UI_HEADER_HEIGHT*1.5 + 4,
		maxRecordingLength
	);
	
	// Print the instructions
	lcd_fontColor( UI_TEXT, UI_BG );
	lcd_putString( 
		(DISPLAY_WIDTH - 138)/2,
		DISPLAY_HEIGHT - UI_HEADER_HEIGHT,
		"Press UP to return home"
	);
	
	DrawRecordingProgress();
	DrawWholeRecordingWaveform();
	
	DrawRecordingButtons();
}


void DrawHeader ( void )
{
	
	if( STATE_recordingInProgress ) {
		lcd_fillRect( 0, 0, DISPLAY_WIDTH, UI_HEADER_HEIGHT, UI_C_RECORDING );
		lcd_fontColor( UI_BG, UI_C_RECORDING );
	} else if( STATE_playbackInProgress ) {
		lcd_fillRect( 0, 0, DISPLAY_WIDTH, UI_HEADER_HEIGHT, UI_C_PLAYBACK );
		lcd_fontColor( UI_BG, UI_C_PLAYBACK );
	} else {
		lcd_fillRect( 0, 0, DISPLAY_WIDTH, UI_HEADER_HEIGHT, UI_C1 );
		lcd_fontColor( UI_BG, UI_C1 );
	}
	
	switch ( STATE_screen ) {
		
		case HOME:
			lcd_putString( 8, 3, "chqx69               RTC Assignment 2" );
			break;
		case RECORDING:
			lcd_putString( 8, 3, "RECORD               RTC Assignment 2" );
			break;
		case PLAYBACK:
			lcd_putString( 8, 3, "LISTEN               RTC Assignment 2" );
			break;
	}
	
	lcd_fontColor( UI_TEXT, UI_BG );
}


// Draw the list of recordings
inline void DrawRecordingList()
{
	int i;
	int startY = UI_HEADER_HEIGHT*2;
	int rowY;
	char recordingName[11] = "Recording 0";
	char recordingLength[5] = "00:00";
	int seconds;
	
	for( i = 0; i < NUM_RECORDINGS; i++ ) {
		
		rowY = startY + i*UI_ROW_HEIGHT;
		
		// Compose the track names and lengths
		itoa( i+1, &recordingName[10], 10 );
		
		seconds = STATE_recordings[i].length/RECORDING_RATE;
		
		if ( seconds < 10 ) {
			itoa( 0, &recordingLength[3], 10 );
			itoa( seconds, &recordingLength[4], 10 );
		} else {
			itoa( seconds, &recordingLength[3], 10 );
		}
		
		
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
			UI_ROW_PADDING*3,
			rowY + ( UI_ROW_HEIGHT/2 - 3 ),
			recordingName
		);
		
		lcd_putString( 
			DISPLAY_WIDTH - UI_ROW_PADDING*2 - 33,
			rowY + ( UI_ROW_HEIGHT/2 - 3 ),
			recordingLength
		);
		
	}
	
	lcd_fontColor( UI_TEXT, UI_BG );
}


void DrawHomeScreenButtons ( void )
{
	
	int buttonY = DISPLAY_HEIGHT - (UI_HOME_BUTTON_RADIUS/2)*3;
	
	//Clear draw areas
	lcd_fillcircle( 
		DISPLAY_WIDTH/4,
		buttonY,
		UI_HOME_BUTTON_RADIUS,
		UI_BG
	);
	
	lcd_fillcircle( 
		(DISPLAY_WIDTH/4)*3,
		buttonY,
		UI_HOME_BUTTON_RADIUS,
		UI_BG
	);
	
	
	if ( STATE_selectedActionHome != PLAY ) {
		// Draw unselected play button
		lcd_circle( 
			DISPLAY_WIDTH/4,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_TEXT
		);
		
		DrawPlayButton(
			DISPLAY_WIDTH/4 - UI_HOME_BUTTON_RADIUS/3,
			buttonY - UI_HOME_BUTTON_RADIUS/2,
			UI_HOME_BUTTON_RADIUS,
			UI_TEXT
		);
		
		// Draw selected recording button
		
		lcd_fillcircle( 
			(DISPLAY_WIDTH/4)*3,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_C_RECORDING
		);
		
		lcd_fillcircle( 
			(DISPLAY_WIDTH/4)*3,
			buttonY,
			UI_HOME_BUTTON_RADIUS/2,
			UI_BG
		);
		
	} else {
		
		// Draw selected play button
		lcd_fillcircle( 
			DISPLAY_WIDTH/4,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_TEXT
		);
		
		DrawPlayButton(
			DISPLAY_WIDTH/4 - UI_HOME_BUTTON_RADIUS/3,
			buttonY - UI_HOME_BUTTON_RADIUS/2,
			UI_HOME_BUTTON_RADIUS,
			UI_BG
		);
		
		// Draw unselected recording button
		lcd_circle( 
			(DISPLAY_WIDTH/4)*3,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_C_RECORDING
		);
		
		lcd_fillcircle( 
			(DISPLAY_WIDTH/4)*3,
			buttonY,
			UI_HOME_BUTTON_RADIUS/2,
			UI_C_RECORDING
		);
		
	}
	
	
	
}


void DrawPlayButton ( int x, int y, int h, lcd_color_t colour )
{
	int i = 0;
	
	for ( i = 0; i < h; i++ ) {
		
		lcd_line(
			x + i,
			y + i/2,
			x + i,
			y + h - i/2,
			colour
		);
		
	}
}


void DrawRecordingProgress ( void )
{
	
	// Update the progress text
	lcd_fontColor( WHITE, BLACK );
	
	char recordingLength[6] = "00:00";
	
	int seconds = STATE_recordings[STATE_selectedRecording].length/RECORDING_RATE;
		
	if ( seconds < 10 ) {
		itoa( 0, &recordingLength[3], 10 );
		itoa( seconds, &recordingLength[4], 10 );
	} else {
		itoa( seconds, &recordingLength[3], 10 );
	}
	
	lcd_putString( 
		DISPLAY_WIDTH - 76,
		UI_HEADER_HEIGHT*1.5 + 4,
		recordingLength
	);
}


void DrawWholeRecordingWaveform ( void )
{
	const int baseY = UI_HEADER_HEIGHT*1.5 + 115;
	
	int l = STATE_recordings[STATE_selectedRecording].length;
	int i = 0;
	int x = 0;
	
	for ( i = 0; i < l; i += WAVE_REDRAW_INTERVAL ) {
		
		lcd_line(
			x,
			baseY,
			x,
			baseY - ( (STATE_recordings[STATE_selectedRecording].samples[i]*100) / 1024 ),
			UI_C1
		);
		
		x++;
	}
	
}


void ClearRecordingProgress ( void )
{
	// Update the progress text
	lcd_fontColor( WHITE, BLACK );
	
	char recordingLength[6] = "00:00";
	itoa( 0, &recordingLength[3], 10 );
	
	lcd_putString( 
		DISPLAY_WIDTH - 76,
		UI_HEADER_HEIGHT*1.5 + 4,
		recordingLength
	);
}


void DrawRecordingButtons ( void )
{
	int buttonY = DISPLAY_HEIGHT - (UI_HOME_BUTTON_RADIUS/2)*3;
	
	//Clear draw areas
	lcd_fillRect(
		0,
		buttonY - UI_HOME_BUTTON_RADIUS,
		DISPLAY_WIDTH,
		buttonY + UI_HOME_BUTTON_RADIUS,
		UI_BG
	);
	
	
	
	//Draw clear button
	if ( STATE_selectedActionRecording != RECORD_CLEAR ) {
		lcd_fontColor( UI_TEXT, UI_BG );
		lcd_circle( 
			DISPLAY_WIDTH/4,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_TEXT
		);
		
		lcd_putString( DISPLAY_WIDTH/4-14, buttonY-3, "CLEAR" );
	} else {
		lcd_fontColor( UI_BG, UI_TEXT );
		lcd_fillcircle( 
			DISPLAY_WIDTH/4,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_TEXT
		);
		
		lcd_putString( DISPLAY_WIDTH/4-14, buttonY-3, "CLEAR" );
	}
	
	int recordingButtonX = (DISPLAY_WIDTH/4)*3;
	
	//Draw recording button
	if ( STATE_selectedActionRecording != RECORD_RECORD ) {	
		lcd_circle( 
			recordingButtonX,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_C_RECORDING
		);
		
		lcd_fillcircle( 
			recordingButtonX,
			buttonY,
			UI_HOME_BUTTON_RADIUS/2,
			UI_C_RECORDING
		);
	} else {
		lcd_fillcircle( 
			recordingButtonX,
			buttonY,
			UI_HOME_BUTTON_RADIUS,
			UI_C_RECORDING
		);
		
		// Draw a pause icon if recording
		if ( !STATE_recordingInProgress ) {
			lcd_fillcircle( 
				recordingButtonX,
				buttonY,
				UI_HOME_BUTTON_RADIUS/2,
				UI_BG
			);
		} else {
			lcd_fillRect(
				recordingButtonX - 10,
				buttonY - UI_HOME_BUTTON_RADIUS/2,
				recordingButtonX - 3,
				buttonY + UI_HOME_BUTTON_RADIUS/2,
				UI_BG
			);
			lcd_fillRect(
				recordingButtonX + 3,
				buttonY - UI_HOME_BUTTON_RADIUS/2,
				recordingButtonX + 10,
				buttonY + UI_HOME_BUTTON_RADIUS/2,
				UI_BG
			);
		}
	}
}


inline void DrawWholePlaybackWaveform ( void )
{
	DrawPlaybackWaveform( STATE_recordings[STATE_selectedRecording].length, UI_C1 );
}


void DrawPlaybackWaveform ( int cutoff, lcd_color_t colour )
{
	const int baseY = UI_HEADER_HEIGHT*1.5 + 115;
	
	int l = STATE_recordings[STATE_selectedRecording].length;
	int scaledWaveRedrawInterval = l/DISPLAY_WIDTH;
	int i = 0;
	int x = 0;
	
	for ( i = 0; i < min(l, cutoff); i += scaledWaveRedrawInterval ) {
		
		lcd_line(
			x,
			baseY,
			x,
			baseY - ( (STATE_recordings[STATE_selectedRecording].samples[i]*100) / 1024 ),
			colour
		);
		
		x++;
	}
}


void DrawPlaybackProgress ( void )
{
	
	const int baseY = UI_HEADER_HEIGHT*1.5 + 115;
	int scaledWaveRedrawInterval = STATE_recordings[STATE_selectedRecording].length/DISPLAY_WIDTH;
	int progressX = STATE_playbackPosition/scaledWaveRedrawInterval;
	char recordingLength[6] = "00:00";
	int seconds = STATE_playbackPosition/RECORDING_RATE;
		
	if ( seconds < 10 ) {
		itoa( 0, &recordingLength[3], 10 );
		itoa( seconds, &recordingLength[4], 10 );
	} else {
		itoa( seconds, &recordingLength[3], 10 );
	}
	
	// Update the progress text
	lcd_fontColor( WHITE, BLACK );
	
	lcd_putString( 
		DISPLAY_WIDTH - 76,
		UI_HEADER_HEIGHT*1.5 + 4,
		recordingLength
	);
	
	// Draw the playback bar
	lcd_fillRect(
		0,
		baseY,
		progressX,
		baseY + UI_PLAYBACK_BAR_HEIGHT,
		UI_C_PLAYBACK
	);
	
	//Erase anything after the playback bar
	lcd_fillRect(
		progressX,
		baseY,
		DISPLAY_WIDTH,
		baseY + UI_PLAYBACK_BAR_HEIGHT,
		UI_BG
	);
}


void DrawPlaybackButtons ( void )
{
	
	int buttonY = DISPLAY_HEIGHT - (UI_HOME_BUTTON_RADIUS/2)*3;
	int buttonX = DISPLAY_WIDTH/2;

	// Draw selected play button
	lcd_fillcircle( 
		buttonX,
		buttonY,
		UI_HOME_BUTTON_RADIUS,
		UI_TEXT
	);
	
	if ( !STATE_playbackInProgress ) {
	
		DrawPlayButton(
			buttonX - UI_HOME_BUTTON_RADIUS/3,
			buttonY - UI_HOME_BUTTON_RADIUS/2,
			UI_HOME_BUTTON_RADIUS,
			UI_BG
		);
	
	} else {
		
		lcd_fillRect(
			buttonX - 10,
			buttonY - UI_HOME_BUTTON_RADIUS/2,
			buttonX - 3,
			buttonY + UI_HOME_BUTTON_RADIUS/2,
			UI_BG
		);
		lcd_fillRect(
			buttonX + 3,
			buttonY - UI_HOME_BUTTON_RADIUS/2,
			buttonX + 10,
			buttonY + UI_HOME_BUTTON_RADIUS/2,
			UI_BG
		);
		
	}
}


void DrawPlaybackSpeedBar ( void )
{
	// Break out the float into digits and deal with them individually
	
	int baseY = UI_HEADER_HEIGHT*2.5 + 155;
	char speedString[] = "Speed: +0.00";
	char unitString[] = "0";
	char tenthString[] = "0";
	char hundredthString[] = "0";
	int units = abs((int)STATE_playbackSpeed)%10;
	int tenths = abs((int)(STATE_playbackSpeed*10.0))%10;
	int hundredths = abs((int)(STATE_playbackSpeed*100.0))%10;
	
	speedString[7] = STATE_playbackSpeed < 0.0 ? '-' : '+';
	
	itoa( units, unitString, 10 );
	itoa( tenths, tenthString, 10 );
	itoa( hundredths, hundredthString, 10 );
	
	speedString[8] = unitString[0];
	speedString[10] = tenthString[0];
	speedString[11] = hundredthString[0];
	
	lcd_fontColor( UI_TEXT, UI_BG );
	
	lcd_putString(
		(DISPLAY_WIDTH - 72)/2,
		baseY,
		speedString
	);
}


void ClearRecording ( void )
{
	int i = 0;
	STATE_recordings[STATE_selectedRecording].length = 0;
	
	for ( i = 0; i < RECORDING_LENGTH*RECORDING_RATE; i++ ) {
		STATE_recordings[STATE_selectedRecording].samples[i] = 0;
	}
}


void StartRecording ( void )
{
	STATE_recordingInProgress = 1;
	DrawRecordingButtons();
	DrawHeader();
	
	unsigned int *l = &(STATE_recordings[STATE_selectedRecording].length);
	int x = *l/WAVE_REDRAW_INTERVAL;
	const int baseY = UI_HEADER_HEIGHT*1.5 + 115;
	unsigned int sample;
	
	while ( STATE_recordingInProgress  ) {
		
		if ( *l == MAX_RECORDING_LENGTH || IsButtonPressed( BUTTON_CENTRE ) ) {
			STATE_recordingInterrupted = IsButtonPressed( BUTTON_CENTRE );
			StopRecording();
			break;
		}
		
		sample = GetADCReading();
		STATE_recordings[STATE_selectedRecording].samples[*l] = sample;
		
		if ( *l % WAVE_REDRAW_INTERVAL == 0 ) {
			DrawRecordingProgress();
			lcd_line(
				x,
				baseY,
				x,
				baseY - ( (sample*100) / 1024 ),
				UI_C1
			);
			x++;
		}
		*l = *l + 1;
	}
}


void StopRecording ( void )
{
	STATE_recordingInProgress = 0;
	DrawRecordingProgress();
}


void StartPlayback ( void )
{
	STATE_playbackInProgress = 1;
	
	int l = STATE_recordings[STATE_selectedRecording].length;
	int scaledWaveRedrawInterval = l/DISPLAY_WIDTH;
	
	if ( !l ) {
		StopPlayback();
	}
	
	DrawPlaybackButtons();
	DrawHeader();
		
	while ( STATE_playbackInProgress ) {
		
		if ( IsButtonPressed( BUTTON_CENTRE ) ) {
			STATE_playbackInterrupted = 1;
			StopPlayback();
		}/* else if ( IsButtonPressed( BUTTON_LEFT ) ) {
			HandleButtonPressPlayback( LEFT );
			STATE_playbackButtonPressed = 1;
		} else if ( IsButtonPressed( BUTTON_RIGHT ) ) {
			HandleButtonPressPlayback( LEFT );
			STATE_playbackButtonPressed = 1;
		} else {
			STATE_playbackButtonPressed = 0;
		}*/
		
		WaitForCycles( STATE_playbackWaitCycles );
		
		STATE_playbackPosition += STATE_playbackIncrement;
		
		if ( STATE_playbackPosition > l ) {
			STATE_playbackPosition = 0;
		} else if( STATE_playbackPosition < 0 ) {
			STATE_playbackPosition = l;
		}
		
		SetDACOutput( STATE_recordings[STATE_selectedRecording].samples[STATE_playbackPosition] );
		
		if ( STATE_playbackPosition%scaledWaveRedrawInterval == 0 ) {
			DrawPlaybackProgress();
		}
	}
	
}

void StopPlayback ( void )
{
	STATE_playbackInProgress = 0;
	SetDACOutput( 0 );
	DrawPlaybackProgress();
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
	Button buttonPressed;
	if ( IO0_INT_STAT_R & (1<<BUTTON_UP) ) {
		buttonPressed = UP;
	}
	if ( IO0_INT_STAT_R & (1<<BUTTON_DOWN) ) {
		buttonPressed = DOWN;
	}
	if ( IO0_INT_STAT_R & (1<<BUTTON_LEFT) ) {
		buttonPressed = LEFT;
	}
	if ( IO0_INT_STAT_R & (1<<BUTTON_RIGHT) ) {
		buttonPressed = RIGHT;
	}
	if ( IO0_INT_STAT_R & (1<<BUTTON_CENTRE) ) {
		buttonPressed = CENTRE;
	}
	
	switch ( STATE_screen ) {
		
		case HOME:
			HandleButtonPressHome ( buttonPressed );
			break;
		case RECORDING:
			HandleButtonPressRecording ( buttonPressed );
			break;
		case PLAYBACK:
			HandleButtonPressPlayback ( buttonPressed );
			break;
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


void HandleButtonPressHome ( Button button )
{
	
	switch ( button ) {
		
		case CENTRE:
			STATE_screen = (STATE_selectedActionHome == PLAY) ? PLAYBACK : RECORDING;
			DrawScreen();
			break;
		
		case UP:
			STATE_selectedRecording = max( 0, STATE_selectedRecording - 1 );
			DrawRecordingList();
			break;
			
		case DOWN:
			STATE_selectedRecording = min( NUM_RECORDINGS - 1, STATE_selectedRecording + 1 );
			DrawRecordingList();
			break;
			
		case LEFT:
		case RIGHT:
			STATE_selectedActionHome = STATE_selectedActionHome == PLAY ? RECORD : PLAY;
			DrawHomeScreenButtons();
			break;
		
	}
}


void HandleButtonPressRecording ( Button button )
{
	switch ( button ) {
		
		case CENTRE:
			
			switch ( STATE_selectedActionRecording ) {
				case RECORD_RECORD:
					if ( STATE_recordingInProgress ) {
						StopRecording();
					} else if( !STATE_recordingInterrupted ) {
						StartRecording();
					} else {
						STATE_recordingInterrupted = 0;
					}
					DrawRecordingButtons();
					break;
				
				case RECORD_CLEAR:
					ClearRecording();
					DrawScreenRecording();
					break;
			}
			
			break;
		
		case UP:
			StopRecording();
			STATE_screen = HOME;
			DrawScreen();
			break;
			
		case DOWN:
			break;
			
		case LEFT:
		case RIGHT:
			if ( STATE_recordingInProgress ) break;
			STATE_selectedActionRecording = STATE_selectedActionRecording == RECORD_CLEAR ? RECORD_RECORD : RECORD_CLEAR;
			DrawRecordingButtons();
			break;
		
	}
	
	DrawHeader();
}


void HandleButtonPressPlayback ( Button button )
{
	
	switch ( button ) {
		
		case CENTRE:
			if ( STATE_playbackInProgress ) {
				StopPlayback();
			} else if ( !STATE_playbackInterrupted ) {
				StartPlayback();
			} else {
				STATE_playbackInterrupted = 0;
			}
			
			DrawPlaybackButtons();
			DrawHeader();
			break;
		
		case UP:
			StopPlayback();
			STATE_screen = HOME;
			DrawScreen();
			break;
			
		case DOWN:
			break;
			
		case LEFT:
			// Change the playback speed and disallow 0 as a value
			STATE_playbackSpeed = max(
				STATE_playbackSpeed - PLAYBACK_SPEED_DIFF,
				-PLAYBACK_SPEED_MAX
			);
			if ( STATE_playbackSpeed > -PLAYBACK_SPEED_DIFF/2
				&& STATE_playbackSpeed < PLAYBACK_SPEED_DIFF/2 ) {
				STATE_playbackSpeed = -PLAYBACK_SPEED_DIFF;
			}
			DrawPlaybackSpeedBar();
			break;
			
		case RIGHT:
			// Change the playback speed and disallow 0 as a value
			STATE_playbackSpeed = min(
				STATE_playbackSpeed + PLAYBACK_SPEED_DIFF,
				PLAYBACK_SPEED_MAX
			);
			if ( STATE_playbackSpeed > -PLAYBACK_SPEED_DIFF/2
				&& STATE_playbackSpeed < PLAYBACK_SPEED_DIFF/2 ) {
				STATE_playbackSpeed = PLAYBACK_SPEED_DIFF;
			}
			DrawPlaybackSpeedBar();
			break;
		
	}
	
	// Compute the playback cycles
	if ( STATE_playbackSpeed > 0 ) {
		STATE_playbackWaitCycles = (int)(250.0 / STATE_playbackSpeed);
		STATE_playbackIncrement = 1;
	} else {
		STATE_playbackWaitCycles = (int)(250.0 / -STATE_playbackSpeed);
		STATE_playbackIncrement = -1;
	}

}


// Wait a number of milliseconds using T2
void wait ( unsigned int milliseconds )
{
	int waitCycles = MillisecondsToCycles( milliseconds );
	
	WaitForCycles( waitCycles );
}


// Wait for a particular number of cycles
void WaitForCycles ( unsigned int cycles )
{
	EnableTimer( TIMER_ENABLE_2 );
	
	// Reset the timer to 0
	T2TCR = 2;
	
	// Clear all the T2 interrupt bits
	T2IR |= 0x0F;
	
	// Set the timer target value
	T2MR0 = cycles;
	
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