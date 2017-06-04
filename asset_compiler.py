import os
import shutil
import argparse
from struct import pack, unpack

LOG_LEVEL = 0

ASSET_TYPE_GLSL = 0
ASSET_TYPE_SCENE = 1
ASSET_TYPE_FONT = 2
ASSET_TYPE_PNG = 3
ASSET_TYPE_JPG = 15
ASSET_TYPE_DDS = 16
ASSET_TYPE_PVR = 17
ASSET_TYPE_END = -1

def copy_file(src, dest):
	shutil.copy(src, dest)

class FileWriter:
	def __init__(self, path):
		self.target = open(path, "wb")
		self.offset = 0
		self.file_start = 0
		self.BYTE_ALIGNMENT = 4

	def close(self):
		self.target.close()

	def padding(self, size):
		return (self.BYTE_ALIGNMENT - size % self.BYTE_ALIGNMENT) % self.BYTE_ALIGNMENT

	def i32(self, val):
		self.target.write(pack("i", val))
		self.offset += 4

	def u32(self, val):
		self.target.write(pack("I", val))
		self.offset += 4

	def f64(self, val):
		self.target.write(pack("d", val))
		self.offset += 8

	def f32(self, val):
		self.target.write(pack("f", val))
		self.offset += 4

	def string(self, val):
		str_encoded = val.encode('ascii')
		str_encoded += '\0'.encode('ascii') #for c strings only!!
		size = len(str_encoded)
		pad = self.padding(size)

		self.i32(size)
		self.target.write(str_encoded)
		for i in range(0, pad):
			self.target.write(pack("x"))
		self.offset += size + pad

	def bytes(self, val):
		size = len(val)
		pad = self.padding(size)
		self.target.write(val)
		for i in range(0, pad):
			self.target.write(pack("x"))
		self.offset += size + pad


def compile_binary(name, read_path, writer, type):
	log(read_path)
	writer.i32(type)
	file_size = os.path.getsize(read_path)
	writer.u32(file_size)
	
	file = open(read_path, 'rb')
	writer.string(name)
	writer.bytes(file.read())
	file.close()

def read_glsl(read_path, output):
	file = open(read_path, 'r')
	for line in file:
		if line.startswith('//import'):
			path = line.split('//import ')[1].rstrip('\n').rstrip('\r')
			output.append('\n')
			read_glsl(path, output)
		else:
			output.append(line)
	file.close()
	#log(read_path)

def compile_glsl(name, read_path, writer):
	output = []

	read_glsl(read_path, output)

	read_state = 0
	vertex_shader_found = False
	fragment_shader_found = False

	vertex_src = ""
	fragment_src = ""

	for line in output:
		if read_state == 0:
			if line.startswith("#VERTEX"):
				vertex_shader_found = True
				read_state = 1
				continue
		elif read_state == 1:
			if line.startswith("#FRAGMENT"):
				fragment_shader_found = True
				read_state = 2
				continue
			else:
				vertex_src += line
		elif read_state == 2:
			fragment_src += line

	if not vertex_shader_found:
		log("Could not find vertex shader in file: " + name)
		return False

	if not fragment_shader_found:
		log("Could not find fragment shader in file: " + name)
		return False

	log(read_path)
	writer.i32(ASSET_TYPE_GLSL)
	writer.string(name)
	writer.string(vertex_src)
	writer.string(fragment_src)

def compile_assets(dir, write_path):
	writer = FileWriter(write_path)

	for root, directories, files in os.walk(dir):
		for f in files:
			split = f.split(".")
			name = split[0]
			file_type = split[1]
			path = os.path.join(root, f)

			if file_type == 'glsl': compile_glsl(name, path, writer)
			elif file_type == 'scene': compile_binary(name, path, writer, ASSET_TYPE_SCENE)
			elif file_type == 'font': compile_binary(name, path, writer, ASSET_TYPE_FONT)
			elif file_type == 'png': 
				compile_binary(name, path, writer, ASSET_TYPE_PNG)
			#elif file_type == 'jpg': compile_binary(name, path, writer, ASSET_TYPE_JPG)
			#elif file_type == 'dds': compile_binary(name, path, writer, ASSET_TYPE_DDS) 
			#elif file_type == 'pvr': compile_binary(name, path, writer, ASSET_TYPE_PVR) 

	writer.i32(ASSET_TYPE_END)
	writer.close()


def log(msg):
	if LOG_LEVEL == 1: print(msg)

def build():
	parser = argparse.ArgumentParser()
	parser.add_argument('--debug', required=True)
	parser.add_argument('--log', type=int, required=1)

	args = parser.parse_args()
	debug = args.debug == 'True'

	global LOG_LEVEL
	LOG_LEVEL = args.log

	log('----------Compiling Assets-----------')
	compile_assets('src', 'build/assets.bin')

	#log('')

build()