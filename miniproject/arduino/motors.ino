/**
 * EENG 350 - Motor Library
 * Tim Puls
 * 
 * Wraps motor pins in functions
 */

// motor pins, annotated
// nD2 engages the motors when high, when low the motors will coast
const uint8_t nD2 = 4;
// MxDIR sets motor direction, high for reverse
const uint8_t M1DIR = 7;
const uint8_t M2DIR = 8;
// MxPWM is the PWM input for motor speed, high is on
const uint8_t M1PWM = 9;
const uint8_t M2PWM = 10;
// nSF is for diagnostics, goes low when a fault occurs
const uint8_t nSF = 12;
// MxFB is for reading the current draw on the motors
const uint8_t M1FB = A0;
const uint8_t M2FB = A1;
// M1 is left, M2 is right


// init motors
void motors_init() {
  // set pin modes
  pinMode(nD2, OUTPUT);
  pinMode(M1DIR, OUTPUT);
  pinMode(M2DIR, OUTPUT);
  pinMode(M1PWM, OUTPUT);
  pinMode(M2PWM, OUTPUT);
  pinMode(nSF, INPUT);
  pinMode(M1FB, INPUT);
  pinMode(M2FB, INPUT);
  // turn on the motors
  enable_motors(true);
  // default to forward
  set_dir(false);
  set_pwm(0);
}

// set motor enable pin, false means motors coast
void motors_enable(bool enabled) {
  digitalWrite(nD2, enabled);
}

// set motor directions, true is reverse
void set_dir(bool reverse) {
  set_dir_left(reverse);
  set_dir_right(reverse);
}
void set_dir_left(bool reverse) {
  digitalWrite(M1DIR, reverse);
}
void set_dir_right(bool reverse) {
  digitalWrite(M2DIR, reverse);
}

// set motor PWM, high means motor is on
void set_pwm(uint8_t duty) {
  set_pwm_left(duty);
  set_pwm_right(duty);
}
void set_pwm_left(uint8_t duty) {
  analogWrite(M1PWM, duty);
}
void set_pwm_right(uint8_t duty) {
  analogWrite(M2PWM, duty);
}
