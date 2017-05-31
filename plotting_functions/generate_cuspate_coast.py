# -*- coding: utf-8 -*-
"""
Created on Wed Feb 19 09:16:33 2014

Python script for creating a cuspate coast from a Gaussian function

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

#control params
NoNodes = 100
NodeSpacing = 50.
Trend = 180.

#coordinates
X = np.zeros(NoNodes)
Y = np.zeros(NoNodes)

#Gaussian properties
Position = -NoNodes*NodeSpacing/2.
print Position
Amplitude = 200.
StDev = 200.

#loop through and calculate coast
for i in range(0,NoNodes):
    if (i==0):
        X[i] = 0
        Y[i] = 0
    else:
        X[i] = X[i-1] + NodeSpacing*np.sin(np.radians(Trend))
        Y[i] = Y[i-1] + NodeSpacing*np.cos(np.radians(Trend))

for i in range(0,NoNodes):
    Mod = Amplitude*np.exp(-((Y[i]-Position)**2.)/(2.*StDev**2.))
    #print Y[i], Mod
    X[i] += Mod
        
#export to xy file
f = open('CuspateCoast.xy','w')
f.write('1 1\n0 ')
for i in range(0,NoNodes):
    f.write(str(X[i]) + " ")
f.write("\n0 ")
for i in range(0,NoNodes):
    f.write(str(Y[i]) + " ")

f.close()

plt.figure()
plt.axis('equal')
plt.plot(X,Y,'k-')
plt.xlabel('X (m)')
plt.ylabel('Y (m)')
plt.show()