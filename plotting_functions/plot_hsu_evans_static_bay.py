# -*- coding: utf-8 -*-
"""
Created on Thu Mar 13 10:01:51 2014

@author: mhurst
"""

#modules
import numpy as np
import matplotlib.pyplot as plt

#X and Y
X = np.array([0.,15.71704532,-74.92664596,-187.8262909,-279.3293235,-380.7505265,-509.0531734,-593.4525606,-629.1734104,-628.1066695,-601.5640206,-563.9524267,-503.9655166,-447.6114037,-396.2264338,-347.5808861,-296.9455161,-248.6230907,-197.488759,-144.0112371,-89.29394457,-37.5222821,18.18699822,73.28198371,125.9137576,179.4854729,230.5870794,315.9300083,331.1279735])
Y = np.array([0.,-95.79847935,-10.8746567,64.16096309,77.54572466,79.62520476,17.46059907,-94.30470328,-216.2658669,-331.5698525,-437.3417576,-525.5250878,-639.4591682,-736.709306,-822.3985665,-902.2190662,-984.1597601,-1061.270155,-1141.693381,-1224.491109,-1307.891882,-1385.680993,-1468.48533,-1549.645606,-1626.630979,-1704.675543,-1780.152052,-1850.684601,-1950.640793])
OffshoreWaveDirection = 55.

#Calculate FluxOrientation for determination of downbay control point
#first get R_beta
FluxOrientation = np.zeros(len(X))
for i in range(0,len(X)):
    if (i == 0 or i == len(X)-1):   continue
    else:
        dX = X[i+1]-X[i]
        dY = Y[i+1]-Y[i]
        SpatialGradient = dY/dX
        #convert to aximuth
        if (dX == 0 and dY < 0): FluxOrientation[i] = 180.
        elif (dX == 0 and dY > 0):  FluxOrientation[i] = 0.
        elif (dX > 0):   FluxOrientation[i] = np.degrees(np.pi*0.5 - np.arctan(SpatialGradient))
        elif (dX < 0):   FluxOrientation[i] = np.degrees(np.pi*1.5 - np.arctan(SpatialGradient))

control_i=2
ControlPoint = False
while ControlPoint is False:
    if (FluxOrientation[control_i] < OffshoreWaveDirection+90.):
        ControlPoint = True
    else:
        control_i+=1

### #Get parameters for Hsu and Evans model ####

#first get R_beta
dX = X[-3]-X[1]
dY = Y[-3]-Y[1]
R_beta = np.sqrt(dX*dX + dY*dY) # length of baseline

#Next get orientation of baseline		
SpatialGradient = dY/dX
#convert to aximuth
if (dX == 0 and dY < 0): Orientation = 180.
elif (dX == 0 and dY > 0):  Orientation = 0.
elif (dX > 0):   Orientation = np.degrees(np.pi*0.5 - np.arctan(SpatialGradient))
elif (dX < 0):   Orientation = np.degrees(np.pi*1.5 - np.arctan(SpatialGradient))
beta = Orientation-(55.+90.)

#Setup angles to control line
Angles = np.zeros(len(X)-10)
for i in range(0,len(Angles)):
    dX = X[i+2]-X[1]
    dY = Y[i+2]-Y[1]
    SpatialGradient = dY/dX
    #convert to aximuth
    if (dX == 0 and dY < 0): Angles[i] = 180.
    elif (dX == 0 and dY > 0):  Angles[i] = 0.
    elif (dX > 0):   Angles[i] = np.degrees(np.pi*0.5 - np.arctan(SpatialGradient))
    elif (dX < 0):   Angles[i] = np.degrees(np.pi*1.5 - np.arctan(SpatialGradient))
#Angles = Angles-Orientation

#table data from Hsu and Evans (1989)
betas = np.array([10,15,20,25,30,35,40,45,50,55,60,65,70,75,80])
C0 = np.array([0.036,0.050,0.055,0.054,0.045,0.029,0.000,-0.039,-0.088,-0.151,-0.227,-0.315,-0.409,-0.505,-0.600])
C1 = np.array([1.011,0.998,1.029,1.083,1.146,1.220,1.326,1.446,1.588,1.756,1.930,2.113,2.284,2.422,2.520])
C2 = np.array([- 0.047,- 0.049,-0.088,-0.142,-0.194,-0.253,-0.332,-0.412,-0.507,-0.611,-0.706,-0.800,-0.873,- 0.909,- 0.906 ])

#Linear interpolation to get C0, C1, and C2
C_0 = np.interp(beta,betas,C0)
C_1 = np.interp(beta,betas,C1)
C_2 = np.interp(beta,betas,C2)
print C_0, C_1, C_2
print beta
print R_beta
#Solve for radius as a function of Angles \theta_n
Rn = R_beta*(C_0 + C_1*(beta/Angles) + C_2*(beta/Angles)**2.) 
#Rn = R_beta*(C_0 + C_1*(beta/np.radians(Angles)) + C_2*(beta/np.radians(Angles))**2.) 
print Rn

#Convert Angles, Rn to X, Y positions
Xmod = X[1] - Rn*np.cos(np.radians(Angles[i]+90.))
Ymod = Y[1] + Rn*np.sin(np.radians(Angles[i]+90.))

print Angles
print Xmod
print Ymod

# plot
plt.figure(1)
plt.plot(X,Y,'k-')
plt.plot(X,Y,'k.')
plt.plot(X[control_i],Y[control_i],'ko')
plt.plot(Xmod,Ymod,'ro')
for i in range(0,len(X)):
    plt.text(X[i]+20,Y[i],str(i))
plt.axis('equal')
plt.show()