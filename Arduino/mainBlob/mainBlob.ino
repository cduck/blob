
// Create array to enumerate received command
typedef enum {
  FWD,BACK,STP,NONE
} state_t;
byte state = STP; // Set default command state

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

// boolean to see if the IMU is initialized fully
bool ready;

void loop() {
  
  byte xbcmd = XBLoop(); // Check if there's a command from the XBee
  float imuout = IMULoop(&ready);
  int16_t *motor = MCLoop(xbcmd,imuout,lsGet());
  Gloop();
  
}

// Get a Boolean array to check if any of the limit switches have
// been triggered by a foot plate. 
bool *lsGet() {
  static bool switches[8] = {false};

  //...

  return switches;
}
