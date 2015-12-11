#include <lpc24xx.h>
#include <textDisplay.h>
#include <lcd_grph.h>
#include <stdlib.h>

#define DEBUG 0

#define TRUE 1
#define FALSE 0

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))
#define even(a) (a%2==0)

#define EMPTY_MASK 0

#define TIMER_ENABLE_0 1
#define TIMER_ENABLE_1 2
#define TIMER_ENABLE_2 22
#define TIMER_ENABLE_3 23

#define BUTTON_UP 10
#define BUTTON_DOWN 11
#define BUTTON_LEFT 12
#define BUTTON_RIGHT 13
#define BUTTON_CENTRE 22

#define UI_HEADER_HEIGHT 13

#define UI_ROW_HEIGHT 20
#define UI_ROW_PADDING 3

#define UI_PLAYBACK_BAR_HEIGHT 5

#define UI_BG WHITE
#define UI_TEXT DARK_GRAY
#define UI_C1 BLUE
#define UI_C2 NAVY
#define UI_C_RECORDING RED
#define UI_C_PLAYBACK 0xFD20

#define UI_HOME_BUTTON_RADIUS 35

#define CPU_MHZ 12

#define VOLUME_DEFAULT 5

#define NUM_RECORDINGS 5
#define RECORDING_LENGTH 10
#define RECORDING_RATE 44000


typedef struct recording {
	unsigned int samples[RECORDING_LENGTH*RECORDING_RATE];
	unsigned int length; // Length in samples
} Recording;

typedef enum {HOME, RECORDING, PLAYBACK} ScreenState;

typedef enum button {CENTRE, UP, RIGHT, DOWN, LEFT} Button;


void InitUI ( void );
void ClearScreenContents ( void );
void DrawScreen ( void );
void DrawScreenHome ( void );
void DrawScreenPlayback ( void );
void DrawScreenRecording ( void );

void DrawHeader ( void );

void DrawRecordingList ( void );
void DrawHomeScreenButtons ( void );
void DrawPlayButton ( int x, int y, int w, lcd_color_t colour );

void DrawRecordingProgress ( void );
void DrawWholeRecordingWaveform ( void );
void ClearRecordingProgress ( void );
void DrawRecordingButtons ( void );

inline void DrawWholePlaybackWaveform ( void );
void DrawPlaybackWaveform ( int cutoff, lcd_color_t colour );
void DrawPlaybackProgress ( void );
void DrawPlaybackButtons ( void );

void ClearRecording ( void );
void StartRecording ( void );
void StopRecording ( void );

void EnableADC ( void );
inline void TakeADCReading( void );
inline unsigned int GetADCReading( void );

void EnableDAC ( void );
inline void SetDACOutput( unsigned int voltage );

inline void SetupButtonHandlers ( void );
void OnButtonPress(void) __attribute__ ((interrupt));
void HandleButtonPressHome ( Button button );
void HandleButtonPressRecording ( Button button );
void HandleButtonPressPlayback ( Button button );

void wait ( unsigned int milliseconds );
void WaitForCycles ( unsigned int cycles );

inline unsigned int MillisecondsToCycles( unsigned int milliseconds );
inline unsigned int CyclesToMilliseconds( unsigned int cycles );

void EnableTimer ( int timer );
void EnableDisplay ( void );

inline unsigned long ToggleBit ( unsigned long number, unsigned int toggleBit );
inline unsigned long SetBitOn ( unsigned long number, unsigned int onBit );
inline unsigned long SetBitOff ( unsigned long number, unsigned int offBit );
inline unsigned int IsBitOn ( unsigned long number, unsigned int testBit );

void SetPortPinValue ( unsigned int port, unsigned int pin, int value );
unsigned int GetPortPinValue ( unsigned int port, unsigned int pin );
inline unsigned int IsButtonPressed( int button );