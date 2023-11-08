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
  if (rec_linear != rec_linear || rec_angular != rec_angular || nanded) {
    nanded = true;
    // """"state machine""""
    if (!didaflip) {
      // turn 90 degrees
      control_angle(PI/2);
      //Serial.println("flippin");
      delay(5000); // tune this?
      didaflip = true;
      
      // where should we point
      //Serial.println(encoders_pos_phi());
      ang_target = encoders_pos_phi() - 2.0f*PI;
      //Serial.println(ang_target);
    } else if (encoders_pos_phi() > ang_target) {
      // ~2ft diameter circle
      //Serial.println(encoders_pos_phi());
      float radius = 30.48f; // 1 foot
      linear = 20.0f; // idk pretty slow
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
  
  delay(100);
}
