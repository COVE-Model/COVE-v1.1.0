# -*- coding: utf-8 -*-
"""
Created on Wed Feb 19 09:16:33 2014

Python script for creating sine wave coast for testing symettrical
coastal behaviour

Dec 2015

@author: mdhurst1
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

#control params
NoNodes = 51
NodeSpacing = 100.
Trend = 180.
SineWaveAmplitude = 100.
SineWaveWaveLength = 5000.

#coordinates
X = np.zeros(NoNodes)
Y = np.zeros(NoNodes)

#loop through and calculate coast
for i in range(0,NoNodes):
    if (i==0):
        X[i] = 0
        Y[i] = 0
    else:
        Y[i] = Y[i-1] - NodeSpacing
        X[i] = X[i-1] + SineWaveAmplitude*np.sin(2.*np.pi*Y[i]/SineWaveWaveLength)
          
#export to xy file
f = open('../driver_files/init.txt','w')
f.write('1 1\n0 ')
for i in range(0,NoNodes):
    f.write(str(X[i]) + " ")
f.write("\n0 ")
for i in range(0,NoNodes):
    f.write(str(Y[i]) + " ")
f.close()

plt.figure(figsize=(2,8))
plt.axis('equal')
plt.plot(X,Y,'k-')
plt.xlabel('X (m)')
plt.ylabel('Y (m)')
plt.show()