#!/usr/bin/python
# Plot data from the PIC32 in python
# requries pyserial, matplotlib, and numpy
import serial
import matplotlib.pyplot as plt
import numpy as np

port = '/dev/ttyUSB0' # the name of the serial port

with serial.Serial(port,230400,rtscts=1) as ser:
  ser.write("\n".encode())  #tell the pic to send data. encode converts to a byte array
  line = ser.readline()
  nsamples = int(line)
  x = np.arange(0,nsamples) # x is [1,2,3,... nsamples]
  y = np.zeros(nsamples)# x is 1 x nsamples an array of zeros and will store the data

  for i in range(nsamples): # read each sample
    line = ser.readline()   # read a line from the serial port
    y[i] = int(line)        # parse the line (in this case it is just one integer)

plt.plot(x,y)
plt.show()
