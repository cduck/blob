// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  IMUsetup();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop() {
  bool thing;
  float imuresult = IMUloop(&thing);
  //Serial.println(imuresult);
  //uint32_t *a;
  //a = (uint32_t*)(void*)&imuresult;
  //Serial.println(*a, HEX);
}
