# -*- coding: utf-8 -*-
"""

Python script for creating a straight coast with "estuaries" at the up and downdrift boundaries

Martin Hurst
November 2015

"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

#control params
NoNodes = 120
NodeSpacing = 100.
InlandNodes = 10.
#coordinates
X = np.zeros(NoNodes)
Y = np.zeros(NoNodes)

#loop through and calculate coast
for i in range(0,NoNodes):
    if (i<InlandNodes):
        X[i] = i*NodeSpacing
        Y[i] = 0 + 1.*(-0.5+np.random.rand())
    elif (i<=(NoNodes-InlandNodes)):
        X[i] = X[i-1] + 2.*(-0.5+np.random.rand())
        Y[i] = Y[i-1] - NodeSpacing + 2.*(-0.5+np.random.rand())
    else:
        X[i] = X[i-1] -NodeSpacing + 2.*(-0.5+np.random.rand())
        Y[i] = Y[i-1] + 1.*(-0.5+np.random.rand())
        
#export to xy file
f = open('RightAngleCoast.xy','w')
f.write('2 2\n0 ')
for i in range(0,NoNodes):
    f.write(str(X[i]) + " ")
f.write("\n0 ")
for i in range(0,NoNodes):
    f.write(str(Y[i]) + " ")
f.close()

plt.figure()
plt.axis('equal')
plt.plot(X,Y,'k-')
plt.plot(X,Y,'k.')
plt.xlabel('X (m)')
plt.ylabel('Y (m)')
plt.show()