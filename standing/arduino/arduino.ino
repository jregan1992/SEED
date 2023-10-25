#include <Wire.h>

const int16_t ACCEL_ADDR = 0x68;

#define wasteatonoftime false

const double Kp = 50;

const double offset = 6;

void setup() {
  // Serial
  Serial.begin(57600);
  // libraries
  //encoders_init();
  motors_init();
  //control_init();

  // manually setting up and using the accelerometer for now
  Wire.begin();
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.write(0x6B);
  Wire.write(0); // Setting PWR_MGMT_1 to 0, wakes up accelerometer
  Wire.endTransmission(true);
}

// accelerometer data
int16_t accel_x = 0;
int16_t accel_y = 0;
int16_t accel_z = 0;
int16_t temp = 0;
int16_t gyro_x = 0;
int16_t gyro_y = 0;
int16_t gyro_z = 0;

void loop() {
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.write(0x3B); // start reading at reg 3B, accel X high bits
  Wire.endTransmission(false); // we're not done after one byte
  Wire.requestFrom(ACCEL_ADDR, 14, true); // get a total of 14 bytes

  // collect all the data
  accel_x = Wire.read() << 8 | Wire.read(); // each register is 16 bytes set as two bits
  accel_y = Wire.read() << 8 | Wire.read();
  accel_z = Wire.read() << 8 | Wire.read();
  temp = Wire.read() << 8 | Wire.read();
  gyro_x = Wire.read() << 8 | Wire.read();
  gyro_y = Wire.read() << 8 | Wire.read();
  gyro_z = Wire.read() << 8 | Wire.read();

  // show it
  if (wasteatonoftime) {
    Serial.print("aX: ");
    Serial.print(accel_x);
    Serial.print(", aY: ");
    Serial.print(accel_y);
    Serial.print(", aZ: ");
    Serial.print(accel_z);
    Serial.print(", T: ");
    Serial.print(temp/340.0 + 36.53);
    Serial.print(", gX: ");
    Serial.print(gyro_x);
    Serial.print(", gY: ");
    Serial.print(gyro_y);
    Serial.print(", gZ: ");
    Serial.print(gyro_z);
    Serial.println();
  }
  
  // for funsies
  accel_z = accel_z / 256;
  Serial.print("accZ: ");
  Serial.print(accel_z);
  
  // here's an idea: set PWM to the amount of g seen along the tilting axis?
  uint8_t pwm = max(0, min(255, Kp*abs(accel_z - offset)));
  //if (pwm < 30) pwm = 0;
  bool dir = (accel_z - offset) < 0;
  
  Serial.print("pwm: ");
  Serial.print(pwm);
  Serial.print(", dir: ");
  Serial.print(dir);
  Serial.println();
  
  
  motors_pwm(pwm);
  motors_dir(dir);
  
  delay(50);
}
