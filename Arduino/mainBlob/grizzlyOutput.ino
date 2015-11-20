#include "I2Cdev.h"
#include "Wire.h"

#define MOTOR0 0x0A
#define MOTOR1 0x0B
#define MOTOR2 0x0C
#define MOTOR3 0x0D
#define PID_THRESHOLD 400  // Use PID when error is less than one inch.

#define NUM_MOTORS 4
const byte MOTORS[NUM_MOTORS] = {MOTOR0,MOTOR1,MOTOR2,MOTOR3};
const byte reverseMotor[NUM_MOTORS] = {false, false, true, false};
const byte limitSwitches[NUM_MOTORS*2] = {7,6,5,4,3,A1,A2,A3};  // Two per motor

#define MOTOR_HOME_OFFSET 23080L  // Encoder steps
#define ENCODER_SCALING_FACTOR 4990L   // 23080 encoder steps per 4.625 inches
#define MOTOR_MAX_EXTENT_INCH (4.625-0.5)  // Half inch buffer
#define MOTOR_MAX_EXTENT 20584L //or ((int32_t)(MOTOR_MAX_EXTENT_INCH*ENCODER_SCALING_FACTOR))

int32_t motorEncOffset[NUM_MOTORS] = {0, 0, 0, 0};

float lim = 3;
float p = .1;
float i = 0;
float d = 0;//4.5, 9.25

// Grizzly commands
#define WireSendArr(ADDR, DATA) {Wire.beginTransmission(ADDR);\
                                 Wire.write(DATA, sizeof(DATA));\
                                 Wire.endTransmission();}
