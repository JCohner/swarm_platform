import pandas as pd
import numpy as np
import sys

def get_idx(df, new_mac):
	return df.index[df.loc[:,'MAC'] == new_mac][0]



if __name__ == '__main__':
	new_mac = sys.argv[1]
	df = pd.read_csv('mem.csv')
	print(new_mac)
	print(df)
	print(df.loc[:,'MAC'].isin([new_mac]).any().any())
	if (df.isin([new_mac]).any().any()):
		print("already in df! idx is: {}".format(get_idx(df, new_mac)))
	else:

		print("adding new element!")
		df.append([new_mac])
	print(df)

	df.to_csv('mem.csv')