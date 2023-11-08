// the arduino forums suck
const uint32_t inf_b = 0b01111111100000000000000000000000;
const float infinity = *(float*)&inf_b;

// there's a guy who talks in my head, his name is pie
volatile float linear = 0;
volatile float angular = 0;

void setup() {
  // Serial
  Serial.begin(9600);
  // libraries
  encoders_init();
  motors_init();
  control_init();
  // reset pi?
  Serial.println('r');
}

float rec_linear = 0.0f;
float rec_angular = 0.0f;
bool nanded = false;
bool becometangent = false;

bool didaflip = false;
float ang_target = 0.0f;

void loop() {
  // check for serial data
  if(Serial.available() >= 8){
    Serial.readBytes((char*)&rec_linear, sizeof(rec_linear));
    Serial.readBytes((char*)&rec_angular, sizeof(rec_angular));
  }
  // are we in control now?
  // this is checking if either of these variables are NaN
  if (rec_linear == infinity || rec_angular == infinity || nanded) {
    nanded = true;
    
    // """"state machine""""
    if (!didaflip) {
      // turn 45 degrees
      control_angle(PI/4);
      //Serial.println("flippin");
      delay(2500); // tune this?
      didaflip = true;
      
    } else if (!becometangent) {
      // scoot forward
      control_pos_right(encoders_pos_right() + 45.0f);
      control_pos_left(encoders_pos_left() + 45.0f);
      delay(1500);
      becometangent = true;
      
      // where should we point
      //Serial.println(encoders_pos_phi());
      ang_target = encoders_pos_phi() - 2.0f*PI; 
      ang_target += 0.3f + 1.0f; // it overshoots a little
      //Serial.println(ang_target);
    } else if (encoders_pos_phi() > ang_target) {
      // ~2ft diameter circle
      //Serial.println(encoders_pos_phi());
      float radius = 1.2f*30.48f; // 1 foot
      linear = 40.0f; // idk pretty slow
      angular = -1*linear/radius; // negative for CCW
    } else {
      // done!!!!!!
      linear = 0.0f;
      angular = 0.0f;
    }
  } else {
    linear = rec_linear;
    angular = rec_angular;
  }
  
  // run motors
  control_vel(linear, angular);
  
  delay(10);
}
