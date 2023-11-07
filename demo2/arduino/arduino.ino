#include <Wire.h>

// I2C bus
const uint8_t ADDR = 8;
volatile float linear = 0;
volatile float angular = 0;

void setup() {
  // Serial
  Serial.begin(57600);
  // I2C
  Wire.begin(ADDR);
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  // libraries
  encoders_init();
  motors_init();
  control_init();

  // ~2ft diameter circle
  //float radius = 30.48f; // 1 foot
  //float lin_speed = 20.0f; // idk pretty slow

  //float ang_speed = lin_speed/radius;
  //ang_speed = 0;
  
  //control_vel(lin_speed, ang_speed);
}

void loop() {
  Serial.print(linear);
  Serial.print(", ");
  Serial.println(angular);
  delay(1000);
}

void receiveEvent(int count) {
  // we expect to see 8 bytes from the pi
  if (count == 8+2) {
    // throw out the trash
    Wire.read(); Wire.read();   
    // the first 4 bytes are the linear speed as a float, LSB first
    uint32_t raw = (uint32_t)Wire.read() | (uint32_t)Wire.read() << 8 | (uint32_t)Wire.read() << 16 | (uint32_t)Wire.read() << 24;
    linear = *(float*)&raw;
    // the last 4 bytes are the angular speed as a float, LSB first
    raw = (uint32_t)Wire.read() | (uint32_t)Wire.read() << 8 | (uint32_t)Wire.read() << 16 | (uint32_t)Wire.read() << 24;
    angular = *(float*)&raw;
  }
}

void requestEvent() {
  Serial.println("!");
  // give the current heading
  float h = encoders_pos_phi();
  uint32_t crimes = *(uint32_t*)&h;
  Wire.write((uint8_t) ((crimes & 0x000F) >> 0));
  Wire.write((uint8_t) ((crimes & 0x00F0) >> 8));
  Wire.write((uint8_t) ((crimes & 0x0F00) >> 16));
  Wire.write((uint8_t) ((crimes & 0xF000) >> 24));
}
