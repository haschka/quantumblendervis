from PIL import Image
import sys
import os
import struct
import numpy as np 

npx = int(sys.argv[2])
npy = int(sys.argv[3])
npz = int(sys.argv[4])

fd = os.open(sys.argv[1], os.O_RDONLY)

object=[]

for i in range(npx):
    for j in range(npy):
        for k in range(npz):
            value = struct.unpack('B',os.read(fd,1))
            object.append(value)

for k in range(npz):
    imagedata = []
    for i in range(npx):
        for j in range(npy):
            imagedata.append(object[i+j*npx+k*npx*npy])

    smalldata = np.array(imagedata,dtype="uint8")
    print ("Saving image: " + str(k))
    theimage = Image.frombytes('L',(npy,npx),smalldata)
    theimage.save('image' + '%04d' % k + '.png', 'PNG')
