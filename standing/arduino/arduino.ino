#include <Kalman.h>

#include <Wire.h>

const int16_t ACCEL_ADDR = 0x68;
// accelerometer data
int16_t accel_x = 0;
int16_t accel_y = 0;
int16_t accel_z = 0;
int16_t temp = 0;
int16_t gyro_x = 0;
int16_t gyro_y = 0;
int16_t gyro_z = 0;

#define wasteatonoftime false

#define WHEEL_CIRC 47.1 // wheel circumference, centimeters
#define CPR 3200 // encoder counts per wheel rotation

const float ACCEL_ANGLE_ZERO = 9.5f;

const float THE_CONSTANT = -100.0f;

const float DIST_TO_COM = 15.0f;

Kalman k;
float a = 0.0;
uint32_t t_last = 0;

void setup() {
  // Serial
  Serial.begin(57600);
  // libraries
  encoders_init();
  motors_init();
  control_init();

  // manually setting up and using the accelerometer for now
  Wire.begin();
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.write(0x6B);
  Wire.write(0); // Setting PWR_MGMT_1 to 0, wakes up accelerometer
  Wire.endTransmission(true);
  // set Gyro range
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.write(0x1B);
  Wire.write(0b00000000); // FSR of +/-250deg/s
  Wire.endTransmission(true);
  // set DLPF
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.write(0x1A);
  Wire.write(0b00000100);
  Wire.endTransmission(true);
  // set accelerometer range
  Wire.beginTransmission(ACCEL_ADDR);
  Wire.write(0x1C);
  Wire.write(0b00010000); // FSR of +/-8g
  Wire.endTransmission(true);

  // get a starting angle for the kalman filter
  read_mpu();
  float startAngle = ACCEL_ANGLE_ZERO - (atan2(accel_z, accel_y) * 180.0f/PI);
  k.setAngle(startAngle);
}


uint32_t last_calc = 0;
float current_angle = 0.0f;
float set_speed = 0.0f;
float last_speed = 0.0f;
float COR_accel = 0.0f;


void loop() {
  read_mpu();

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
  /*
  // integrate the gyro value
  uint32_t this_calc = millis();
  if (gyro_x > 150 || gyro_x < -150) {
    current_angle += gyro_x * (250.0f/32767.0f) * (this_calc - last_calc)/1000.0f;
  } else {
    gyro_x = 0;
  }
  last_calc = this_calc;
  //Serial.print(gyro_x);
  //Serial.print(", ");
  //Serial.println(current_angle);

  // add in accelerometer measurement
  float current_angle_ACCEL = ACCEL_ANGLE_ZERO - (atan2(COR_accel + (accel_z*980.0f*8.0f/32767.0f), accel_y*980.0f*8.0f/32767.0f) * 180.0f/PI);
  //Serial.println(current_angle_ACCEL);
  //Serial.println(accel_z*980.0f*8.0f/32767.0f);

  // merge into one angle
  current_angle = current_angle*0.99f + current_angle_ACCEL*0.01f; 
  //Serial.println(current_angle);
  //Serial.print(", ");*/

  /*
  // calculate COR speed
  float curr_speed = encoders_vel_left() + gyro_x*(250.0f/32767.0f)*MAGIC1_gyrotolinear;
  // keep track of COR acceleration
  COR_accel = curr_speed - last_speed;
  last_speed = curr_speed;
  // since we want to not be moving any speed is error
  float err_speed = 0.0f - curr_speed;
  // calculate a goal angle?
  float goal_angle = err_speed * MAGIC2_speedtoangle;
  // and the error in that?
  float err_angle = goal_angle - current_angle;
  // and then adjust set speed
  set_speed = set_speed + err_angle * MAGIC3_angletospeed;
  */

  
  // plug numbers into kalman filter
  float accelAngle = ACCEL_ANGLE_ZERO - (atan2(accel_z, accel_y) * 180.0f/PI);
  // update filter
  uint32_t t_this = millis();
  a = k.getAngle(accelAngle, gyro_x/131.0f, (t_this - t_last)/1000.0f);
  t_last = t_this;

  // estimate horizontal distance from wheels to COM
  float dist = -1*DIST_TO_COM*sin(a*(PI/180.0f));
  //Serial.println(dist);

  int32_t newpos_l = encoders_pos_left() + dist*(CPR/WHEEL_CIRC);
  int32_t newpos_r = encoders_pos_right() + dist*(CPR/WHEEL_CIRC);
  //Serial.println(newpos);
  
  control_pos_left(newpos_l);
  control_pos_right(newpos_r);
  
  //set_speed += THE_CONSTANT*sin(a*(PI/180.0f));
  //set_speed = constrain(set_speed, -255, 255);

  //Serial.println(set_speed);

  //motors_pwm(abs(set_speed));
  //motors_dir(set_speed < 0);
  
  /*Serial.print(curr_speed);
  Serial.print(", ");
  Serial.print(goal_angle);
  Serial.print(", ");
  Serial.println(set_speed);*/
    
  //control_vel(set_speed);
  
  //delay(20);
}

void read_mpu() {
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
}
