/*

  This example uses the Adafruit DS1307 real time clock to measure time. This sketch takes the hours and minutes values and displays them on the IN-9 Nixie tubes.

  The RTC is plugged into the UNO with A2 as GND and A3 as 5v.

*/

// This sketches needs the following libraries:
#include <PWM.h>
#include <Wire.h>
#include "RTClib.h"

#define TUBE1 9                // Define pins
#define TUBE2 10

// Adjust the max and min values for the tubes. This can compensate tube variations
#define TUBE1_PWM_MIN 40   // minimum duty cycle in the linear range of the IN-9 tube
#define TUBE1_PWM_MAX 205 // maximum duty cycle in the linear range.
#define TUBE2_PWM_MIN 40   // minimum duty cycle in the linear range of the IN-9 tube
#define TUBE2_PWM_MAX 205 // maximum duty cycle in the linear range.

#define SLEW_RATE 5 // To prevent rapid changes that can cause the plasma to not "clamp" to the bottom a slew rate limit is introduced to limit the maximum rate of any tube change. This equals 3ms.

RTC_DS1307 rtc;

// declare variables
int hours = 6;
int minutes = 30;
int old_hour_pwm = 0;
int old_min_pwm = 0;

// set the PWM frequency, 10kHz works well with the default RC values with the kit.
int32_t frequency = 10000; //frequency (in Hz)

void setup()
{
  Serial.begin(9600);

  // Set A2 as GND and A3 5v for the RTC power supply
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  digitalWrite(A2, LOW);
  digitalWrite(A3, HIGH);

  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // initialise the RTC
  if (! rtc.isrunning())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 59, 50));
  }
  else
  {
    Serial.println("RTC is working");
  }

  rtc.adjust(DateTime(2014, 1, 21, 3, 59, 50));

  // Now set up timers to allow for fast PWM.
  //initialize all timers except for 0, to save time keeping functions
  InitTimersSafe();

  // initialise the PWM pins
  if (!SetPinFrequencySafe(TUBE1, frequency))
  {
    Serial.println("Tube1 fail");
    while (1);
  }
  if (!SetPinFrequencySafe(TUBE2, frequency))
  {
    Serial.println("Tube2 fail");
    while (1);
  }

}

void loop()
{
  DateTime now = rtc.now();
  int time_hour = now.hour();
  int time_minute = now.minute();
  static uint8_t old_hour = 0;
  static uint8_t old_min = 0;

  // Change hour format to between 1 and 12
  if (time_hour > 12)
    time_hour -= 12;

  Serial.print(time_hour);
  Serial.print(":");
  Serial.print(time_minute);
  Serial.print(":");
  Serial.println(now.second());
  // of the time has changed then update the tubes
  if (time_hour != old_hour || time_minute != old_min)
  {
    old_hour = time_hour;
    old_min = time_minute;
    write_time(time_hour, time_minute);
  }
}


/* This function updates the time PWM values. It also includes a slew rate limit. This adds a delay time between successive pwm rights, preventing 
 *  fast changes that could cause the bottom to become detached from the bottom of the tube. Faster changes can be enabled by reducing the value of
 *  SLEW_RATE
*/
void write_time(int hours, int minutes)
{
  static uint8_t old_hour_pwm = 0;
  static uint8_t old_min_pwm = 0;
  uint8_t new_hour_pwm;
  uint8_t new_min_pwm;

  new_hour_pwm = map(hours, 0, 12, TUBE1_PWM_MIN, TUBE1_PWM_MAX);
  new_min_pwm = map(minutes, 0, 59, TUBE2_PWM_MIN, TUBE2_PWM_MAX);

  // while either or both of the new times are different from the old times increment the PWM value until they match
  while ((new_hour_pwm != old_hour_pwm) || (new_min_pwm != old_min_pwm))
  {
    if (old_hour_pwm == new_hour_pwm);    // if they are the same do nothing
    else if (old_hour_pwm > new_hour_pwm) // if the old value is larger than the new, decrease by 1
      old_hour_pwm--;
    else                                  // else the old value must be smaller that the new so increase it by 1
      old_hour_pwm++;

    if (old_min_pwm == new_min_pwm);
    else if (old_min_pwm > new_min_pwm)
      old_min_pwm--;
    else
      old_min_pwm++;

    // update the tube PWM duty cycles
    pwmWrite(TUBE1, old_hour_pwm);
    pwmWrite(TUBE2, old_min_pwm);

    delay(SLEW_RATE);
  }
}

