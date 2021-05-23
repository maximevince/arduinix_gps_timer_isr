// 6-tube IN-17 board with default connections + Ublox GPS attached to SPI.
// by Maxime Vincent aka vnz0r
//
// 2017-02-23 - modded to add GPS timesource by and ISR tube refreshing
//
// Using libraries:
// NeoGPS   for GPS parsing
// Time     for time keeping
// TimerOne for Timer1 interrupts
// Timezone for Timezones / DST
//

#include <SPI.h>
#include <NMEAGPS.h>    // https://github.com/SlashDevin/NeoGPS
#include <TimeLib.h>    // http://playground.arduino.cc/Code/time
#include <TimerOne.h>   // https://github.com/PaulStoffregen/TimerOne
#include <Timezone.h>   // https://github.com/JChristensen/Timezone


// Tube refresh rate in milliseconds
const int tube_delay = 2; // multiplexing delay; adjust for tube type

// Timezone
// For more examples, see https://github.com/JChristensen/Timezone/blob/master/examples/WorldClock/WorldClock.ino
//
// Central European Time (Frankfurt, Paris)
TimeChangeRule CEST = {"CEST", Last, Sun, Mar, 2, 120};     // Central European Summer Time
TimeChangeRule CET = {"CET ", Last, Sun, Oct, 3, 60};       // Central European Standard Time
Timezone myTimeZone(CEST, CET);
TimeChangeRule *tcr;        // pointer to the time change rule, use to get TZ abbrev

// SN74141 : Truth Table (BCD)
// D,C,B,A are BCD coded, with A being LSB

// SN74141 (1)
int ledPin_0_a = 2;
int ledPin_0_b = 3;
int ledPin_0_c = 4;
int ledPin_0_d = 5;

// SN74141 (2)
int ledPin_1_a = 6;
int ledPin_1_b = 7;
int ledPin_1_c = 8;
int ledPin_1_d = 9;

// anode pins
int ledPin_a_1 = 10;
int ledPin_a_2 = 11;
int ledPin_a_3 = 12;
int ledPin_a_4 = 13;

// Ublox GPS settings
const uint8_t ublox_CSn_PIN     = A5;
const uint8_t ublox_RESETn_PIN  = A4;
const uint8_t SPI_MODE          = 0;


static NMEAGPS gps;
static gps_fix fix_data;

void setup()
{
  pinMode(ledPin_0_a, OUTPUT);
  pinMode(ledPin_0_b, OUTPUT);
  pinMode(ledPin_0_c, OUTPUT);
  pinMode(ledPin_0_d, OUTPUT);

  pinMode(ledPin_1_a, OUTPUT);
  pinMode(ledPin_1_b, OUTPUT);
  pinMode(ledPin_1_c, OUTPUT);
  pinMode(ledPin_1_d, OUTPUT);

  pinMode(ledPin_a_1, OUTPUT);
  pinMode(ledPin_a_2, OUTPUT);
  pinMode(ledPin_a_3, OUTPUT);
  pinMode(ledPin_a_4, OUTPUT);

  // GPS-stuff
  GPSSetup();

  // Timer ISR
  Timer1.initialize(tube_delay * 1000); // set a timer of length 3000 microseconds (or 3 ms)
  Timer1.attachInterrupt(timerIsr); // attach the service routine here
}

void timerIsr()
{
  int days = day();
  int hours = hour();
  int minutes = minute();
  int seconds = second();

  // Get the high and low order values for hours,min,seconds.
  int lowerHours = hours % 10;
  int upperHours = hours - lowerHours;
  int lowerMins = minutes % 10;
  int upperMins = minutes - lowerMins;
  int lowerSeconds = seconds % 10;
  int upperSeconds = seconds - lowerSeconds;
  if( upperSeconds >= 10 )   upperSeconds = upperSeconds / 10;
  if( upperMins >= 10 )      upperMins = upperMins / 10;
  if( upperHours >= 10 )     upperHours = upperHours / 10;

  // Display.
  if (GPSIsTimeSynced())
  {
    // Display clock
    WriteDisplay(upperHours, lowerHours, upperMins, lowerMins, upperSeconds, lowerSeconds);
  }
  else
  {
    // Display loading bar
    static int bar[9] = {1, 1, 1, 10, 10, 10, 10, 10, 10};
    static unsigned long lastShift = millis();

    if (lastShift + 200 < millis())
    {
      const int len = sizeof(bar)/sizeof(int);
      const int temp = bar[len - 1]; // save temp value
      for (int i = len - 1; i > 0; i--)
      {
        bar[i] = bar[i - 1];
      }
      bar[0] = temp; // restore saved temp value
      lastShift = millis();
    }

    WriteDisplay(bar[5], bar[4], bar[3], bar[2], bar[1], bar[0]);
  }
}

void loop()
{
  // GPS loop
  GPSLoop();
}
