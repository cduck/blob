int16_t motorArray[4,] = {0,0,0,0};
state_t state = STP; // Set default command state

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void MCsetup() {
  
}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

int16_t *MCloop(state_t xbcmd,float imuout,bool *lsout) {

  if (xbcmd != NONE) {
    state = xbcmd;
  }

  switch (state) {
    case FWD:
      return fwd();
    case BACK:
      return back();
    default:
      return stp();
  }

  for (int i=0; i<8; i++){
    if (lsout[i] = TRUE) {
      motorReset();
    }
  }
}

int16_t *fwd(float imuout) {
  // calc
}

int16_t *back(float imuout) {
  //calc
}

int16_t *stp(float imuout) {
  //calc
}

int16_t *motorReset(float imuout) {
