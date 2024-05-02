#include <Servo.h>
 
//Variable Declaration Part
//Joysticks are analog meaning they can have a range of values
//JS = joystick, H = Horizontal, V = vertical, A0-3 is pin it is connected to
 
 
// Joystick shield is connected to A0-A3
const uint8_t LEFT_JS_H = A0;   // Pin for Left Joystick Horizontal Pin is on the HERO board
const uint8_t LEFT_JS_V = A1;   // Pin for Left Joystick Vertical
const uint8_t RIGHT_JS_H = A3;  // Pin for Right Joystick Horizontal
const uint8_t RIGHT_JS_V = A2;  // Pin for Right Joystick Vertical
 
const uint8_t LEFT_JS_BUTTON = 2;   // Pin for Left Joystick Button
const uint8_t RIGHT_JS_BUTTON = 4;  // Pin for Right Joystick Button
 
// Instantiate Servo objects for base, horizontal, vertical and the claw

Servo base;
Servo horizontal;
Servo vertical;
Servo claw;
 
struct Servo_info {
  Servo servo_object;
  uint8_t servo_pin;
  uint8_t servo_min;
  uint8_t servo_max;
  uint8_t joystick_pin;
};
 
Servo_info servos [] = {
  {base,       5,  0, 180, LEFT_JS_H}, //base connected to pin 5 on the controller, min value of 0, max value 180, controlled by LEFT JS Horizontal
  {horizontal, 9, 52, 180, LEFT_JS_V},
  {vertical,  10, 94, 180, RIGHT_JS_V},
  {claw,      11, 60,  89, RIGHT_JS_H}  
};
const int8_t SERVO_COUNT = sizeof(servos) / sizeof(Servo_info); //added in case we want to add more Servos, currently we have 4 (28/7)
																//made this an equation so it is more dynamic rather than hardcoding 4
 
//Reads which way the joystick is getting pushed, either positive or negative on the axis 
//Will be called later in the loop
int8_t joystick_read(uint8_t servo_number) {
  uint16_t value = analogRead(servos[servo_number].joystick_pin);
  if (value < 250) return 1;  // Increase for UP joystick 
							  //If it senses the joystick is getting pushed above plot 250 then it will read as 1 for being pushed up
  if (value > 750) return -1; // Decrease for DOWN joystick
							  //If it senses the joystick is getting pushed below 750 on the axis then it will read as -1 and push it down
  return 0;   // default to NO movement
}


void setup() {
  Serial.begin(115200);
  pinMode(LEFT_JS_BUTTON, INPUT_PULLUP);
  pinMode(RIGHT_JS_BUTTON, INPUT_PULLUP);
 
 //For loop that runs through and maps each pin to the joystick when moved
 //Resets to middle of the range each time it restarts
  for (int i = 0; i < SERVO_COUNT; i++) {
    Servo_info servo = servos[i];
    servo.servo_object.attach(servo.servo_pin);
    uint8_t mid_range = servo.servo_min + (servo.servo_max - servo.servo_min) / 2;
    servo.servo_object.write(mid_range);
  }
  delay(200);
}

// 
void loop() {
  for (int i = 0; i < sizeof(servos) / sizeof(Servo_info); i++) { //loops 4 times for the 4 servos
    if (int8_t move_direction = joystick_read(i)) { //reads the analog value, senses which joystick is being moved and where it is moved and then maps to which part the pin and joystick are assigned to
      Serial.print(i);
      Serial.print(": direction: ");
      Serial.println(move_direction);
 
      // Move in indicated direction unless move would go outside MIN/MAX range
      uint8_t servo_position = servos[i].servo_object.read();
      Serial.print("at: ");
      Serial.println(servo_position);
      if ((servo_position <= servos[i].servo_min && move_direction == -1) || //If you're on the min or max value and still pushing on the joystick, it will stop moving
          (servo_position >= servos[i].servo_max && move_direction == 1))
        move_direction = 0;
      if (move_direction) { //if you are not in the max or min position, this will print what position you are moving the servo to 
        Serial.print("move to: ");
        Serial.println(servo_position + move_direction);
        servos[i].servo_object.write(servo_position + move_direction);
      }
    }
  }
  delay(50);
}