##Arduino sketch and libraries here.

When using the Arduino IDE, change the default sketchbook location to this directory.

###Sketches

####IMU_Test
A test sketch for the MPU-6050 breakout board from Sparkfun (6 DOF IMU).  Based on the MPU6050_DMP6 example sketch.

####LED Control W/ XBee
Connect the Sending XBee on the XBee Explorer via USB to the computer.  On XCTU (http://www.digi.com/products/xbee-rf-solutions/xctu-software/xctu#resources), use Add Devices to connect the XBee.  If the XBee drivers have not been installed, go here (http://www.ftdichip.com/Drivers/VCP.htm)  Once connected select it from the list of Radio Modules and wait for XCTU to read the configuration.  Click the Console icon in the top right, and then connect it by plugging in the plug in the top left of the console window.  Use the console to control the LED, ddh writes high, ddl writes low to pin 13 (first d means digital write, second d is 13 in hexadecimal, h for high, l for low).

###Libraries

####I2Cdev and MPU6050
Arduino libraries and example code for the MPU-6050 breakout board from Sparkfun (https://www.sparkfun.com/products/11028).  The source is on GitHub here: https://github.com/jrowberg/i2cdevlib.
