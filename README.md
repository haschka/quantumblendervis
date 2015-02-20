# quantumblendervis
Tools to render quantum mechanical datasets using blender

These tools should compile either using clang or gcc.

cubealign.c has to be linked agains a lapack library, i.e. Apple's Veclib,
Atlas or Intels Math Kernel Library

raw2img.py requires the python imaging library to be installed

High resolution images, and a sample Gaussian Cube dataset can further
be found here. The sample Guassian cube file has to be rebuild first due to the
limitations of github. In order to do this:

- Download the gpv001-500.cube.xzaa, pv001-500.cube.xzab, gpv001-500.cube.xzac
  files

- Join the either using the "cat" or the "join" command.

- Extract the cube from the resulting xz file using the "unxz" command

Binaries of the tools are provided for the 64 bit linux platform. If you
encounter difficulties running them on your distribution or unix system
please feel free to recompile them from the provided sources. 