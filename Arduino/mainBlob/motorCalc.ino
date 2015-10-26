int16_t motor[4] = {0,0,0,0};

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void MCSetup() {
  
}


// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

int16_t *MCLoop(byte xbcmd,float imuout, bool *lsout) {

  // If there is a new command from the XBee, change the current 
  // state for calculating motor distances.
  if (xbcmd != NONE) {
    state = xbcmd;
  }

  if (imuout == NAN) {
    
  }

  // Return an array of motor values using the functions below
  switch (state) {
    case FWD:
      return fwd(imuout);
    case BACK:
      return back(imuout);
    default:
      return stp(imuout);
  }

  //Check if any of the limit switches have been triggered in order to reset all the motors
  for (int i=0; i<8; i++){
    if (lsout[i] == true) {
      motorReset(imuout);
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

}
