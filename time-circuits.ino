#include <TimeLib.h>
#include <TinyGPS++.h>

#define ENABLE_PIN 8
#define RESET_PIN 9
#define LATCH_PIN 10
#define DATA_PIN  11
#define CLOCK_PIN 13

// font derived from https://scratch.mit.edu/projects/11093075/
uint16_t font[ 96 ] = { 0x0, 0x13000, 0x2040, 0x3C55, 0xDD55, 0x9977, 0x9EC9, 0x40, 0x28, 0x82, 0xFF, 0x55, 0x2, 0x11, 0x10000, 0x22, 0xFF22, 0xC44, 0xEE11, 0xFC11, 0x155, 0xCD09, 0xDF11, 0xC033, 0xFF11, 0xFD11, 0x8400, 0x8002, 0xC22, 0xC11, 0xC88, 0x1E014, 0xFE05, 0x3032, 0xFC54, 0xCF00, 0xFC44, 0xCF11, 0xC311, 0xDF10, 0x3311, 0xCC44, 0x3E00, 0x329, 0xF00, 0x33A0, 0x3388, 0xFF00, 0xE311, 0xFF08, 0xE319, 0xDD11, 0xC044, 0x3F00, 0x322, 0x330A, 0xAA, 0x2115, 0xCC22, 0x4844, 0x88, 0x8444, 0x180, 0xC00, 0x80, 0x60D, 0x705, 0x601, 0xE45, 0x603, 0x4055, 0x8545, 0x305, 0x804, 0x444, 0x5C, 0x844, 0x1215, 0x205, 0x605, 0x8341, 0x8145, 0x201, 0x818, 0x855, 0xE04, 0x202, 0x120A, 0x66, 0xA2, 0x403, 0x4845, 0x44, 0x8454, 0xC000 };
byte nums[ 10 ] = { 0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1F, 0x01, 0x09 };
byte frameBuffer[ 16 ];
TinyGPSPlus gps;
const int offset = -4;

uint16_t showFont( char c, int p ) {
  switch ( p ) {
    case 0: p = 4; break;
    case 1: p = 2; break;
    case 2: p = 0; break;
    case 3: p = 6; break;
    case 4: p = 7; break;
    case 5: p = 10; break;
    case 6: p = 11; break;
    case 7: p = 8; break;
    case 8: p = 9; break;
    case 9: p = 12; break;
    case 10: p = 13; break;
    case 11: p = 14; break;
    case 12: p = 15; break;
  }
  if ( p < 6 ) {
    frameBuffer[ p ] = font[ c - 32 ] >> 8;
    frameBuffer[ p + 1 ] = font[ c - 32 ];
  }
  else frameBuffer[ p ] = nums[ c ];
}

void writeMonth( int m ) {
  switch ( m ) {
    case 1: showFont( 'J', 0 ); showFont( 'A', 1 ); showFont( 'N', 2 ); break;
    case 2: showFont( 'F', 0 ); showFont( 'E', 1 ); showFont( 'B', 2 ); break;
    case 3: showFont( 'M', 0 ); showFont( 'A', 1 ); showFont( 'R', 2 ); break;
    case 4: showFont( 'A', 0 ); showFont( 'P', 1 ); showFont( 'R', 2 ); break;
    case 5: showFont( 'M', 0 ); showFont( 'A', 1 ); showFont( 'Y', 2 ); break;
    case 6: showFont( 'J', 0 ); showFont( 'U', 1 ); showFont( 'N', 2 ); break;
    case 7: showFont( 'J', 0 ); showFont( 'U', 1 ); showFont( 'L', 2 ); break;
    case 8: showFont( 'A', 0 ); showFont( 'U', 1 ); showFont( 'G', 2 ); break;
    case 9: showFont( 'S', 0 ); showFont( 'E', 1 ); showFont( 'P', 2 ); break;
    case 10: showFont( 'O', 0 ); showFont( 'C', 1 ); showFont( 'T', 2 ); break;
    case 11: showFont( 'N', 0 ); showFont( 'O', 1 ); showFont( 'V', 2 ); break;
    case 12: showFont( 'D', 0 ); showFont( 'E', 1 ); showFont( 'C', 2 ); break;
  }
}

void writeDay( int d ) {
  byte d1 = d / 10;
  byte d2 = d % 10;
  showFont( d1, 3 );
  showFont( d2, 4 );
}

void writeYear( int y ) {
  int y1 = ( y / 1000 ) % 10;
  int y2 = ( y / 100 ) % 10;
  int y3 = ( y / 10 ) % 10;
  int y4 = ( y / 1 ) % 10;
  showFont( y1, 5 );
  showFont( y2, 6 );
  showFont( y3, 7 );
  showFont( y4, 8 );
}

void writeHour( int h ) {
  h = h % 12;
  byte h1 = h / 10;
  byte h2 = h % 10;
  showFont( h1, 9 );
  showFont( h2, 10 );
}

void writeMinute( int m ) {
  byte m1 = m / 10;
  byte m2 = m % 10;
  showFont( m1, 11 );
  showFont( m2, 12 );
}

void writeSecond( int s ) {
  if ( s % 2 ) {
    frameBuffer[ 14 ] &= ~( 1 << 0 );
    frameBuffer[ 15 ] &= ~( 1 << 0 );
  }
  else {
    frameBuffer[ 14 ] |= ( 1 << 0 );
    frameBuffer[ 15 ] |= ( 1 << 0 );
  }
}

void drawDisplay() {
  digitalWrite( LATCH_PIN, LOW );
  for ( int i = 0; i < 16; i++ ) shiftOut( DATA_PIN, CLOCK_PIN, LSBFIRST, frameBuffer[ 15 - i ] );
  digitalWrite( LATCH_PIN, HIGH );
}

void setup() {
  Serial.begin( 9600 );
  pinMode( ENABLE_PIN, OUTPUT );
  pinMode( RESET_PIN, OUTPUT );
  pinMode( LATCH_PIN, OUTPUT );
  pinMode( DATA_PIN, OUTPUT );
  pinMode( CLOCK_PIN, OUTPUT );
  digitalWrite( ENABLE_PIN, LOW );
  digitalWrite( RESET_PIN, HIGH );
}

time_t prevDisplay = 0;

void loop() {
  if ( Serial.available() ) gps.encode( Serial.read() );
  if (gps.location.isUpdated()) {
    int Year = gps.date.year();
    byte Month = gps.date.month();
    byte Day = gps.date.day();
    byte Hour = gps.time.hour();
    byte Minute = gps.time.minute();
    byte Second = gps.time.second();
    setTime( Hour, Minute, Second, Day, Month, Year );
    adjustTime( offset * SECS_PER_HOUR );
    writeYear( year() );
    writeMonth( month() );
    writeDay( day() );
    writeMinute( minute() );
    writeHour( hour() );
    writeSecond( second() );
    if (timeStatus() != timeNotSet) {
      if (now() != prevDisplay) { //update the display only if the time has changed
        prevDisplay = now();
        if ( hour() > 11 ) {
          frameBuffer[ 12 ] |= ( 1 << 0 );
          frameBuffer[ 13 ] &= ~( 1 << 0 );
        }
        else {
          frameBuffer[ 12 ] &= ~( 1 << 0 );
          frameBuffer[ 13 ] |= ( 1 << 0 );
        }
        drawDisplay();
      }
    }
  }
}
