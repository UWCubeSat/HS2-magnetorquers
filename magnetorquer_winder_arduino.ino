/* HSL Magnetorquer Winder Testbench
 * Outputs pulses to stepper motor driver circuit.
 * Motor speed controlled by potentiometer and progress
 * printed on LCD display.
 */

#include <LiquidCrystal.h>
#include <math.h>

// parameters to define
const int stepsPerRevolution = 200;  // motor parameter
const int stepRatio = 3; // number of rotational steps to horizontal steps
const long totalSteps = 390400;  // total number of rotational steps
const long stepsPerLayer = 8000;  // horizontal steps before reversing direction

// setup
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;  // setup lcd display
const int rotStep = 10;
const int horizStep = 9;
const int dir = 6;
const int motorOn = 13;

long steps = 0;
long layerSteps = 0;

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// reads potentiometer input and converts to motor speed
int speed() {
  int potInput = analogRead(A0);
  return (int) (3000 - (2800/1023)*potInput);
}

int delayValue;

void setup() {

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  digitalWrite(7, HIGH);
  lcd.print("Progress:");

  // set up 'PWM' output to stepper driver
  pinMode(rotStep,OUTPUT);  // rotational (main) step
  pinMode(horizStep,OUTPUT);  // horizontal (geared) step
  pinMode(dir,OUTPUT); // shared direction
  
  pinMode(motorOn,INPUT); // switch to activate winding

  digitalWrite(dir,HIGH); // may need to change
}


void loop() {
  
  // at end of solenoid, reverse direction of horizontal stepper motor
  if (layerSteps >= stepsPerLayer) {
    layerSteps = 0;
    digitalWrite(dir, !digitalRead(dir));
  }

  // fetch speed from potentiometer input
  delayValue = speed();
  
  if ((steps < totalSteps) && (digitalRead(motorOn) == HIGH)) {
    // <stepRatio> rotational steps
    for (int i = 0; i < stepRatio; i++) {
      digitalWrite(rotStep,HIGH);
      delayMicroseconds(delayValue);
      digitalWrite(rotStep,LOW);
      delayMicroseconds(delayValue);
    }
  
    // one horizontal / translational step
    digitalWrite(horizStep,HIGH);
    delayMicroseconds(delayValue);
    digitalWrite(rotStep,LOW);
    delayMicroseconds(delayValue);
    
    layerSteps++;
    steps += stepRatio;
  }

  lcd.setCursor(0, 1);
  float progress = 100.0*steps / totalSteps;
  lcd.print(progress);
  lcd.write("%");

  // display motor direction (f or r)
  lcd.setCursor(15,0);
  if (digitalRead(dir)) {
    lcd.write("F");
  } else {
    lcd.write("R");
  }
  
}
