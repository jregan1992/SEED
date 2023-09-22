/**
 * EENG 350 - Control Library
 * Tim Puls
 * 
 * Uses the encoder and motor libraries to provide velocity control
 */

// tuneable parameters
const double Kp = 1;


// init control
bool inited = false;
void control_init() {
  // encoders already sets up timer2, let's use the COMPB interrupt now
  OCR2A = 255; // COMPB interrupt when timer = 255, period should be ~16 ms
  TIMSK2 |= B00000100; // enable COMPB interrupt
  inited = true;
}

// desired velocities
double left_vel = 0;
double right_vel = 0;
// current PWM settings
volatile uint8_t left_pwm = 0;
volatile uint8_t right_pwm = 0;
// control loop, this is called by an interrupt
void control_loop() {
  // instead of the broken P controller we're given i'm doing an I controller i think
  double l_error = abs(left_vel) - abs(get_vel_left()); // ignore sign when finding velocity error
  left_pwm = max(0, min(255, left_pwm + Kp*l_error)); // pwm' = pwm + Kp*error, with bounding
  set_dir_left(left_vel < 0); // sign matters for direction tho
  set_pwm_left(left_pwm);

  double r_error = abs(right_vel) - abs(get_vel_right());
  right_pwm = max(0, min(255, right_pwm + Kp*r_error));
  set_dir_right(right_vel < 0);
  set_pwm_right(right_pwm);
}
// getters for current PWM and dir i guess
uint8_t get_pwm_left() { return left_pwm; }
uint8_t get_pwm_right() { return right_pwm; }
bool get_dir_left() { return left_vel < 0; }
bool get_dir_right() { return right_vel < 0; }


// set velocity, cm/s
void set_vel(double v) {
  set_vel_left(v);
  set_vel_right(v); 
}
void set_vel_left(double v) { left_vel = v; }
void set_vel_right(double v) { right_vel = v; }

// ISR for control loop
ISR(TIMER2_COMPB_vect) {
  if (inited) control_loop();
}
