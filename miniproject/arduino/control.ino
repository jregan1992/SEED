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

// desired positions
int32_t left_dpos = 0;
int32_t right_dpos = 0;
// current PWM and dir settings
volatile uint8_t left_pwm = 0;
volatile bool left_dir = false;
volatile uint8_t right_pwm = 0;
volatile bool right_dir = false;
// control loop, this is called by an interrupt
void control_loop() {
  int32_t l_error = left_dpos - get_pos_left();
  left_pwm = max(0, min(255, Kp*abs(l_error)));
  left_dir = l_error < 0;
  set_dir_left(left_dir);
  set_pwm_left(left_pwm);

  int32_t r_error = right_dpos - get_pos_right();
  right_pwm = max(0, min(255, Kp*abs(r_error)));
  right_dir = r_error < 0;
  set_dir_right(right_dir);
  set_pwm_right(right_pwm);
}
// getters for current PWM and dir i guess
uint8_t get_pwm_left() { return left_pwm; }
uint8_t get_pwm_right() { return right_pwm; }
bool get_dir_left() { return left_dir; }
bool get_dir_right() { return right_dir; }


// set position, encoder counts
void set_pos(int32_t p) {
  set_pos_left(p);
  set_pos_right(p); 
}
void set_pos_left(int32_t p) { left_dpos = p; }
void set_pos_right(int32_t p) { right_dpos = p; }

// ISR for control loop
ISR(TIMER2_COMPB_vect) {
  if (inited) control_loop();
}
