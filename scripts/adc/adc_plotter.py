import serial 
from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import sys 
if __name__ == '__main__':
	MAX = 100
	storage = np.array(np.zeros((6, MAX)))
	i = 0

	ser = serial.Serial()
	ser.baudrate = 115200
	# ser.timeout = 2
	print('/dev/ttyACM{}'.format(sys.argv[1]))
	ser.port = '/dev/ttyACM{}'.format(sys.argv[1])
	ser.open()
	ser.reset_input_buffer()
	while(ser.is_open and (i < MAX)):
		mess = ser.readline().decode('utf-8')

		if (np.fromstring(mess, sep=",").shape[0] != 6):
			continue 
		print(np.fromstring(mess, sep=",").shape)
		storage[:,i] = np.fromstring(mess, sep=",").astype('uint16')
		print(storage[:,i])
		print(i)
		i = (i + 1) #% 100000

	ser.close()

	plt.figure()
	tvec = np.arange(100)

	# legend_list = ["out left", "mid left", 
	# "cent left", "cent right", "mid right", "out right"]
	# for i in range(6):
	# 	plt.plot(tvec, storage[i, :])
	# # plt.plot(tvec, storage[1, :])
	# # plt.plot(tvec, storage[2, :])
	# # plt.plot(tvec, storage[3, :])
	# # plt.plot(tvec, storage[4, :])
	# # plt.legend(["l m","l c","r c", "r m"])

	# plt.legend(legend_list)
	# plt.show()

	plt.subplot(3,1,1)
	plt.title("cent")
	plt.plot(tvec, storage[2, :])
	plt.plot(tvec, storage[3, :])
	# plt.legend("left", "right")
	plt.subplot(3,1,2)
	plt.title("mid")
	plt.plot(tvec, storage[1, :])
	plt.plot(tvec, storage[4, :])
	# plt.legend("left", "right")
	plt.subplot(3,1,3)
	plt.title("outter")
	plt.plot(tvec, storage[0, :])
	plt.plot(tvec, storage[5, :])
	# plt.legend("left", "right")

	plt.show()