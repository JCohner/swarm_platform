
import sys
import serial

from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *

from data_handler import data_in
from helpful import int_to_bin_str
from comm_packet import packet

import time
import threading
def task1(data_in, num_its):
	print("inside 1")
	print("idx: {} num_its: {}", data_in.data_idx, num_its)

	while(data_in.data_idx < num_its):
		data_in.read()
		time.sleep(0.002)
		print(data_in.data_idx)
	data_in.to_csv()
	print("wrote to csv")

class UI(QWidget):
	def __init__(self):
		super().__init__()
		self.curr_pack = packet()
		grid = QGridLayout()
		self.setLayout(grid)

		#data handler stuff
		self.data_sock = data_in(0)
		print("open")
		label = QLabel("Swarm Control GUI")
		header = QHBoxLayout()
		header.addWidget(label)
		label.setAlignment(Qt.AlignCenter)
		grid.addLayout(header, 0, 0, 1, 1)

		# self.csv_name = QGroupBox("csv name")

		# grid.addLayout(csv_name_box, 0, 1, 1, 1)

		cmd_group = QGroupBox("commands")
		cmd_grid = QGridLayout()
		ID_edit = QLineEdit()
		ID_edit.setPlaceholderText("Robot ID <press Enter>")
		ID_edit.returnPressed.connect(self.set_mach_id)
		cmd_grid.addWidget(ID_edit, 0,0, 1, 2)
		start_btn = QPushButton("start",self)
		stop_btn = QPushButton("stop",self)

		self.enable_btns = [start_btn, stop_btn]
		start_btn.clicked[bool].connect(self.enable)
		stop_btn.clicked[bool].connect(self.enable)


		cmd_grid.addWidget(start_btn, 1,0, 1, 1)
		cmd_grid.addWidget(stop_btn, 1, 1, 1, 1)
		

		self.ID_edit = ID_edit
		self.policy_combo_box = QComboBox();
		# self.policy_combo_box.setPlaceholderText("set policy")
		for i in range(pow(2,5)):
			self.policy_combo_box.addItem(int_to_bin_str(i, 5))
		cmd_grid.addWidget(self.policy_combo_box,2, 0, 1, 2)
		send_pol_btn = QPushButton("send policy")
		send_pol_btn.clicked[bool].connect(self.upload_pol)

		cmd_grid.addWidget(send_pol_btn,3, 1, 1, 1)
		disp_info_btn = QPushButton("disp info")
		cmd_grid.addWidget(disp_info_btn,3, 0, 1, 1)
		cmd_group.setLayout(cmd_grid)
		grid.addWidget(cmd_group, 1,0,1,1)

		trail_group = QGroupBox("data collect")
		trail_group_grid = QGridLayout()

		csv_name_edit = QLineEdit()
		csv_name_edit.setPlaceholderText("CSV Name <press Enter>")
		csv_name_box = QHBoxLayout()
		csv_name_box.addWidget(csv_name_edit)
		csv_name_edit.returnPressed.connect(self.csv_rename)
		self.csv_name_edit = csv_name_edit
		trail_group_grid.addWidget(self.csv_name_edit, 0,0,1,2)
		
		begin_trial_btn = QPushButton("begin")
		end_trial_btn = QPushButton("end")
		self.idx_counter = QLabel("data idx: {}".format(self.data_sock.data_idx))

		begin_trial_btn.clicked[bool].connect(self.begin_trial)
		self.csv_num_el_edit = QLineEdit()
		self.csv_num_el_edit.setPlaceholderText("input # enteries")
		self.csv_num_el_edit.returnPressed.connect(self.num_el_edit)
		self.num_els = 100

		trail_group_grid.addWidget(begin_trial_btn, 1,0,1,1)
		trail_group_grid.addWidget(end_trial_btn, 2,0,1,1)
		trail_group_grid.addWidget(self.csv_num_el_edit, 1, 1, 1, 1)
		trail_group_grid.addWidget(self.idx_counter,2, 1, 1,1)
		trail_group.setLayout(trail_group_grid)
		grid.addWidget(trail_group, 0,1,2,1)

		self.info_disp = QLabel("info:")
		grid.addWidget(self.info_disp, 3,0,1,2)

		# self.line_edit = QLineEdit()
		# enter_line = QPushButton("Enter",self)
		# enter_line.clicked[bool].connect(self.transmit)
		# self.line_edit.returnPressed.connect(self.transmit)
		# liney = QHBoxLayout()
		# liney.addWidget(self.line_edit)
		# liney.addWidget(enter_line)
		# self.line_edit.setPlaceholderText("ex: ID CMD")
		# grid.addLayout(liney, 1, 0, 1, 1)

		self.setWindowTitle('Swarm Interface')
		self.resize(500, 250)
		self.center()
		self.show()

	def set_mach_id(self):
		self.curr_pack.set_mach_id(self.ID_edit.text())
		self.ID_edit.setPlaceholderText(self.ID_edit.text())
		self.info_disp.setText("curr bot: {}".format(self.ID_edit.text()))
		self.ID_edit.setText("")

	def enable(self):
		source = self.sender()
		self.curr_pack.set_cmd("enable")
		if (source == self.enable_btns[0]):
			self.curr_pack.set_info(1)
			txt = "on"
		elif (source == self.enable_btns[1]):
			self.curr_pack.set_info(0)
			txt = "off"
		self.data_sock.write_packet(self.curr_pack)
		self.info_disp.setText("turning bot {} {}".format(self.curr_pack.mach_id, txt))

	def upload_pol(self):
		print(self.policy_combo_box.currentText())
		self.curr_pack.set_info(self.policy_combo_box.currentText())
		self.curr_pack.set_cmd("set_pol")
		self.data_sock.write_packet(self.curr_pack)

	def center(self):
		qr = self.frameGeometry()
		#print(qr)
		cp = QDesktopWidget().availableGeometry().center()
		#print(cp)
		qr.moveCenter(cp)
		self.move(qr.topLeft())

	def csv_rename(self):
		self.data_sock.set_csv_name(self.csv_name_edit.text())
		self.csv_name_edit.setPlaceholderText(self.csv_name_edit.text())
		self.csv_name_edit.setText("")

	def begin_trial(self):
		self.data_sock.data_idx = 0
		ser_read = threading.Thread(target=task1, args=[self.data_sock, self.num_els])
		ser_read.start()
		# while(self.data_sock.data_idx < self.num_els):
		# 	print(self.data_sock.data_idx)
		# 	self.data_sock.read()
		# 	self.idx_counter.setText("data idx: {}".format(self.data_sock.data_idx))

		# self.data_sock.to_csv()

	def num_el_edit(self):
		self.num_els = int(self.csv_num_el_edit.text())
		self.csv_num_el_edit.setPlaceholderText(str(self.num_els))
		self.csv_num_el_edit.setText("")
		self.idx_counter.setText("data idx: {}/{}".format(self.data_sock.data_idx, self.num_els))

if __name__ == '__main__':

	app = QApplication(sys.argv)
	ex = UI()
	# ex.data_sock.ser.close()
	sys.exit(app.exec_())