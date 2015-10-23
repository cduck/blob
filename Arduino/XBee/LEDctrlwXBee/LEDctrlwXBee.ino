
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
  between Arduino and XBee. Make sure the SWITCH IS IN THE 
  "DLINE" POSITION.

Development environment specifics:
    IDE: Arduino 1.0.5
    Hardware Platform: SparkFun RedBoard
    XBee Shield & XBee Series 1 1mW (w/ whip antenna)
        XBee USB Explorer connected to computer with another
          XBee Series 1 1mW connected to that.

This code is beerware; if you see me (or any other SparkFun 
employee) at the local, and you've found our code helpful, please 
buy us a round!

Distributed as-is; no warranty is given.
*****************************************************************/
// SoftwareSerial is used to communicate with the XBee
#include <SoftwareSerial.h>

// SoftwareSerial XBee(2, 3); // Arduino RX, TX (XBee Dout, Din)

void setup()
{
  // Initialize Bee Software Serial port. Make sure the baud
  // rate matches your XBee setting (9600 is default).
  Serial.begin(9600);
  delay(100);
  printMenu(); // Print a helpful menu:

}

void loop()
{
  // In loop() we continously check to see if a command has been
  //  received.
  if (Serial.available())
  {
    char c = Serial.read();
    switch (c)
    {
    case 'w':      // If received 'w'
    case 'W':      // or 'W'
      writeAPin(); // Write analog pin
      break;
    case 'd':      // If received 'd'
    case 'D':      // or 'D'
      writeDPin(); // Write digital pin
      break;
    case 'r':      // If received 'r'
    case 'R':      // or 'R'
      readDPin();  // Read digital pin
      break;
    case 'a':      // If received 'a'
    case 'A':      // or 'A'
      readAPin();  // Read analog pin
      break;
    }
  }
}

// Write Digital Pin
// Send a 'd' or 'D' to enter.
// Then send a pin #
//   Use numbers for 0-9, and hex (a, b, c, or d) for 10-13
// Then send a value for high or low
//   Use h, H, or 1 for HIGH. Use l, L, or 0 for LOW
void writeDPin()
{
  while (Serial.available() < 2)
    ; // Wait for pin and value to become available
  char pin = Serial.read();
  char hl = ASCIItoHL(Serial.read());

  // Print a message to let the control know of our intentions:
  Serial.print("Setting pin ");
  Serial.print(pin);
  Serial.print(" to ");
  Serial.println(hl ? "HIGH" : "LOW");

  pin = ASCIItoInt(pin); // Convert ASCCI to a 0-13 value
  pinMode(pin, OUTPUT); // Set pin as an OUTPUT
  digitalWrite(pin, hl); // Write pin accordingly
}

// Write Analog Pin
// Send 'w' or 'W' to enter
// Then send a pin #
//   Use numbers for 0-9, and hex (a, b, c, or d) for 10-13
//   (it's not smart enough (but it could be) to error on
//    a non-analog output pin)
// Then send a 3-digit analog value.
//   Must send all 3 digits, so use leading zeros if necessary.
void writeAPin()
{
  while (Serial.available() < 4)
    ; // Wait for pin and three value numbers to be received
  char pin = Serial.read(); // Read in the pin number
  int value = ASCIItoInt(Serial.read()) * 100; // Convert next three
  value += ASCIItoInt(Serial.read()) * 10;     // chars to a 3-digit
  value += ASCIItoInt(Serial.read());          // number.
  value = constrain(value, 0, 255); // Constrain that number.

  // Print a message to let the control know of our intentions:
  Serial.print("Setting pin ");
  Serial.print(pin);
  Serial.print(" to ");
  Serial.println(value);

  pin = ASCIItoInt(pin); // Convert ASCCI to a 0-13 value
  pinMode(pin, OUTPUT); // Set pin as an OUTPUT
  analogWrite(pin, value); // Write pin accordingly
}

// Read Digital Pin
// Send 'r' or 'R' to enter
// Then send a digital pin # to be read
// The Arduino will print the digital reading of the pin to XBee.
void readDPin()
{
  while (Serial.available() < 1)
    ; // Wait for pin # to be available.
  char pin = Serial.read(); // Read in the pin value

  // Print beggining of message
  Serial.print("Pin ");
  Serial.print(pin);

  pin = ASCIItoInt(pin); // Convert pin to 0-13 value
  pinMode(pin, INPUT); // Set as input
  // Print the rest of the message:
  Serial.print(" = "); 
  Serial.println(digitalRead(pin));
}

// Read Analog Pin
// Send 'a' or 'A' to enter
// Then send an analog pin # to be read.
// The Arduino will print the analog reading of the pin to XBee.
void readAPin()
{
  while (Serial.available() < 1)
    ; // Wait for pin # to be available
  char pin = Serial.read(); // read in the pin value

  // Print beginning of message
  Serial.print("Pin A");
  Serial.print(pin);

  pin = ASCIItoInt(pin); // Convert pin to 0-6 value
  // Printthe rest of the message:
  Serial.print(" = ");
  Serial.println(analogRead(pin));
}

// ASCIItoHL
// Helper function to turn an ASCII value into either HIGH or LOW
int ASCIItoHL(char c)
{
  // If received 0, byte value 0, L, or l: return LOW
  // If received 1, byte value 1, H, or h: return HIGH
  if ((c == '0') || (c == 0) || (c == 'L') || (c == 'l'))
    return LOW;
  else if ((c == '1') || (c == 1) || (c == 'H') || (c == 'h'))
    return HIGH;
  else
    return -1;
}

// ASCIItoInt
// Helper function to turn an ASCII hex value into a 0-15 byte val
int ASCIItoInt(char c)
{
  if ((c >= '0') && (c <= '9'))
    return c - 0x30; // Minus 0x30
  else if ((c >= 'A') && (c <= 'F'))
    return c - 0x37; // Minus 0x41 plus 0x0A
  else if ((c >= 'a') && (c <= 'f'))
    return c - 0x57; // Minus 0x61 plus 0x0A
  else
    return -1;
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
  Serial.println(F("w#nnn - analog WRITE pin # to nnn"));
  Serial.println(F("  e.g. w6088 - write pin 6 to 88"));
  Serial.println(F("d#v   - digital WRITE pin # to v"));
  Serial.println(F("  e.g. ddh - Write pin 13 High"));
  Serial.println(F("r#    - digital READ digital pin #"));
  Serial.println(F("  e.g. r3 - Digital read pin 3"));
  Serial.println(F("a#    - analog READ analog pin #"));
  Serial.println(F("  e.g. a0 - Read analog pin 0"));
  Serial.println();
  Serial.println(F("- Use hex values for pins 10-13"));
  Serial.println(F("- Upper or lowercase works"));
  Serial.println(F("- Use 0, l, or L to write LOW"));
  Serial.println(F("- Use 1, h, or H to write HIGH"));
  Serial.println(F("============================"));  
  Serial.println();
}
