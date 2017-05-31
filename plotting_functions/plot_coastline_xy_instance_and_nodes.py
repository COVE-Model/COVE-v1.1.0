# -*- coding: utf-8 -*-
"""
Created on Fri Jun 14 19:04:58 2013

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animate

#open coastline evolution file
#filename = "Spiral_RefDiff_6_1_1._0.1_130_20.xy"
filename = "./../driver_files/Cuspate_0.6_0.7.xy"
#filename = "Spiral_6_1_125.xy"
#filename = "Test_Spiral_Out85.xy"
#filename = "Spiral_Holderness_Waves.xy"

Time2Plot = 1

f = open(filename,"r")
Lines = f.readlines()
NoLines = len(Lines)-1
Header = Lines[0]

Time = np.zeros(NoLines-1)
for i in range(0,NoLines-1,2):
    Time[i] = float(Lines[i+1].strip().split(" ")[0])

Diff = np.abs(Time-Time2Plot)
Line = np.argmin(Diff)
print Line

X = np.array(Lines[Line+1].strip().split(" ")[1:],dtype='float64')
Y = np.array(Lines[Line+2].strip().split(" ")[1:],dtype='float64')

print X
print Y

print len(X)

#setup the figure space, axis and line to animate
fig = plt.figure(1, figsize=(5,5))
plt.axis('equal')
plt.plot(X,Y,'k.-')

plt.plot(X[0:2],Y[0:2],'k-',lw=2)
plt.plot(X[-2:],Y[-2:],'k-',lw=2)
plt.show()
        