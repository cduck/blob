#include "I2Cdev.h"
#include "Wire.h"

#define MOTOR1 0x0A
#define MOTOR2 0x0B
#define MOTOR3 0x0C
#define MOTOR4 0x0D

const byte MOTORS[] = {MOTOR1,MOTOR2,MOTOR3,MOTOR4};

float lim = 3;
float p = .5;
float i = 0;
float d = 0;
// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

// No setup for I2C but should include if it doesn't work
void setup() {
  Wire.begin();
  Serial.begin(9600);
//  
//  Serial.println("1");
//  Wire.beginTransmission(0x0A);
//  
//  Serial.println("2");
//  byte data[] = {1,3,0,0,0,0,100,0,0};
//  
//  Serial.println("3");
//  Wire.write(data, sizeof(data));
//  
//  Serial.println("4");
//  Wire.endTransmission();
//  
//  Serial.println("5");
 
  for (int i = 0; i < 4; i++) {
    sendMode(MOTORS[i], 0x3); //0x3 for raw speed, 0x5 for pid speed, 0x7 for pid pos
    sendCurrentLimit(MOTORS[i], lim);
    sendPID(MOTORS[i], p, i, d);
  }  
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

// Takes the desired motor positions from motorCalc and outputs an
// I2C command to the Grizzlys to spin the motors
void loop() {
  sendPosition(MOTOR1, 100L<<16);
}


void sendPosition(byte addr, long pos) {
  byte data[] = {0x4, (byte)pos, (byte)(pos>>8), (byte)(pos>>16), (byte)(pos>>24),0};
  sendCmd(addr, data, sizeof(data));
}

void sendMode(byte addr, byte mode) {
  byte data[2] = {0x1, mode};
  sendCmd(addr, data, sizeof(data));
}

void sendCurrentLimit(byte addr, float lim) {
  uint16_t limInt = lim*66*1024/5000;
  byte data[] = {0x82, (byte)limInt, (byte)(limInt>>8)};
  sendCmd(addr, data, sizeof(data));
}

void sendPID(byte addr, float p, float i, float d) {
  int16_t pInt = p*0x10000;
  int16_t iInt = i*0x10000;
  int16_t dInt = d*0x10000;
  byte data[] = {0x30, (byte)pInt, (byte)(pInt>>8), (byte)(pInt>>16), (byte)(pInt>>24),
    (byte)iInt, (byte)(iInt>>8), (byte)(iInt>>16), (byte)(iInt>>24),
    (byte)dInt, (byte)(dInt>>8), (byte)(dInt>>16), (byte)(dInt>>24)};
  sendCmd(addr, data, sizeof(data));
}

void sendCmd(byte addr, byte *data, byte dataLength) {
  Wire.beginTransmission(addr);
  Wire.write(data, dataLength);
  Wire.endTransmission();
}
