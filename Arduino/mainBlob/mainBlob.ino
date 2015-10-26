
// Create array to enumerate received command
typedef enum {
  FWD,BACK,STP,NONE
} state_t;
byte state = STP; // Set default command state

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  IMUsetup();
  XBsetup();
  MCsetup();
  Gsetup();
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  byte xbcmd = XBloop();
  float imuout = IMUloop();
  int16_t *motor = MCloop(xbcmd,imuout,lsGet());
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
