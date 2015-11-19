#include <lpc24xx.h>
#include <textDisplay.h>
#include <lcd_grph.h>
#include <stdlib.h>

#define DEBUG 0

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

#define PULSE_PERIOD 6000
#define NUM_SELECTABLE_SPEEDS 7
#define MEASURE_INTERVAL 1000
#define MIN_SPEED 30
#define MAX_SPEED 120

#define UI_ROW_HEIGHT 20
#define UI_ROW_PADDING 3

#define CPU_MHZ 12

void MeasureSpeed ( void );
void ControlSpeed ( void );

void InitUI ( void );
inline void DrawOptions ( void );
inline void DrawRevs ( void );
inline void DrawDesiredSpeed ( void );
inline void DrawDebug ( int diff, int gainFactor, int nextPulseWidth, int pulseWidth );

inline void SetupButtonHandlers ( void );
void OnButtonPress(void) __attribute__ ((interrupt));

void wait ( unsigned int milliseconds );

inline unsigned int MillisecondsToCycles( unsigned int milliseconds );
inline unsigned int CyclesToMilliseconds( unsigned int cycles );

void EnableTimer ( int timer );
void EnableDisplay ( void );
void EnableMotor ( void );
void EnableRevCounter ( void );
void SetPulseWidth ( int pulseWidth );

inline unsigned long ToggleBit ( unsigned long number, unsigned int toggleBit );
inline unsigned long SetBitOn ( unsigned long number, unsigned int onBit );
inline unsigned long SetBitOff ( unsigned long number, unsigned int offBit );
inline unsigned int IsBitOn ( unsigned long number, unsigned int testBit );

void SetPortPinValue ( unsigned int port, unsigned int pin, int value );
unsigned int GetPortPinValue ( unsigned int port, unsigned int pin );
inline unsigned int IsButtonPressed( int button );