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
	print(pd.concat([rel_df], ignore_index=True, axis=1))
	return pd.concat([rel_df], ignore_index=True, axis=1)

class Robot():
	def __init__(self, MAC):
		self.MAC = MAC

	def load_data(self, df):
		self.data = df

if __name__ == '__main__':
	mem_df = pd.read_csv("mem/mem.csv", index_col=False)
	df = pd.read_csv("logging/{}".format(sys.argv[1]), index_col=0)
	MACs = get_active_macs(mem_df)
	robot_list = []
	num_ents = df.shape[1]
	for mac, i in zip(MACs, range(MACs.shape[0])):
		robot_list.append(Robot(mac))
		robot_list[i].load_data(extract_info(mac,df))










