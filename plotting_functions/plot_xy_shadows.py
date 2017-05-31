# -*- coding: utf-8 -*-
"""
Created on Fri Jun 14 19:04:58 2013

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

X, Y, Shadows = np.loadtxt("./../driver_files/W25/Shadows.txt", unpack=True)
OffshoreWaveDirection = 45.
ShadowAngle = OffshoreWaveDirection+180.
    
#setup the figure space, axis and line to animate
fig = plt.figure(1, figsize=(5,5))
plt.axis('equal')
plt.plot(X,Y,'k.-')

#plt.plot(X2,Y2,'r.')
for i in range(0,len(X)):
    plt.text(X[i],Y[i],str(i))
    if Shadows[i] == 1:
        plt.plot(X[i],Y[i],'ko', ms=8)
    elif Shadows[i] == 2:
        plt.plot(X[i],Y[i],'ro', ms=5)
#        X2 = X[i] + 1000.*np.sin((np.pi/180.)*OffshoreWaveDirection);
#        Y2 = Y[i] + 1000.*np.cos((np.pi/180.)*OffshoreWaveDirection);
#        plt.plot([X[i],X2],[Y[i],Y2],'r--')
    elif Shadows[i] == 3:
        plt.plot(X[i],Y[i],'go', ms=5)
    elif Shadows[i] == 4:
        plt.plot(X[i],Y[i],'bo', ms=5)
#plt.plot(X[0:2],Y[0:2],'k-',lw=2)
#plt.plot(X[-2:],Y[-2:],'k-',lw=2)
plt.show()    