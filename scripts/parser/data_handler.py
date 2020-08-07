import serial 
from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import sys 

class data_in():
	def __init__(self, port):
		self.data = np.array(np.zeros((5, 10000)))
		self.data_idx = 0
		self.csv_name = ""

		ser = serial.Serial()
		ser.baudrate = 115200
		ser.port = '/dev/ttyACM{}'.format(port)
		self.ser = ser
		self.ser.open()

	def to_csv(self):
		pd.DataFrame(self.data).to_csv(self.csv_name, header=None, index=None)

	def set_csv_name(self, csv_name):
		self.csv_name = csv_name

	def read(self):
		mess = self.ser.readline().decode('utf-8')
		print(np.fromstring(mess, sep=","))
		if (np.fromstring(mess, sep=",").shape[0] != 5):
			return
		print(mess)
		self.data[:,self.data_idx] = np.fromstring(mess, sep=",").astype('uint16')
		self.data_idx += 1

	def write(info):
		ser.write(info)

if __name__ == '__main__':
	DI = data_in(0)
	while(DI.data_idx < 300):
		DI.read()
		print("{}".format(DI.data_idx))

	DI.ser.close()

	DI.to_csv("my_john.csv")
