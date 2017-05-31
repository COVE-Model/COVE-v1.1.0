# -*- coding: utf-8 -*-
"""
Created on Wed Feb 19 09:16:33 2014

Python script for creating right angle coast from which to
grow a spit.

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

#control params
NoNodes = 50
NodeSpacing = 50.
Trend = 135.
Angle = 90.

#coordinates
X = np.zeros(NoNodes)
Y = np.zeros(NoNodes)

#loop through and calculate coast
for i in range(0,NoNodes):
    if (i==0):
        X[i] = 0
        Y[i] = 0
    elif (i<NoNodes/2):
        X[i] = X[i-1] + NodeSpacing*np.sin(np.radians(Trend)) + 2*(-0.5+np.random.rand())
        Y[i] = Y[i-1] + NodeSpacing*np.cos(np.radians(Trend)) + 2*(-0.5+np.random.rand())
    else:
        X[i] = X[i-1] + NodeSpacing*np.sin(np.radians(Trend+Angle)) + 2*(-0.5+np.random.rand())
        Y[i] = Y[i-1] + NodeSpacing*np.cos(np.radians(Trend+Angle)) + 2*(-0.5+np.random.rand())

#export to xy file
f = open('RightAngleCoast.xy','w')
f.write('2 2\n0 ')
for i in range(0,NoNodes):
    f.write(str(X[i]) + " ")
f.write("\n0 ")
for i in range(0,NoNodes):
    f.write(str(Y[i]) + " ")

plt.figure()
plt.axis('equal')
plt.plot(X,Y,'k-')
plt.xlabel('X (m)')
plt.ylabel('Y (m)')
plt.show()