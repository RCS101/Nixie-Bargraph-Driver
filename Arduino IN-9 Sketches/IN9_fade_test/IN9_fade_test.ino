/*
IN9_fade_test
 
This is the fade example but instead driving the tube heights.

Connect Tube1 and Tube2 to Arduino uno pins 9 and 10. These will be driven by the PWM library, not the standard Arduino PWM function. 
This allows a much higher PWM frequency to be used. In this case 10kHz.

This example code is in the public domain.
 */

// This sketches needs the following libraries:
#include <PWM.h> 

// Define pins
#define TUBE1 9                
#define TUBE2 10

// declare variables
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

// set the PWM frequency, 10kHz works well with the default RC values with the kit. 
int32_t frequency = 10000; //frequency (in Hz)

void setup()
{
  Serial.begin(9600);
  //initialize all timers except for 0, to save time keeping functions
  InitTimersSafe(); 

// initialise the PWM pins
  if(!SetPinFrequencySafe(TUBE1, frequency))
  {
    Serial.println("Tube1 fail");
    while(1);
  }
  if(!SetPinFrequencySafe(TUBE2, frequency))
  {
    Serial.println("Tube2 fail");
    while(1);
  }
}

void loop()
{
  // Write the PWM values
   pwmWrite(TUBE1, brightness);
   pwmWrite(TUBE2, 255-brightness);
   
   brightness = brightness + fadeAmount;

  // reverse the direction of the fading at the ends of the fade:
  if (brightness <= 0 || brightness >= 255) {
    fadeAmount = -fadeAmount;
  }
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}


