#include <lpc24xx.h>
#include <textDisplay.h>

#define EMPTY_MASK 0

#define BUTTON_UP 10
#define BUTTON_DOWN 11
#define BUTTON_LEFT 12
#define BUTTON_RIGHT 13
#define BUTTON_CENTRE 22

#define PULSE_PERIOD 6000


void wait ( unsigned int ticks );

void EnableDisplay ( void );
void EnableMotor ( void );
void SetPulseWidth ( int pulseWidth );

inline unsigned long ToggleBit ( unsigned long number, unsigned int toggleBit );
inline unsigned long SetBitOn ( unsigned long number, unsigned int onBit );
inline unsigned long SetBitOff ( unsigned long number, unsigned int offBit );
inline unsigned int IsBitOn ( unsigned long number, unsigned int testBit );

void SetPortPinValue ( unsigned int port, unsigned int pin, int value );
unsigned int GetPortPinValue ( unsigned int port, unsigned int pin );
inline unsigned int IsButtonPressed( int button );