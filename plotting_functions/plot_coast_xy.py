# -*- coding: utf-8 -*-
"""
Created on Thu Mar 13 10:01:51 2014

@author: mhurst
"""

#modules
import numpy as np
import matplotlib.pyplot as plt

#X and Y
try:
    X,Y = np.loadtxt("./../driver_files/XY.txt", unpack=True)
            
except:
    print "Cannot find XY file!"

NoNodes = len(X)
 
plt.figure(1,figsize=(3,11))
plt.axis('Equal')
plt.plot(X,Y,'k.-',zorder=10)
[plt.text(X[i]-20,Y[i],str(i),fontsize="small") for i in range(0,NoNodes)]

plt.show()