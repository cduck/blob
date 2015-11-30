
#define XBEE_FWD 1
#define XBEE_BACK 2
#define XBEE_STP 3
#define XBEE_NONE 0

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================
void setup() {
  IMUSetup();
  XBSetup();
  MCSetup();
  GSetup();
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
void loop() {
  byte xbcmd = XBLoop(); // Check if there's a command from the XBee
  bool ready = false;  // boolean to see if the IMU is initialized fully
  float imuout = 0;//IMULoop(&ready);
  float *positions = MCLoop(xbcmd,imuout,ready||true);
//  for (int i=0; i<4; i++) {
//    Serial.print(positions[i]);
//    Serial.print(", ");
//  }
//  Serial.println();
  GLoop(positions);
}

