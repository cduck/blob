// SoftwareSerial is used to communicate with the XBee
#include <SoftwareSerial.h>

// Create array to store last received command
typedef enum {
  FWD,BACK,STOP
} state_t;

char c = ' ';
state_t state = STP; // Set default command state
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

void XBloop() {
  // if data is avalailable from the Serial port (XBee), go 
  // through cases and change state to change function of blob 
  if (Serial.available()){
    char c = Serial.read();
    switch (c)
      {
        case 'f':      // If received 'f'
          Serial.println("Rolling forward...");
          state = FWD;    // Set state to FWD
          break;
        case 'b':      // If received 'b'
          Serial.println("Rolling backward...");
          state = BACK;   // Set state to BACK
          break;
        case 's':      // If received 's'
          Serial.println("Stopping the blob");
          state = STP;    // Set state to STOP
          break;
      }
  }

  switch (state) {
    case FWD:
      fwd();
      break;
    case BACK:
      back();
      break;
    default:
      stp();
      break;
  }
  delay(50);
}

