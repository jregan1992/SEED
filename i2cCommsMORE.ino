#include <Wire.h>

const int ARD_ADDR = 8; // I2C address of the Arduino
int receivedValue = 0;

void setup() {
  Wire.begin(ARD_ADDR); // Initialize I2C communication as a slave
  Wire.onReceive(receiveEvent); // Set up the function to call when data is received
  Wire.onRequest(requestEvent); // Set up the function to call when data is requested
  Serial.begin(9600); // Initialize serial communication for debugging
}

void loop() {
  // Your main Arduino loop code here
}

// Function to receive data from the Python script
void receiveEvent(int byteCount) {
  if (byteCount == 1) { // Check if only one byte is received
    receivedValue = Wire.read(); // Read the received byte
  }
}

// Function to respond to data requests from the Python script
void requestEvent() {
  // Respond with the received value
  Wire.write(receivedValue+100);
}
