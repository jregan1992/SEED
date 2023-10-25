/**
 * EENG 350 - Control Library
 * Tim Puls
 * 
 * Uses the encoder and motor libraries to provide velocity control
 */

// tuneable parameters
// tuned via a sign from god
const double Kp_vel = 2;
const double Kp_pos = 0.5;
const double Ki_pos = 1;

// important constants
#define ROBOT_WIDTH 37.5 // centimeters
#define WHEEL_CIRC 47.1 // wheel circumference, centimeters
#define CPR 3200 // encoder counts per wheel rotation

#define MAX_PWM 100
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
volatile double left_dpos = 0;
volatile double right_dpos = 0;
// desired velocities
volatile double left_dvel = 0;
volatile double right_dvel = 0;
// current PWM and dir settings
volatile uint8_t left_pwm = 0;
volatile bool left_dir = false;
volatile uint8_t right_pwm = 0;
volatile bool right_dir = false;
// integrator
volatile double left_integral = 0;
volatile double right_integral = 0;
volatile uint32_t last_millis = 0;
// control loop, this is called by an interrupt
void _control_loop() {
  double p_error = 0;
  double v_error = 0;
  double dt = (millis() - last_millis)/1000.0d;
  last_millis = millis();
  // this is the implementation from the tutorial doc
  // my prediction is that it won't work at all

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

double gimme_integrator() { return left_dpos; }

// ask for a relative positional change, cm
void control_pos(double p) {
  control_pos_left(p);
  control_pos_right(p); 
}
void control_pos_left(double p) { left_dpos = p; left_mode = true; }
void control_pos_right(double p) { right_dpos = p; right_mode = true; }

// set velocity, cm/s
void control_vel(double v) {
  control_vel_left(v);
  control_vel_right(v); 
}
void control_vel_left(double v) { left_dvel = v; left_mode = false; }
void control_vel_right(double v) { right_dvel = v; right_mode = false; }

// ask for a relative angular change, radians
void control_angle(double a) {
  double displacement = (ROBOT_WIDTH * a / 2.0);
  control_pos_left(-displacement);
  control_pos_right(displacement);
}

// ISR for control loop
ISR(TIMER2_COMPB_vect) {
  if (control_inited) _control_loop();
}