// First byte is address; the rest is data.  (little endian byte order)
byte disableB[] = {0x1, 0b00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
byte startB[] = {0x1, 0b00010011,0x0,0x0,0x0,0x0,100,0x0,0x0};  // Mode (byte 1) and speed (bytes 4-7)
byte startBSlow[] = {0x1, 0b00010011,0x0,0x0,0x0,0x0,30,0x0,0x0};
byte stopB[] = {0x1, 0b00010011,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
byte reverseB[] = {0x1, 0b00010011,0x0,0x0,0x0,0x0,0x9C,0xFF,0x0};
byte reverseBSlow[] = {0x1, 0b00010011,0x0,0x0,0x0,0x0,0xE2,0xFF,0x0};
byte accelLimit[] = {0x90, 50, 0};  // Acceleration limit (pwm change per ms)
byte currLimit[] = {0x82, 54, 0};  // Current limit, val=limit*(1024/5)*(1/1000)*(66), 54->4 Amps
byte setToutB[] = {0x80, 0xFF, 0x00};  // Timeout (100 ms)
byte setPID[] = {0x30,   0, 0, 10, 0,   0x8F,0x02,0,0,   0x9A,0x19,0,0};  // PID=(10, .01, .1)
byte pidTargetB[] = {0x1, 0b00010111, 0x0,0x0,  0x0,0x0,0x0,0x0,  0x0};  // Set target, mode=PID position
byte rawSpeedTarget[] = {0x1, 0b00010011, 0x0,0x0,  0x0,0x0,0x0,0x0,  0x0};  // Set target, mode=PID position


bool isHomedVar = false;

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

// No setup for I2C but should include if it doesn't work
void GSetup() {
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

  for (int i = 0; i < NUM_MOTORS*2; i++) {
    pinMode(limitSwitches[i], INPUT_PULLUP);
    if (digitalRead(limitSwitches[i]) == LOW) {
      Serial.print("Limit switch ");
      Serial.print(i/2);
      static char a[] = "A";
      a[0] = 'A'+(i%2);
      Serial.print(a);
      Serial.println(" pressed");
    }else {
      Serial.print("Limit switch ");
      Serial.print(i/2);
      static char a[] = "A";
      a[0] = 'A'+(i%2);
      Serial.print(a);
      Serial.println(" not pressed");
    }
  }
  Serial.println();
  //delay(1000);

  /*Serial.println("Running Motor 0");
  int n = 3;
  sendMode(MOTORS[n], 0x13); //0x3 for raw speed, 0x5 for pid speed, 0x7 for pid pos
  sendCurrentLimit(MOTORS[n], lim);
  sendPosition(MOTORS[n], 100L*0x10000);
  delay(200);
  sendPosition(MOTORS[n], 0L*0x10000L);
  Serial.println("Done");*/
 
  for (int i = 0; i < NUM_MOTORS; i++) {
    byte addr = MOTORS[i];
    sendMode(addr, 0x13); //0x13 for raw speed, 0x15 for pid speed, 0x17 for pid pos
    WireSendArr(addr, accelLimit);
    WireSendArr(addr, setToutB);
    sendPID(MOTORS[i], p, i, d);
  }
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

// Takes the desired motor positions from motorCalc and outputs an
// I2C command to the Grizzlys to spin the motors
void GLoop(float *positions) {
  //sendPosition(MOTOR0, 100L<<16);

  byte *switches = readLimitSwitches();
  /*for (int i=0; i<NUM_MOTORS*2; i++) {
    Serial.println(switches[i]);
  }*/

  int32_t a;
  readEncoderPos(MOTORS[1], &a);
  Serial.print("ENC ");
  Serial.println(a);
  
  static bool done = false;
  if (!done) {
    done = isHomedVar = homeMotors(switches);
    Serial.println(done ? "Done homing" : "Still homing");
    int32_t pos = 0;
    for (int i=0; i<NUM_MOTORS; i++) {
      if (readEncoderPos(MOTORS[i], &pos)) {
        Serial.println(pos);
      } else {
        Serial.println("FAILED TO READ ENCODER");
      }
    }
    if(done) {
      Serial.println("OFFSET");
      for (int i=0; i<NUM_MOTORS; i++) {
        Serial.println(motorEncOffset[i]);
      }
      Serial.println("ENC");
      for (int i=0; i<NUM_MOTORS; i++) {
        int32_t a;
        readEncoderPos(MOTORS[i], &a);
        Serial.println(a);
      }
      //while (1) {}
    }
  }else {
    for (int i=0; i<NUM_MOTORS; i++) {
      float target = positions[i];
      if (setRealPosition(i, target, switches)) {
      }else {
        //Serial.print("FAILED TO SET POSITION TO ");
        //Serial.println(target);
      }
    }
  }
}


bool setRealPosition(byte motorI, float inches, byte *limitSwitches) {
  int32_t targetScale = (int32_t)(inches * ENCODER_SCALING_FACTOR);
  if (targetScale > MOTOR_MAX_EXTENT || targetScale < -MOTOR_MAX_EXTENT) {
    sendRawSpeed(MOTORS[motorI], 0);
    return false;
  }
  int32_t targetPos;
  if (reverseMotor[motorI]) {
    targetPos = motorEncOffset[motorI] - targetScale - MOTOR_HOME_OFFSET;
  }else {
    targetPos = motorEncOffset[motorI] + targetScale + MOTOR_HOME_OFFSET;
  }
  int32_t currentPos;
  if (readEncoderPos(MOTORS[motorI], &currentPos)) {
    int32_t error = targetPos - currentPos;
    if ((limitSwitches[motorI*2] && error < 0) || (limitSwitches[motorI*2+1] && error > 0)) {
      sendRawSpeed(MOTORS[motorI], 0);
      return false;
    }
    if (error <= PID_THRESHOLD && error >= -PID_THRESHOLD) {
      //sendPosition(MOTORS[motorI], targetPos);
      sendRawSpeed(MOTORS[motorI], 0);
    }else {
      sendRawSpeed(MOTORS[motorI], error > 0 ? 50 : -50);
//      Serial.print(currentPos);
//      Serial.print(", ");
//      Serial.println(targetPos);
//      Serial.print(", ");
//      Serial.println(motorEncOffset[motorI]);
    }
    return true;
  }else {
    return false;
  }
}

bool homeMotors(byte *limitSwitches) {
  bool done = true;
  bool reverseHome = false;
  for (int i=0; i<NUM_MOTORS; i++) {
    bool reverse = reverseMotor[i] ^ reverseHome;
    if (limitSwitches[i*2+(reverse ? 1 : 0)]) {
      sendRawSpeed(MOTORS[i], 0);  // Stop
      int32_t pos = 0;
      bool success = false;
      for (int j=0; !(success=readEncoderPos(MOTORS[i], &pos)); j++) {
        if (j >= 5) {
          fatalError();
        }
      }
      if (reverse) {
        motorEncOffset[i] = pos;// - MOTOR_HOME_OFFSET;
      }else {
        motorEncOffset[i] = pos;// + MOTOR_HOME_OFFSET;
      }
    }else {
      sendRawSpeed(MOTORS[i], reverse ? 50 : -50);
      done = false;
    }
  }
  return done;
}

bool isHomed() {
  return isHomedVar;
}





void sendPosition(byte addr, uint32_t pos) {
  pidTargetB[4] = (byte)pos;
  pidTargetB[5] = (byte)(pos>>8);
  pidTargetB[6] = (byte)(pos>>16);
  pidTargetB[7] = (byte)(pos>>24);
  WireSendArr(addr, pidTargetB);
}
void sendRawSpeed(byte addr, uint16_t speed) {
  rawSpeedTarget[4] = (byte)0;
  rawSpeedTarget[5] = (byte)0;
  rawSpeedTarget[6] = (byte)(speed>>0);
  rawSpeedTarget[7] = (byte)(speed>>8);
  WireSendArr(addr, rawSpeedTarget);
}

void sendMode(byte addr, byte mode) {
  byte data[2] = {0x1, mode};
  WireSendArr(addr, data);
}

void sendCurrentLimit(byte addr, float lim) {
  uint16_t limInt = lim*66*1024/5000;
  byte data[] = {0x82, (byte)limInt, (byte)(limInt>>8)};
  WireSendArr(addr, data);
}

void sendPID(byte addr, float p, float i, float d) {
  long pp = p*0x10000;
  setPID[1] = (byte)pp;
  setPID[2] = (byte)(pp>>8);
  setPID[3] = (byte)(pp>>16);
  setPID[4] = (byte)(pp>>24);
  long ii = i*0x10000;
  setPID[5] = (byte)ii;
  setPID[6] = (byte)(ii>>8);
  setPID[7] = (byte)(ii>>16);
  setPID[8] = (byte)(ii>>24);
  long dd = d*0x10000;
  setPID[9] = (byte)dd;
  setPID[10] = (byte)(dd>>8);
  setPID[11] = (byte)(dd>>16);
  setPID[12] = (byte)(dd>>24);
  WireSendArr(addr, setPID);
}

// Returns 1 on success
bool readEncoderPos(uint8_t addr, int32_t *pos) {
  Wire.beginTransmission(addr | 0x80);
  Wire.write(0x20);
  Wire.endTransmission(false);
  Wire.requestFrom(addr, (uint8_t)4, (uint8_t)true);
  char *pp = (char*)(void*)pos;
  /*if (Wire.available() <= 0) {
    Wire.beginTransmission(addr | 0x80);
    Wire.write(0x20);
    Wire.endTransmission(false);
    Wire.requestFrom(addr, (uint8_t)4, (uint8_t)true);
  }*/
  long startTime = micros();
  while (Wire.available() < 4 && micros()-startTime < 2000) {}
  if (Wire.available() >= 4) {
    pp[0] = Wire.read();
    pp[1] = Wire.read();
    pp[2] = Wire.read();
    pp[3] = Wire.read();
    return true;
  } else {
    return false;
  }
}

/*void sendCmd(byte addr, byte *data, byte dataLength) {
  Wire.beginTransmission(addr);
  Wire.write(data, dataLength);
  Wire.endTransmission();
}*/


byte *readLimitSwitches() {
  static byte switchValues[NUM_MOTORS*2];
  for (int i=0; i<NUM_MOTORS*2; i++) {
    switchValues[i] = digitalRead(limitSwitches[i]) == LOW;  // True if pressed
  }
  return switchValues;
}

void fatalError() {
  pinMode(13, OUTPUT);
  while (1) {
    digitalWrite(13, HIGH);
    delay(75);
    digitalWrite(13, LOW);
    delay(75);
  }
}


