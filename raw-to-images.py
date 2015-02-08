from PIL import Image
import os
import struct
import numpy as np 

npx = 500
npy = 500
npz = 500

fd = os.open("aspcombined.raw", os.O_RDONLY)

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
