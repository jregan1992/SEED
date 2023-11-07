/**
 * EENG 350 - Control Library
 * Tim Puls
 * 
 * Uses the encoder and motor libraries to provide velocity control
 */

// tuneable parameters
// tuned via a sign from god
const float Kp_vel = 2;
const float Kp_pos = 0.5;
const float Ki_pos = 1;

// important constants
#define ROBOT_WIDTH 38.0 // centimeters
#define WHEEL_CIRC 47.2 // wheel circumference, centimeters
#define CPR 3200 // encoder counts per wheel rotation

#define MAX_PWM 150
#define MAX_INTEGRAL 1


// init control
bool control_inited = false;
void control_init() {
  control_inited = true;
  // encoders already sets up timer2, let's use the COMPB interrupt now
  OCR2A = 255; // COMPB interrupt when timer = 255, period should be ~16 ms
  TIMSK2 |= B00000100; // enable COMPB interruptx
}

// control mode
volatile bool left_mode = false; // true for position mode i guess
volatile bool right_mode = false;
// desired positions
volatile float left_dpos = 0;
volatile float right_dpos = 0;
// desired velocities
volatile float left_dvel = 0;
volatile float right_dvel = 0;
// current PWM and dir settings
volatile uint8_t left_pwm = 0;
volatile bool left_dir = false;
volatile uint8_t right_pwm = 0;
volatile bool right_dir = false;
// integrator
volatile float left_integral = 0;
volatile float right_integral = 0;
volatile uint32_t last_millis = 0;
// control loop, this is called by an interrupt
void _control_loop() {
  float p_error = 0;
  float v_error = 0;
  float dt = (millis() - last_millis)/1000.0f;
  last_millis = millis();
  // this was the implementation from the tutorial doc
  // my prediction is that it won't work at all, i was right
  // this line: pwm = Kp_vel*vel_error;   DOES NOT WORK
  // the real motors cannot have PWM of zero since that will make them brake and completely stop

  // mode switch
  if (left_mode) {
    // positional mode
    p_error = left_dpos - encoders_pos_left();
    left_integral = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, left_integral + p_error*dt));
    left_dvel = Kp_pos*p_error + Ki_pos*left_integral;
  }
  // velocity mode
  v_error = abs(left_dvel) - abs(encoders_vel_left());
  left_pwm = max(0, min(MAX_PWM, left_pwm + Kp_vel*v_error));
  left_dir = left_dvel < 0;

  // again on the right
  if (right_mode) {
    // positional mode
    p_error = right_dpos - encoders_pos_right();
    right_integral = max(-MAX_INTEGRAL, min(MAX_INTEGRAL, right_integral + p_error*dt));
    right_dvel = Kp_pos*p_error + Ki_pos*right_integral;
  }
  // velocity mode
  v_error = abs(right_dvel) - abs(encoders_vel_right());
  right_pwm = max(0, min(MAX_PWM, right_pwm + Kp_vel*v_error));
  right_dir = right_dvel < 0;

  // set motors
  motors_pwm_left(left_pwm);
  motors_dir_left(left_dir);
  motors_pwm_right(right_pwm);
  motors_dir_right(right_dir);
}
// getters for current PWM and dir i guess
uint8_t control_pwm_left() { return left_pwm; }
uint8_t control_pwm_right() { return right_pwm; }
bool control_dir_left() { return left_dir; }
bool control_dir_right() { return right_dir; }

float gimme_integrator() { return left_dpos; }

// set a specific encoder position, cm
void control_pos(float p) {
  control_pos_left(p);
  control_pos_right(p); 
}
void control_pos_left(float p) { left_dpos = p; left_mode = true; }
void control_pos_right(float p) { right_dpos = p; right_mode = true; }

// set velocity, cm/s and rad/s
void control_vel(float v_linear, float v_angular) {
  // calculate net linear speeds for both wheels
  control_vel_left(v_linear + (ROBOT_WIDTH/2.0f)*v_angular);
  control_vel_right(v_linear - (ROBOT_WIDTH/2.0f)*v_angular); 
}
void control_vel_left(float v) { left_dvel = v; left_mode = false; }
void control_vel_right(float v) { right_dvel = v; right_mode = false; }


// ISR for control loop
ISR(TIMER2_COMPB_vect) {
  if (control_inited) _control_loop();
}
