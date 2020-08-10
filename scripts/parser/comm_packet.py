cmd_dict = {
	"enable" : 0,
	"set_pol" : 1
}

class packet():
	def __init__(self, mach_id=0, cmd=7,info=0):
		self.mach_id = mach_id
		self.cmd = cmd
		self.info = info

	def set_cmd(self,cmd):
		self.cmd = cmd_dict[cmd]

	def set_mach_id(self, ID):
		self.mach_id = ID

	def set_info(self, info):
		self.info = info

if __name__ == '__main__':
	packy = packet()
	packy.set_cmd("set_pol")
	print(packy.cmd)