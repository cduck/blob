// SoftwareSerial is used to communicate with the XBee
#include <SoftwareSerial.h>

// Create array to store last received command
typedef enum {
  FWD,BACK,STP,NONE
} state_t;

char c = ' ';
SoftwareSerial XBee(2, 3); // Arduino RX, TX (XBee Dout, Din)

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void XBsetup() {
  // Initialize Bee Software Serial port. Make sure the baud
  // rate matches your XBee setting (9600 is default).
  Serial.begin(9600); 
  printMenu(); // Print a helpful menu
}

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

state_t XBloop() {
  // if data is avalailable from the Serial port (XBee), go 
  // through cases and change state to change function of blob 
  if (Serial.available()){
    char c = Serial.read();
    switch (c)
      {
        case 'f':      // If received 'f'
          Serial.println("Rolling forward...");
          return FWD;    // Set state to FWD
        case 'b':      // If received 'b'
          Serial.println("Rolling backward...");
          return BACK;   // Set state to BACK
        case 's':      // If received 's'
          Serial.println("Stopping the blob");
          return STP;    // Set state to STOP
      }
  }
  return NONE 
}

