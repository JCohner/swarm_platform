import sys
def int_to_bin_str(val, len):
	strong = ""
	for i in range(len - 1,-1,-1):
		if int(val) & pow(2, i):
			strong += '1'
		else:
			strong += '0'

	return strong

if __name__ == '__main__':
	print(int_to_bin_str(int(sys.argv[1], 16)))