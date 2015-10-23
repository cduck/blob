
/*****************************************************************
XBee_Remote_Control.ino
Write your Arduino's pins (analog or digital) or read from those
pins (analog or digital) using a remote XBee.
Jim Lindblom @ SparkFun Electronics
Original Creation Date: May 7, 2014

This sketch requires an XBee, XBee Shield and another XBee tied to
your computer (via a USB Explorer). You can use XCTU's console, or
another serial terminal program (even the serial monitor!), to send
commands to the Arduino.

Example usage (send these commands from your computer terminal):
    w#nnn - analog WRITE pin # to nnn
      e.g. w6088 - write pin 6 to 88
    d#v   - digital WRITE pin # to v
      e.g. ddh - Write pin 13 High
    r#    - digital READ digital pin #
      e.g. r3 - Digital read pin 3
    a#    - analog READ analog pin #
      e.g. a0 - Read analog pin 0

    - Use hex values for pins 10-13
    - Upper or lowercase works
    - Use 0, l, or L to write LOW
    - Use 1, h, or H to write HIGH

Hardware Hookup:
  The Arduino shield makes all of the connections you'll need
  between Arduino and XBee. Make sure the JUMPERS ARE CONNECTED D1
  TO DIN AND D0 TO DOUT.
*****************************************************************/
// SoftwareSerial is used to communicate with the XBee
#include <SoftwareSerial.h>

typedef enum {
  POT,POTOFF,MAX,OFF
} state_t;


const int potPos = A0;
const int led = 11;
int pot = 0;
char c = ' ';
state_t state = OFF;
SoftwareSerial XBee(2, 3); // Arduino RX, TX (XBee Dout, Din)

void setup()
{
  // Initialize Bee Software Serial port. Make sure the baud
  // rate matches your XBee setting (9600 is default).
  Serial.begin(9600);
  pinMode(potPos, INPUT);
  pinMode(led, OUTPUT);
  printMenu(); // Print a helpful menu:

}

void loop()
{
  // In loop() we continously check to see if a command has been
  //  received.
  
  if (Serial.available()){
    char c = Serial.read();
    switch (c)
      {
        case 'n':      // If received 'n'
          Serial.println("Potentiometer control on...");
          state = POT;    // Turn on pot control
          break;
        case 'f':      // If received 'f'
          Serial.println("Ending potentiometer control...");
          state = POTOFF;   // Turn off pot control
          break;
        case 'x':      // If received 'x'
          Serial.println("Setting LED to MAXIMUM POWER");
          state = MAX;    // Turn LED to max
          break;
        case 'o':      // If received 'o'
          Serial.println("Turning LED off...");
          state = OFF;    // Turn LED off
          break;
      }
  }

  switch (state) {
    case POT:
      ctrlOn();
      break;
    case POTOFF:
      ctrlOff();
      break;
    case MAX:
      ledMax();
      break;
    default:
      ledOff();
      break;
  }
  delay(50);
  // Serial.println(ctr);
}

// Turn on potentiometer control
// Allows changing the LED brightness using the pot

void ctrlOn()
{
  // Type 'n' to begin potientiometer control
  // Print a message to let the control know of our intentions 
  pot = analogRead(potPos);
  analogWrite(led, pot/4); // Send value to LED
  delay(15);
}

void ctrlOff()
{
  // Type 'f' to turn off potentiometer control, leaving the 
  // LED at the previous setting
  // Print a message to let the control know of our intentions:
}

void ledMax()
{
  // Type 'x' to turn the LED to max power
  // Print a message to let the control know of our intentions:
  digitalWrite(led, 255); // Turn on LED to max
}

void ledOff()
{
  // Type 'o' to turn off the LED
  // Print a message to let the control know of our intentions:
  digitalWrite(led, 0); // Turn off LED
}

// printMenu
// A big ol' string of Serial prints that print a usage menu over
// to the other XBee.
void printMenu()
{
  // Everything is "F()"'d -- which stores the strings in flash.
  // That'll free up SRAM for more importanat stuff.
  Serial.println();
  Serial.println(F("Arduino XBee Remote Control!"));
  Serial.println(F("============================"));
  Serial.println(F("Usage: "));
  Serial.println(F(" n for potentiometer control"));
  Serial.println(F("        f for no pot control"));
  Serial.println(F("         x to set LED to max"));
  Serial.println(F("           o to turn off LED"));
  Serial.println(F("============================"));
  Serial.println();
}
