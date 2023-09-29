/**
 * EENG 350 - Encoder Library
 * Tim Puls
 * 
 * Manages reading encoders and calculating robot position/velocity
 */

// encoder pins
const uint8_t left_A = 2;
const uint8_t left_B = 5;
const uint8_t right_A = 3;
const uint8_t right_B = 6;

// important constants
#define ROBOT_WIDTH 37.5 // centimeters
#define WHEEL_CIRC 47.1 // wheel circumference, centimeters
#define CPR 3200 // encoder counts per wheel rotation


// inititalize encoders, call from setup()
bool inited = false;
void encoders_init() {
  inited = true;
  // set pin modes
  pinMode(left_A, INPUT_PULLUP);
  pinMode(left_B, INPUT_PULLUP);
  pinMode(right_A, INPUT_PULLUP);
  pinMode(right_B, INPUT_PULLUP);
  // register interrupts
  attachInterrupt(digitalPinToInterrupt(left_A), _left_ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(right_A), _right_ISR, CHANGE);
  // timer interrupt for keeping track of position
  // set up timer2
  TCCR2A = 0;
  TCCR2B = 0;
  TCCR2B |= B00000111; // 1024x prescaler
  OCR2A = 255; // COMPA interrupt when timer = 255, period should be ~16 ms
  TIMSK2 |= B00000010; // enable COMPA interrupt
}

// left encoder state
volatile bool left_sA = false;
volatile bool left_sB = false;
volatile int32_t left_pos = 0;
// ISR for left encoder
void _left_ISR() {
  // read encoder state
  bool n_sA = digitalRead(left_A);
  bool n_sB = digitalRead(left_B);
  // calculate positional change 
  // if A and B are the same
  if (n_sA == n_sB) {
    // we moved backwards
    left_pos -= 2;
  } else {
    // not same, we moved forwards
    left_pos += 2;
  }
  // update state
  left_sA = n_sA;
  left_sB = n_sB;
}
// getter
int32_t encoders_pos_left() { return left_pos; }

// right encoder state
volatile bool right_sA = false;
volatile bool right_sB = false;
volatile int32_t right_pos = 0; 
// ISR for right encoder
void _right_ISR() {
  // read encoder state
  bool n_sA = digitalRead(right_A);
  bool n_sB = digitalRead(right_B);
  // calculate positional change 
  // if A and B are the same
  if (n_sA == n_sB) {
    // we moved forwards (right motor is mounted in reverse)
    right_pos += 2;
  } else {
    // not same, we moved backwards
    right_pos -= 2;
  }
  // update state
  right_sA = n_sA;
  right_sB = n_sB;
}
// getter
int32_t encoders_pos_right() { return right_pos; }

// current position/velocity
volatile double pos_x = 0; // centimeters
volatile double pos_y = 0; // centimeters
volatile double pos_phi = 0; // radians
volatile double vel_left = 0; // cm/s
volatile double vel_right = 0; // cm/s
// keeping track of things
volatile int32_t last_left = 0;
volatile int32_t last_right = 0;
volatile uint32_t last_time = 0;
// this runs on a timer ISR to update position
void _update_pos() {
  // convert encoder counts to meters
  double del_left_m = (left_pos - last_left) * (wheel_circ / cpr);
  double del_right_m = (right_pos - last_right) * (wheel_circ / cpr);
  // update pos
  pos_x = pos_x + cos(pos_phi)*(del_left_m + del_right_m)/2.0d;
  pos_y = pos_y + sin(pos_phi)*(del_left_m + del_right_m)/2.0d;
  pos_phi = pos_phi + (del_left_m - del_right_m)/robot_width;
  // update last
  last_left = left_pos;
  last_right = right_pos;
  // update vel
  uint32_t new_time = millis();
  vel_left = 1000*del_left_m/(new_time - last_time);
  vel_right = 1000*del_right_m/(new_time - last_time);
  // update last
  last_time = new_time;
}
// getters for position
double encoders_pos_x() { return pos_x; }
double encoders_pos_y() { return pos_y; }
double encoders_pos_phi() { return pos_phi; }
// getters for velocity
double encoders_vel_left() { return vel_left; }
double encoders_vel_right() { return vel_right; }

// register update ISR
ISR(TIMER2_COMPA_vect) {
  if (inited) _update_pos();
}
