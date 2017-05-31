# -*- coding: utf-8 -*-
"""
Created on Fri Jun 14 19:04:58 2013

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animate
import os
#open coastline evolution file
#filename = "Spiral_RefDiff_6_1_1._0.1_130_20.xy"
#filename = "./../driver_files/Spiral_6_1_1._0.1_U0.95_A0.95.xy"
#filename = "Spiral_6_1_125.xy"
#filename = "Test_Spiral_Out85.xy"
#filename = "Spiral_Holderness_Waves.xy"
filename = "./../../../Model_Runs/benacre.xy"

print os.getcwd()

f = open(filename,"r")
Lines = f.readlines()
NoLines = len(Lines)
Header = Lines[0]

X0 = np.array(Lines[1].strip().split(" ")[1:],dtype='float64')
Y0 = np.array(Lines[2].strip().split(" ")[1:],dtype='float64')

#setup the figure space, axis and line to animate
fig = plt.figure()
fig.set_size_inches(4, 8)
#ax = plt.axes(xlim=(-4000,-1000), ylim=(-3000,4000))
ax = plt.axes(xlim=(-1000,2000), ylim=(-3000,1000))
#ax = plt.axes(xlim=(-200,300), ylim=(-1500,-300))
ax.set_aspect('equal')
plt.plot(X0,Y0,'k-')
line, = ax.plot([],[], lw=1.5)


#define an initialisation function
def init():
    line.set_data([], [])
    return line,

#define an animation function
def animation(i):
    j = 2*i+1
    Line  = Lines[j].strip().split(" ")
    print "Time is", '%.2f'%float(Line[0])
        
    X = np.array(Line[1:],dtype='float64')
    Y = np.array(Lines[j+1].strip().split(" ")[1:],dtype='float64')
    line.set_data(X,Y)
    return line,

anim = animate.FuncAnimation(fig,animation,init_func=init,
                             frames=(NoLines-1)/2,
                             interval=20,blit=True)

vidfile = filename.rstrip(".xy")
anim.save(vidfile + ".mp4", writer='mencoder', fps=10)
plt.savefig(filename.rstrip(".xy") + ".pdf")
plt.show()
    
#    plt.figure(1, figsize=(4,10))
#    plt.axis('equal')
#    plt.plot(X,Y,'k-')
#    plt.xlabel('X (m)')
#    plt.ylabel('Y (m)')
#    plt.xlim(-200,1200)
#    plt.ylim(-5000,0)
#    plt.savefig("coast" + str(int(((j+1.)/2.))) + ".png")
#    plt.clf()
    
#plt.plot(X,Y,'b.-', ms = 5, linewidth=2, zorder = 11)

#plot node numbers if needed
#for i in range(0,len(X)):
#    plt.text(X[i]+1,Y[i]+1,str(i))

    
        
        