import pandas as pd
import numpy as np
import sys

def get_idx(df, new_mac):
	return df.index[df.loc[:,'MAC'] == new_mac][0]

class MAC_mem:
	def __init__(self):
		self.mem = pd.read_csv('mem.csv',header = 0, index_col = 0)

	def isin(self, MAC):
		if (self.mem.isin([str(MAC)]).any().any()):
			print("already in df! idx is: {}".format(get_idx(df, str(MAC))))
		else:
			print("adding new element!")
			self.mem = pd.concat([self.mem, pd.DataFrame([str(MAC)], columns=["MAC"])], ignore_index = True, axis = 0)


if __name__ == '__main__':
	new_mac = sys.argv[1]
	df = pd.read_csv('mem.csv',header = 0, index_col = 0)
	print(new_mac)
	print(df)
	print(df.loc[:,'MAC'].isin([new_mac]).any().any())
	if (df.isin([new_mac]).any().any()):
		print("already in df! idx is: {}".format(get_idx(df, new_mac)))
	else:

		print("adding new element!")
		df = pd.concat([df, pd.DataFrame([new_mac], columns=["MAC"])], ignore_index = True, axis = 0)
	print(df)

	df.to_csv('mem.csv')