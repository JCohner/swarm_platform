import pandas as pd
from matplotlib import pyplot as plt
import sys
import numpy as np

def get_active_macs(df):
	idxs = df.index[df.loc[:,"status"] == 1]
	ac_macs = df.iloc[idxs, :].loc[:,"MAC"]
	return ac_macs

def extract_info(mac,df):
	mac_idxs = df.iloc[0,:] == mac
	rel_df = df.loc[:,mac_idxs]
	# print(pd.concat([rel_df], ignore_index=True, axis=1))
	return pd.concat([rel_df], ignore_index=True, axis=1)

class Robot():
	def __init__(self, MAC):
		self.MAC = MAC

	def load_data(self, df):
		self.data = df

	def plot(self):
		plt.figure()
		plt.plot(np.arange(self.data.shape[1]),self.data.loc["pol",:])
		plt.title(self.MAC)
		plt.show()

if __name__ == '__main__':
	mem_df = pd.read_csv("mem/mem.csv", index_col=False)
	df = pd.read_csv("logging/{}".format(sys.argv[1]), index_col=0)
	MACs = get_active_macs(mem_df)
	robot_list = []
	num_ents = df.shape[1]
	for mac, i in zip(MACs, range(MACs.shape[0])):
		robot_list.append(Robot(mac))
		robot_list[i].load_data(extract_info(mac,df))
		robot_list[i].plot()

	plt.figure()
	# colors = [(66, 135, 245),(219, 213, 86),(36, 212, 124), \
	# 			(196, 82, 59),(219, 73, 230),(97, 69, 99),(5, 33, 7), \
	# 			(71, 198, 237),(49, 43, 237)]
	j = 0
	colors = ['#1f77b4', '#ff7f0e', '#2ca02c', '#d62728', '#9467bd', '#8c564b', '#e377c2', '#7f7f7f', '#bcbd22', '#17becf']

	for robot in robot_list:
		print("robot ")
		for i in range(robot.data.shape[1]):
			plt.scatter(robot.data.loc["time", i],robot.data.loc["pol",i], c=[colors[j]])
		j +=1
	plt.show()












