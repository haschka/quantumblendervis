#!/usr/bin/env python3
__help__ = '''
## ubuntu requires:
sudo apt-get install build-essential
sudo apt-get install libatlas-base-dev
sudo apt-get install liblapack*
sudo apt-get install libblas*
sudo apt-get install nwchem

## note the openbabel from snap is broken
#sudo snap install openbabel
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
obabel = os.path.join(_thisdir,'build-openbabel/bin/obabel')

def convert_smile(smile):
	tmp = '/tmp/in.smi'
	open(tmp,'w').write(smile)
	out = '/tmp/out.xyz'
	cmd = [obabel, '-i', 'smiles', tmp, '-o', 'xyz', '-O', out]
	print(cmd)
	os.environ['BABEL_LIBDIR']=os.path.join(_thisdir,'build-openbabel/lib')
	subprocess.check_call(cmd, cwd='/tmp')

def install_openbabel():
	if not os.path.isdir('openbabel'):
		cmd = ['git', 'clone', '--depth', '1', 'https://github.com/openbabel/openbabel.git']
		print(cmd)
		subprocess.check_call(cmd)

	if not os.path.isdir('build-openbabel'):
		os.mkdir('build-openbabel')
	cmd = ['cmake', os.path.join(_thisdir,'openbabel')]
	print(cmd)
	subprocess.check_call(cmd,cwd='./build-openbabel')

	cmd = ['make']
	print(cmd)
	subprocess.check_call(cmd,cwd='./build-openbabel')

def install_nwchem():
	if not os.path.isdir('nwchem'):
		cmd = ['git', 'clone', '--depth', '1', 'https://github.com/nwchemgit/nwchem.git']
		print(cmd)
		subprocess.check_call(cmd)

	cmd = ['make']
	print(cmd)
	os.environ['NWCHEM_TOP'] = os.path.join(_thisdir, 'nwchem')
	os.environ['NWCHEM_TARGET'] ='LINUX64'
	os.environ['USE_MPI']='y'
	os.environ['NWCHEM_MODULES']= 'qm' #"all python"
	os.environ['BLASOPT']='-lopenblas'
	os.environ['LAPACK_LIB']= '-lopenblas'
	os.environ['BLAS_SIZE']='8'
	subprocess.check_call(cmd,cwd='./nwchem/src')


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
		elif arg=='--install':
			if not os.path.isfile(obabel):
				install_openbabel()
			install_nwchem()

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
