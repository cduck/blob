
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

// boolean to see if the IMU is initialized fully
bool ready;

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================
void loop() {
  byte xbcmd = XBLoop(); // Check if there's a command from the XBee
  float imuout = IMULoop(&ready);
  float *positions = MCLoop(xbcmd,imuout,lsGet());
//  for (int i=0; i<4; i++) {
//    Serial.print(positions[i]);
//    Serial.print(", ");
//  }
//  Serial.println();
  //delay(100);
  GLoop(positions);
}

// Get a Boolean array to check if any of the limit switches have
// been triggered by a foot plate. 
bool *lsGet() {
  static bool switches[8] = {false};

  //...

  return switches;
}
