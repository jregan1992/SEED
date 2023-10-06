#include <Wire.h>

// I2C bus
const uint8_t ADDR = 8;
volatile uint8_t left_angle = 0;
volatile uint8_t right_angle = 0;

void setup() {
  // Serial
  Serial.begin(9600);
  // I2C
  Wire.begin(ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  // libraries
  encoders_init();
  motors_init();
  control_init();
  // and run to 0
  control_pos(0);
}

void loop() {
  // put your main code here, to run repeatedly:
  // do nothing slowly
  Serial.println(encoders_pos_right());
  delay(1000);
}

// called when the arduino recieves an I2C message,
// in our case this is the wheel position
void receiveEvent(uint8_t count) {
  // we expect to see 4 bytes from the pi
  if (count == 4) {
    // the first 2 bytes are garbage for some reason
    Wire.read(); 
    Wire.read();
    // next byte is left wheel
    left_angle = Wire.read();
    // lastly we get right wheel
    right_angle = Wire.read();

    // set desired positions
    control_pos_left(left_angle * (3200/4));
    control_pos_right(right_angle * (3200/4));
  }
}

// called when data is requested from the arduino,
// in our case this happens regularly to get current positions
void requestEvent() {
  // idk tell them the left wheel pos
  Wire.write(encoders_pos_right()/1600);
}
