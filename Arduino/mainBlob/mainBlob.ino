
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
  byte xbcmd = XBLoop();
  float imuout = IMULoop(&ready);
  int16_t *motor = MCLoop(xbcmd,imuout,lsGet());
  Gloop();
  
}

bool *lsGet() {
  static bool switches[8] = {false};

  //...

  return switches;
}

/* switch (state) {
    case FWD:
      returnfwd();
      break;
    case BACK:
      back();
      break;
    default:
      stp();
      break;
  }*/
