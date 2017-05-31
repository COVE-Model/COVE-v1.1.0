# -*- coding: utf-8 -*-
"""
Created on Thu Mar 13 10:01:51 2014

@author: mhurst
"""

#modules
import numpy as np
import matplotlib.pyplot as plt

plt.figure(1,figsize=(8,8))
plt.axis('Equal')

#X and Y
try:
    X,Y = np.loadtxt("./../driver_files/XY.txt", unpack=True)
    plt.plot(X,Y,'k.-',zorder=10)
    [plt.text(X[i]-10,Y[i]-10,str(i),fontsize="small") for i in range(0,len(X))]
    NoNodes = len(X)
    
except:
    print "Cannot find XY file!"
    


#load Nodes
try:
    f = open("./../driver_files/Nodes.txt",'r')
    Lines = f.readlines()
    NoCells = len(Lines)
    f.close()
    
    for i in range(0,NoCells):
        Line = Lines[i].strip().split(" ")
        NoNodes = (len(Line)-2)/2
        print i, NoNodes
        X = np.array(Line[2:NoNodes*2+2:2],dtype='float64')
        Y = np.array(Line[3:NoNodes*2+2:2],dtype='float64')
        X = np.append(X,X[0])
        Y = np.append(Y,Y[0])
        plt.plot(X,Y,'-',color=[0.5,0.5,0.5], lw=2)
        plt.plot(X,Y,'k.')

except:
    print "Cannot find Nodes file!"

try:
    f = open("./../driver_files/CliffedCoast_Coastline.xy")
    CoastLines = f.readlines()
    XCoast = np.array(CoastLines[-2].strip().split(" ")[1:],dtype='float64')
    YCoast = np.array(CoastLines[-1].strip().split(" ")[1:],dtype='float64')
    NoCoastNodes = len(XCoast)
    f.close()
    
    plt.plot(XCoast,YCoast,'k.')
    plt.plot(XCoast,YCoast,'k-')
    [plt.text(XCoast[i]+10,YCoast[i]+10,str(i),fontsize="small") for i in range(0,NoCoastNodes)]
    
except:
    print "Cannot find Cliff file!"
    
try:
    f = open("./../driver_files/CliffedCoast_Cliffline.xy")
    CliffLines = f.readlines()
    XCliff = np.array(CliffLines[-2].strip().split(" ")[1:],dtype='float64')
    YCliff = np.array(CliffLines[-1].strip().split(" ")[1:],dtype='float64')
    NoCliffNodes = len(XCliff)
    f.close()
    
    plt.plot(XCliff,YCliff,'ko')
    plt.plot(XCliff,YCliff,'k--')
    [plt.text(XCliff[i],YCliff[i],str(i),fontsize="small") for i in range(0,NoCliffNodes)]
    
except:
    print "Cannot find Cliff file!"

plt.show()