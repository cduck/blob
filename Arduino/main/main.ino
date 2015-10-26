
// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  XBsetup();
  MCsetup();
  Gsetup();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  xbcmd = XBloop();
  motor = MCloop(xbcmd,imuout,lsout);
  Gloop();
  
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
