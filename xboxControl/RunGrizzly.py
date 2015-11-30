#!/usr/bin/env python

import serial
from xbox_read import event_stream

xbee = "/dev/ttyUSB0"
ser = serial.Serial(xbee, baudrate=9600)

inputs = event_stream(4000)
for event in inputs:
    if event.key == "B" and event.is_press():
	print "Forward"
	ser.write('f')
    if event.key == "A" and event.is_press():
	print "Stopping"
	ser.write('s')
    if event.key == "X" and event.is_press():
	print "Backward"
	ser.write('b')

# Appendix: Keys
# --------------
# Key:                      Range of values:
# X1                        -32768 : 32767
# Y1                        -32768 : 32767
# X2                        -32768 : 32767
# Y2                        -32768 : 32767
# du                        0, 1
# dd                        0, 1
# dl                        0, 1
# dr                        0, 1
# back                      0, 1
# guide                     0, 1
# start                     0, 1
# TL                        0, 1
# TR                        0, 1
# A                         0, 1
# B                         0, 1
# X                         0, 1
# Y                         0, 1
# LB                        0, 1
# RB                        0, 1
# LT                        0 : 255
# RT                        0 : 255
