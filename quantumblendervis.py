#!/usr/bin/env python3
__help__ = '''
## ubuntu requires:
sudo apt-get install build-essential
sudo apt-get install libatlas-base-dev
sudo apt-get install liblapack*
sudo apt-get install libblas*
sudo apt-get install nwchem
sudo snap install openbabel
'''

import os, sys, math, subprocess
try:
	import bpy, mathutils
except:
	bpy=None

_thisdir = os.path.split(os.path.abspath(__file__))[0]
if _thisdir not in sys.path: sys.path.insert(0,_thisdir)

cube2raw = '/tmp/cube2raw'
cubealign = '/tmp/cubealign'
multicube2raw = '/tmp/multicube2raw'

def convert_smile(smile):
	tmp = '/tmp/in.smi'
	open(tmp,'w').write(smile)
	out = '/tmp/out.xyz'
	#cmd = ['openbabel.obabel', '-i', 'smiles', '/tmp/in.smi', '-o', 'xyz', '-O', out]
	cmd = ['openbabel.obabel', './in.smi', '-O', './out.xyz', '-h', '--gen3D']
	print(cmd)
	subprocess.check_call(cmd, cwd='/tmp')

if __name__ == '__main__':
	if not os.path.isfile(cube2raw):
		cmd = ['gcc', '-O2', '-march=native', 'cube2raw.c', '-o', cube2raw, '-lm']
		print(cmd)
		subprocess.check_call(cmd)

	if not os.path.isfile(cubealign):
		cmd = ['gcc', '-O2', '-march=native', 'cubealign.c', '-o', cubealign, '-latlas', '-llapack', '-lm']
		print(cmd)
		subprocess.check_call(cmd)

	if not os.path.isfile(multicube2raw):
		cmd = ['gcc', '-O2', '-march=native', 'multicube2raw.c', '-o', multicube2raw, '-lm']
		print(cmd)
		subprocess.check_call(cmd)

	args = []
	kwargs = {}
	blend = None
	for arg in sys.argv:
		if arg.startswith('--') and '=' in arg:
			args.append(arg)
			k,v = arg.split('=')
			k = k[2:]
			kwargs[k]=float(v)
		elif arg.endswith('.blend'):
			blend = arg
		elif arg=='--test':
			convert_smile('CCCCOc1ccccc1')
			sys.exit()

	if not bpy:
		cmd = ['blender']
		if blend: cmd.append(blend)
		cmd += ['--python', __file__]
		if args:
			cmd += ['--'] + args
		print(cmd)
		subprocess.check_call(cmd)
		sys.exit()

	if 'Cube' in bpy.data.objects:
		bpy.data.objects.remove( bpy.data.objects['Cube'] )