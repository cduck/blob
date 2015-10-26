#include "I2Cdev.h"

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

// No setup for I2C but should include if it doesn't work
void GSetup() {
  Wire.begin();
  
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

// Takes the desired motor positions from motorCalc and outputs an
// I2C command to the Grizzlys to spin the motors
void GLoop() {
   Wire.beginTransmission();
}

void sendPID(byte addr, float p, float i, float d) {
  
}

void sendCurrentLimit(byte addr, float lim) {
  
}

void sendPosition(byte addr, float) {
  
}

void sendCmd(byte addr, byte *data, byte dataLength) {
  Wire.beginTransmission(addr);
  Wire.write(data, dataLength);
  Wire.endTransmission();
}

