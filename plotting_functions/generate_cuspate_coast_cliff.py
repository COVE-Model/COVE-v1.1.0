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
NoNodes = 51
NodeSpacing = 100.
Trend = 180.

#coordinates
XBeach = np.zeros(NoNodes)
YBeach = np.zeros(NoNodes)

XCliff = np.zeros(NoNodes)
YCliff = np.zeros(NoNodes)

#Gaussian properties
Position = -NoNodes*NodeSpacing/2.
print Position
Amplitude = 100.
StDev = 100.
BeachWidth = 0.

#loop through and calculate coast
for i in range(0,NoNodes):
    if (i==0):
        XCliff[i] = 0
        YCliff[i] = 0
        XBeach[i] = XCliff[i] + BeachWidth*np.sin(np.radians(Trend-90))
        YBeach[i] = YCliff[i] + BeachWidth*np.cos(np.radians(Trend-90))
    else:
        XCliff[i] = XCliff[i-1] + NodeSpacing*np.sin(np.radians(Trend))
        YCliff[i] = YCliff[i-1] + NodeSpacing*np.cos(np.radians(Trend))
        random = 0
        XBeach[i] = XCliff[i] + (random+BeachWidth)*np.sin(np.radians(Trend-90))
        YBeach[i] = YCliff[i] + (random+BeachWidth)*np.cos(np.radians(Trend-90))

for i in range(0,NoNodes):
    Mod = Amplitude*np.exp(-((YBeach[i]-Position)**2.)/(2.*StDev**2.))
    #print Y[i], Mod
    XBeach[i] += Mod
        
#export to xy file
f = open('CuspateCoast.xy','w')
f.write('1 1\n0 ')
for i in range(0,NoNodes):
    f.write(str(XBeach[i]) + " ")
f.write("\n0 ")
for i in range(0,NoNodes):
    f.write(str(YBeach[i]) + " ")
f.close()
#export to xy file
f = open('CuspateCoastCliff.xy','w')
f.write('1 1\n0 ')
for i in range(0,NoNodes):
    f.write(str(XCliff[i]) + " ")
f.write("\n0 ")
for i in range(0,NoNodes):
    f.write(str(YCliff[i]) + " ")
f.close()

#Generate stuff for nice plotting
xmin = np.min(XCliff)-500
xmax = np.max(XCliff)+1000
ymin = np.min(YCliff)
ymax = np.max(YCliff)
SeaX = np.append(XBeach,[xmax,xmax,XBeach[0]])
SeaY = np.append(YBeach,[ymin,ymax,YBeach[0]])
BeachX = np.append(XBeach,XCliff[::-1])
BeachY = np.append(YBeach,YCliff[::-1])
CliffX = np.append(XCliff,[xmin,xmin,XCliff[0]])
CliffY = np.append(YCliff,[ymin,ymax,YCliff[0]])
    
from matplotlib import rc
rc('font',size=8)
padding = 5
    
fig = plt.figure(1,figsize=(3,7))
#fig.set_size_inches(8, 2)
ax = plt.axes(xlim=(xmin,xmax), ylim=(ymin,ymax))
ax.set_aspect('equal')
plt.plot(XBeach,YBeach,'k-')
plt.plot(XCliff,YCliff,'k-')
plt.fill(SeaX, SeaY, color=[0.7,0.9,1.0])
plt.fill(BeachX, BeachY, color=[1.0,0.9,0.6])
plt.fill(CliffX, CliffY, color=[0.6,0.9,0.6])
plt.xlabel('X (m)')
plt.ylabel('Y (m)')
locs, labels = plt.yticks()
plt.setp(labels, rotation=90)
plt.xlim(xmin,xmax)
plt.ylim(ymin,ymax)
plt.xticks([-500,0,500,1000])
plt.tight_layout()
plt.show()
