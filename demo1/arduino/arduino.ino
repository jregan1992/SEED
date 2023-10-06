#include <Wire.h>

void setup() {
  // Serial
  Serial.begin(9600);
  // libraries
  encoders_init();
  motors_init();
  control_init();
  // and run to 0
  control_pos(0);
}

void loop() {
  // dance
  control_angle(-0.785);
  delay(1500);
  control_angle(0.785);
  delay(1500);
  control_angle(0.785);
  delay(1500);
  control_angle(-0.785);
  delay(1500);

  control_angle(3.927);
  delay(2500);
  control_angle(-1.571);
  delay(2000);
  control_angle(3.927);
  delay(2500);

  control_angle(6.28);
  delay(2500);
  control_angle(6.28);
  delay(2500);

  sit_stand();

  control_pos_left(500);
  delay(1000);
  control_pos_left(0);
  delay(1000);

  sit_stand();

  control_pos_right(500);
  delay(1000);
  control_pos_right(0);
  delay(1000);

  sit_stand();

  control_angle(-0.785);
  delay(1000);
  control_pos(200);
  delay(1000);
  control_pos(0);
  delay(1000);
  control_angle(0.785);
  delay(1000);

  sit_stand();
  
  control_angle(0.785);
  delay(1000);
  control_pos(200);
  delay(1000);
  control_pos(0);
  delay(1000);
  control_angle(-0.785);
  delay(1000);
}


// dance steps
void sit_stand() {
  control_pos(400);
  delay(1000);
  control_pos(0);
  delay(1000);
  control_pos(400);
  delay(1000);
  control_pos(0);
  delay(1000);
}
