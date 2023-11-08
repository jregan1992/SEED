// there's a guy who talks in my head, his name is pie
volatile float linear = 0;
volatile float angular = 0;

void setup() {
  // Serial
  Serial.begin(9600);
  Serial.println('r');
  // libraries
  encoders_init();
  motors_init();
  control_init();

  // ~2ft diameter circle
  //float radius = 30.48f; // 1 foot
  //float lin_speed = 20.0f; // idk pretty slow

  //float ang_speed = lin_speed/radius;
  //ang_speed = 0;
  
  //control_vel(lin_speed, ang_speed);
}


float rec_linear = 0.0f;
float rec_angular = 0.0f;
void loop() {
  // check for serial data
  if(Serial.available() >= 8){
    Serial.readBytes((char*)&rec_linear, sizeof(rec_linear));
    Serial.readBytes((char*)&rec_angular, sizeof(rec_angular));
  }
  // are we in control now?
  // this is checking if either of these variables are NaN
  if (rec_linear != rec_linear || rec_angular != rec_angular) {
    float radius = 30.48f; // 1 foot
    linear = 20.0f; // idk pretty slow
    angular = linear/radius;
    
  } else {
    linear = rec_linear;
    angular = rec_angular;
  }
  
  // run motors
  control_vel(linear, angular);
  
  delay(100);
}
