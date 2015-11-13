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
   Serial.print("\t");
   Serial.println(imuresult);
  } else {
    Serial.print("\t");
    Serial.println("NOT READY");
  }
}
