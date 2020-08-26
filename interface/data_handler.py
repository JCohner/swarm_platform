import serial 
from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import sys 
import time

import comm_packet
from helpful import int_to_bin_str
from add_robot import MAC_mem


def get_time():
	t = time.localtime()
	str_t = time.strftime("%H:%M:%S", t)
	return str_t

class data_in():
	def __init__(self, port):
		self.data = pd.DataFrame(np.array(np.zeros((6, 10000))), index=["id", "tflag",  "pol", "bb_idx","state", "time"]) 
		self.data.loc["id",:] = self.data.loc["id",:].astype(str)
		self.data.loc["time",:] = self.data.loc["time",:].astype(str)	
		self.data_idx = 0
		self.csv_name = ""

		ser = serial.Serial()
		ser.baudrate = 115200
		ser.port = '/dev/ttyACM{}'.format(port)
		self.ser = ser
		self.ser.open()

		self.start_time = 0
	def to_csv(self):
		self.data.to_csv(self.csv_name)

	def set_csv_name(self, csv_name):
		self.csv_name = "logging/{}".format(csv_name)

	def read(self):
		mess = self.ser.readline().decode('utf-8')
		if (len(mess) != 14):
			return

		id = mess[:4]
		state = np.fromstring(mess[5:], sep=",")
		print(state)
		if(state.shape[0] != 4):
			return


		entry = pd.Series(np.zeros(6), index=["id", "tflag", "pol", "bb_idx", "state", "time"])
		entry.id = entry.id.astype(str)
		entry.loc["id"] = id
		entry.iloc[1:-1] = state
		entry.loc["time"] = get_time()#time.time() - self.start_time
		self.data.iloc[:,self.data_idx] = entry
		print(self.data.iloc[:,self.data_idx])
		self.data_idx += 1

	def write(self,info):
		ser.write(info)

	def write_packet(self,packet):
		mess = ""
		mess += int_to_bin_str(int(packet.mach_id,16), 16)
		mess += '00000' #padding
		mess += int_to_bin_str(packet.cmd, 3)
		if not (isinstance(packet.info, str)):
			mess += int_to_bin_str(packet.info, 8)
		else:
			mess += '000'
			mess += packet.info
		print(mess)
		hex_rep = hex(int(mess, 2)) + '\r'
		print(hex_rep)
		self.ser.write(hex_rep.encode())
		# print(self.ser.read_until())

	def write_message(self, mess):
		self.ser.write(mess)

	# def trial(self):


if __name__ == '__main__':
	DI = data_in(0)
	while(DI.data_idx < 300):
		DI.read()
		print("{}".format(DI.data_idx))

	DI.ser.close()

	DI.to_csv("my_john.csv")
