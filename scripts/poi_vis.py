import serial 
from matplotlib import pyplot as plt
import numpy as np

def group(vals):
	
	grp_idx = 0
	val_prev = vals[0]
	grps = [np.array([val_prev])]
	# print("first val {}".format(val_prev))
	for i in range(1,vals.shape[0]):
		val = vals[i]
		# print("comparing: {} & {}".format(val, val_prev))
		# print("abs diff: {}".format(abs(val - val_prev)))
		
		if abs(val - val_prev) < 3:
			#in the same group
			grps[grp_idx] = np.append(grps[grp_idx], val)
		else:
			grps.append(np.array([val]))
			grp_idx += 1

		val_prev = val
	return (grps, len(grps))


if __name__ == '__main__':
	ser = serial.Serial()
	ser.baudrate = 115200
	# ser.timeout = 2
	ser.port = '/dev/ttyACM0'
	ser.open()
	while(ser.is_open):
		mess = ser.readline().decode('utf-8')
		if (len(mess) < 100):
			print(mess)
		vals = np.fromstring(mess, dtype=np.uint, sep=' ')
		filt_vals = np.where(vals >= 1 )[0]
		# print(filt_vals)
		if (filt_vals.shape[0] > 0):
			grps, num_grps = group(filt_vals)
			print(num_grps)
			print(grps)
		# else:
			# print("")
			# print(0)
			# ser.close()