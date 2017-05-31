# -*- coding: utf-8 -*-
"""
Created on Wed Oct 29 09:16:47 2014

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

#updrift control point
Xu = 0.
Yu = 0.
Orientation = 180.
Length = 1000.

#downdrift control point
Xd = Xu + Length*np.cos(np.radians(Orientation-90.))
Yd = Yu - Length*np.sin(np.radians(Orientation-90.))

#wave crest angle
beta = 25.

#coefficients
#table data from Hsu and Evans (1989)
betas = np.array([10,15,20,25,30,35,40,45,50,55,60,65,70,75,80])
C0 = np.array([0.036,0.050,0.055,0.054,0.045,0.029,0.000,-0.039,-0.088,-0.151,-0.227,-0.315,-0.409,-0.505,-0.600])
C1 = np.array([1.011,0.998,1.029,1.083,1.146,1.220,1.326,1.446,1.588,1.756,1.930,2.113,2.284,2.422,2.520])
C2 = np.array([- 0.047,- 0.049,-0.088,-0.142,-0.194,-0.253,-0.332,-0.412,-0.507,-0.611,-0.706,-0.800,-0.873,- 0.909,- 0.906 ])
#Linear interpolation to get C0, C1, and C2
C_0 = np.interp(beta,betas,C0)
C_1 = np.interp(beta,betas,C1)
C_2 = np.interp(beta,betas,C2)

#length of control line
Rb = np.sqrt((Xu-Xd)**2. + (Yu-Yd)**2.)

#angles to evaluate
Thn = np.arange(beta,180.,5.)

#Convert angles for plotting
Angles = Orientation-beta+Thn

#calculate radii
Rn = Rb*(C_0 + C_1*(beta/Thn) + C_2*(beta/Thn)**2.) 

#Convert radii and angles to X,Y coordinates
Xmod = Xu + Rn*np.cos(np.radians(Angles-90.))
Ymod = Yu - Rn*np.sin(np.radians(Angles-90.))
    
#plot the coast
plt.figure(1)
plt.axis('Equal')
plt.plot([Xu,Xd], [Yu,Yd], 'ko--')
plt.plot(Xmod,Ymod,'k.-')
[plt.text(Xmod[i],Ymod[i],str(i)) for i in range(0,len(Xmod))]
#plt.ylim(-120,20)
plt.show()

