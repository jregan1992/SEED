#include <Wire.h>

const int ARD_ADDR = 8; // I2C address of the Arduino
volatile int receivedValue = 0;
volatile int left_wheel = 0;
volatile int right_wheel = 0;

void setup() {
  Wire.begin(ARD_ADDR); // Initialize I2C communication as a slave
  Wire.onReceive(receiveEvent); // Set up the function to call when data is received
  Wire.onRequest(requestEvent); // Set up the function to call when data is requested
  Serial.begin(9600); // Initialize serial communication for debugging
  motors_init();
  encoders_init();
  control_init();
}

void loop() {



  

}

// Function to receive data from the Python script
void receiveEvent(int byteCount) {
  while (Wire.available()) {
    int offset = Wire.read();
    offset = Wire.read();
    left_wheel = Wire.read();
    right_wheel = Wire.read();
    
      if (left_wheel == 0)
    enable_motors(false);
  if (left_wheel == 1){
    enable_motors(true);
    set_vel_left(10);
  }
  if (left_wheel == 2){
    enable_motors(true);
    set_vel_left(20);
  }
  if (left_wheel == 3){
    enable_motors(true);
    set_vel_left(40);
  }
  

  if (right_wheel == 0)
    enable_motors(false);
  if (right_wheel == 1){
    enable_motors(true);
    set_vel_right(10);
  }
  if (right_wheel == 2){
    enable_motors(true);
    set_vel_right(20);
  }
  if (right_wheel == 3){
    enable_motors(true);
    set_vel_right(40);
  }

  delay(100);

  }
}

// Function to respond to data requests from the Python script
void requestEvent() {
}
