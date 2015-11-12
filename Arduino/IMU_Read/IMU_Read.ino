// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup() {
  IMUsetup();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

bool ready;

void loop() {

  float imuresult = IMUloop(&ready);
  //Serial.print("\t");
  //Serial.print(imuresult);
  //Serial.print("\t");
  //Serial.println(ready);
  if (ready) {
    Serial.println(imuresult);
  } else {
    Serial.println("NOT READY");
  }
}
