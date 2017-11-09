/*
  This example reads the value from ADC0 and ADC1 and displays their value on the tubes.

  The adc voltage can be controlled using a pot or any other analog signal.

  This example is in the public domian.
*/

// This sketches needs the following libraries:
#include <PWM.h>

#define TUBE1 9                // Define pins
#define TUBE2 10
#define PWM_MIN 20   // minimum duty cycle in the linear range of the IN-9 tube
#define PWM_MAX 200 // maximum duty cycle in the linear range.
#define SLEW_RATE 3 // To prevent rapid changes that can cause the plasma to not "clamp" to the bottom a slew rate limit is introduced to limit the maximum rate of any tube change. 

// declare variables
int tube1_pwm = 0;
int tube2_pwm = 255;
int old_tube1_pwm = 0;
int old_tube2_pwm = 0;

// set the PWM frequency, 10kHz works well with the default RC values with the kit.
int32_t frequency = 10000; //frequency (in Hz)

void setup()
{
  Serial.begin(9600);
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

  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

void loop()
{
  int adc0_val, adc1_val;

  adc0_val = analogRead(A0);
  adc1_val = analogRead(A1);

  /*
  Serial.print(adc0_val);
  Serial.print(" ");
  Serial.println(adc1_val);
  */
  
  // map the hour and minute onto PWM values
  tube1_pwm = map(adc0_val, 0, 1024, PWM_MIN, PWM_MAX);
  tube2_pwm = map(adc1_val, 0, 1024, PWM_MIN, PWM_MAX);
  
  // This function is called every loop but only does something when there has been a change.
  write_tubes(tube1_pwm, tube2_pwm);
}


/* 
  This function updates the time PWM values. It also includes a slew rate limit. It will only update the PWM duty cycle
  by a maximum of the SLEW_RATE. If the required change exceeds this then it will increment the duty cycle each loop round
  until the duty cycle matches the required.
*/
void write_tubes(int tube1, int tube2)
{
  bool change = false;

  if (tube1 > old_tube1_pwm)
  {
    old_tube1_pwm++;
    change |= true;
  }
  else if (tube1 < old_tube1_pwm)
  {
    old_tube1_pwm--;
    change |= true;
  }

  if (tube2 > old_tube2_pwm)
  {
    old_tube2_pwm++;
    change |= true;
  }
  else if (tube2 < old_tube2_pwm)
  {
    old_tube2_pwm--;
    change |= true;
  }

  if (change == true)
  {
    pwmWrite(TUBE1, old_tube1_pwm);
    pwmWrite(TUBE2, old_tube2_pwm);
    // To limit the rate of change the slew rate is controlled by adding a minimum wait time between repeated write calls
    delay(SLEW_RATE);
  }
}

