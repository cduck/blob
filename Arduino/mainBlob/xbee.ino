// SoftwareSerial is used to communicate with the XBee
#include <SoftwareSerial.h>

char c = ' ';
SoftwareSerial XBee(2, 3); // Arduino RX, TX (XBee Dout, Din)

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void XBSetup() {
  // Initialize Bee Software Serial port. Make sure the baud
  // rate matches your XBee setting (9600 is default).
  Serial.begin(9600); 
}

// ================================================================
// ===                    XBEE PROGRAM LOOP                     ===
// ================================================================

byte XBLoop() {
  // if data is avalailable from the Serial port (XBee), go 
  // through cases and change state to change function of blob 
  if (Serial.available()) {
    char c = Serial.read();
    switch (c)
      {
        case 'f':      // If received 'f'
          PRINTln("Rolling forward...");
          return XBEE_FWD;    // Set state to FWD
        case 'b':      // If received 'b'
          PRINTln("Rolling backward...");
          return XBEE_BACK;   // Set state to BACK
        case 's':      // If received 's'
          PRINTln("Stopping the blob");
          return XBEE_STP;    // Set state to STOP
        case 'c':      // If received 's'
          PRINTln("Centering the blob");
          return XBEE_CENTER;    // Set state to STOP
      }
  }
  return XBEE_NONE; // Default case is no change
}
