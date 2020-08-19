import pandas as pd
import numpy as np
from matplotlib import pyplot as plt
import serial

def get_idx(df, id):
	return df.index[df.loc[:,'id'] == id][0]


if __name__ == '__main__':
	ser = serial.Serial()
	ser.baudrate = 115200
	ser.port = '/dev/ttyACM0'
	ser.timeout = .1
	ser.open()

	SIZE = 9
	idx = 0
	df = pd.DataFrame(np.zeros((SIZE, 3)), columns=["id", "abs_time", "rel_time"])
	df.id = df.id.astype(str)
	df.abs_time = df.abs_time.astype('uint32')
	df.rel_time = df.rel_time.astype('uint32')

	id = "eggs"

	time = 0
	while(1):
		mess = ser.readline().decode('utf-8')
		time+=1
		# print(len(mess))
		if (len(mess) == 14):
			id = mess[:4]
			continue

		if(df.loc[:,"id"].isin([id]).any()):
			ent_idx = get_idx(df, id)
			print("id already there at {}".format(ent_idx))
			df.iloc[ent_idx, 1] = time
		else:
			df.loc[idx,"id"] = id
			df.iloc[idx, 1] = time
			idx = (idx + 1) % SIZE
		print(df)

		for i in range(9):
			df.iloc[i, 2] = time - df.iloc[i,1]

	ser.close()


