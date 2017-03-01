/* Control de dos servos para un sistema de dos grados de libertad
   Servo 1 - PIN 22
   Servo 2 - PIN 23
   Librería: https://www.arduino.cc/en/Reference/Servo
   Para Arduino mega: On the Mega, up to 12 servos can be used without interfering with PWM functionality; use of 12 to 23 motors will disable PWM on pins 11 and 12.
   Usado Servo MG995
*/

#include <Servo.h>

Servo pan_servo;  // create servo object to control a servo
Servo tilt_servo;  // create servo object to control a servo

int pos = 0;    // variable to store the servo position

void setup() {
  Serial.begin(9600);
  pan_servo.attach(44);  // attaches the servo pan on pin 44 to the servo object
  tilt_servo.attach(45);  // attaches the servo on pin 46 to the servo object

}

void loop() {
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    pan_servo.write(pos);              // tell servo to go to position in variable 'pos'
    tilt_servo.write(constrain(pos, 45, 135));
    delay(15);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    pan_servo.write(pos);              // tell servo to go to position in variable 'pos'
    tilt_servo.write(constrain(pos, 45, 135));
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
