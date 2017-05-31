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

##########################
# Customise figure style #
##########################
from matplotlib import rc
#rc('font',**{'family':'sans-serif','sans-serif':['Helvetica']})
rc('font',size=8)
#rc('ytick.major',pad=5)
#rc('xtick.major',pad=5)
#rc('xtick', direction='out')
#rc('ytick', direction='out')
#rc('text', usetex=True)
#rc('pdf',fonttype=42)
padding = 1


#open coastline evolution file
U = 0.7
A = 0.5
filename = "../driver_files/COVE_"+str(U)+"_"+str(A)

f = open(filename+".xy","r")
Lines = f.readlines()
f.close()

NoLines = len(Lines)
Header = Lines[0]

#use last line to get extent
InitY = np.array(Lines[1].strip().split(" ")[1:],dtype='float64')
InitX = -np.array(Lines[2].strip().split(" ")[1:],dtype='float64')

xmin = np.min(InitX)
xmax = np.max(InitX)
ymin = np.min(InitY)-2000
ymax = np.max(InitY)+2000

#UA wave cliamte
width = 45*(np.pi/180.)
bins = np.arange(0,180*(np.pi/180),width)-2.*width
hist = np.ones(4)
hist[0] = U*A
hist[1] = (1-U)*A
hist[2] = (1-U)*(1-A)
hist[3] = U*(1-A)

f = open("filelist.txt","w")
f.close()

for i in range(0,(NoLines-1)/2,2):
    j = 2*i+1
    Line  = Lines[j].strip().split(" ")
    Time = float(Line[0])
    TimeInt = np.rint(Time).astype(int)
    print "Time is", round(Time,1), "years"
        
    Y = np.array(Line[1:],dtype='float64')
    X = -np.array(Lines[j+1].strip().split(" ")[1:],dtype='float64')
    X = X-X[0]
    
    SeaX = np.append(X,[xmax,xmin,X[0]])
    SeaY = np.append(Y,[ymax,ymax,Y[0]])
    BeachX = np.append(X,[xmax,xmin,X[0]])
    BeachY = np.append(Y,[ymin,ymin,Y[0]])
    
    #setup the figure space, axis and line to animate
    fig = plt.figure()
    fig.set_size_inches(6.4, 3.6)
    ax = plt.axes(xlim=(xmin,xmax), ylim=(ymin,ymax))
    ax.set_aspect('equal')
    plt.xticks([0,2000,4000,6000,8000])
    plt.plot(InitX,InitY,'k--')
    plt.plot(X,Y, 'k-', lw=0.5)
    plt.fill(SeaX, SeaY, color=[0.7,0.9,1.0])
    plt.fill(BeachX, BeachY, color=[1.0,0.9,0.6])
    plt.text(xmin+200, ymin+200, ("%.1f" % round(Time,1)) + " years")
    plt.xlabel('X [m]')
    plt.ylabel('Y [m]')
    plt.xlim(xmin,xmax)
    plt.ylim(ymin,ymax)
    locs, labels = plt.yticks()
    plt.setp(labels, rotation=90)
    plt.tight_layout()

    #bin all wave data and plot!
    ax = plt.axes([0.05,0.63,0.2,0.2],polar=True)
    plt.bar(bins,hist,width,color='white',edgecolor='k')
    ax.set_theta_zero_location("N")
    ax.set_theta_direction(-1)
    ax.set_axis_bgcolor('none')
    plt.axis('off')
    plt.yticks([])
    plt.xticks([])
    plt.xlim(0,180)
        
    plt.text(1.25*np.pi,0.6,"U = " + str(U) + "\nA = " + str(A))    
    
    f = open("filelist.txt","a")
    f.write(filename+"_"+str(i)+".png\n")    
    f.close()
    plt.savefig(filename+"_"+str(i)+".png", dpi=200)
    plt.clf()
    
    if TimeInt > 20:
        break





    
        
        