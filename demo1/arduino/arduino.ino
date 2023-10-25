#include <Wire.h>

void setup() {
  // Serial
  Serial.begin(57600);
  // libraries
  encoders_init();
  motors_init();
  control_init();

  // given angle (rad, + is left) and distance(ft) here
  double ang = (PI/180.0d) * 360.0d;
  double dist = 0.0d * 30.48d;

  // send rotation command and give ample time
  if (ang != 0.0d) {
    control_angle(ang);
    //delay(15000);
  }
  
  // send distance command(s), how should we adress this?
  //control_pos_left(encoders_pos_left() + dist);
  //control_pos_right(encoders_pos_right() + dist);
}

void loop() {
  // do nothing, slowly
  delay(100);
  /*
  Serial.print("L igr: ");
  Serial.print(gimme_integrator());
  Serial.print(", L vel: ");
  Serial.print(encoders_vel_left());
  Serial.print(", L pos: ");
  Serial.print(encoders_pos_left());
  Serial.print(", R vel: ");
  Serial.print(encoders_vel_right());
  Serial.print(", R pos: ");
  Serial.print(encoders_pos_right());
  Serial.println();
  */
  
  Serial.print("L dpos: ");
  Serial.print(gimme_integrator());
  Serial.print(", L pos: ");
  Serial.print(encoders_pos_left());
  Serial.print(", phi: ");
  Serial.print(encoders_pos_phi());
  Serial.println();
  
}
