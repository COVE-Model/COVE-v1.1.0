# -*- coding: utf-8 -*-
"""
Created on Fri Jun 14 19:04:58 2013

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

##########################
# Customise figure style #
##########################
from matplotlib import rc
rc('font',size=8)
padding = 5

#open coastline and cliffline evolution files
CliffFile = "../driver_files/CliffedCoast_Cliffline.xy"
CoastFile = "../driver_files/CliffedCoast_Coastline.xy"
OutputFileName = "../driver_files/CliffedCoast"

f = open(CoastFile,"r")
CoastLines = f.readlines()
f.close()

f = open(CliffFile,"r")
CliffLines = f.readlines()
f.close()

NoLines = len(CoastLines)
if NoLines != len(CliffLines):
    print "Output mismatch!"
    raise

Header = CoastLines[0]

#use last line to get extent
InitX = np.array(CliffLines[1].strip().split(" ")[1:],dtype='float64')
InitY = np.array(CliffLines[2].strip().split(" ")[1:],dtype='float64')
NoNodes = len(InitX)

xmin = np.min(InitX)-2000
xmax = np.max(InitX)+3000
ymin = np.min(InitY)
ymax = np.max(InitY)
#xmin = 652000
#xmax = 656000
#ymin = 282000
#ymax = 290000

#open fixed data
ID,Fixed = np.loadtxt("../driver_files/Benacre_Fixed_Cliffs.data",dtype='int32',skiprows=1,unpack=True)
##UA wave cliamte
#width = 45.*(np.pi/180.)
#bins = np.arange(10.*(np.pi/180.),190.*(np.pi/180.),width)
#print bins
#hist = np.ones(4)
#U = 0.6
#A = 0.4
#hist[0] = U*A
#hist[1] = (1-U)*A
#hist[2] = (1-U)*(1-A)
#hist[3] = U*(1-A)

##bin all wave data and plot!
Dir1 = 45
Std1 = 20
Dir2 = 140
Std2 = 20
Waves1 = np.random.normal(Dir1,Std1,10000)
Waves2 = np.random.normal(Dir2,Std2,10000)
Waves = np.concatenate((Waves1,Waves2))
Waves[Waves<0] +=360
widths = 10.*np.pi/180.0
hist = np.histogram(Waves*np.pi/180.0,bins=np.arange(0,361*np.pi/180.0,widths))

#modelled wave data
#Waves, Height, Period = np.loadtxt("../driver_files/Waves.txt",unpack=True)
#widths = 10.*np.pi/180.0
#hist = np.histogram(Waves*np.pi/180.0,bins=np.arange(0,361*np.pi/180.0,widths))

f = open("filelist.txt","w")
f.close()

TrackTime = []
MeanY = []

#for i in range(0,1):
for i in range(0,(NoLines-1)/2):
    j = 2*i+1
    CliffLine  = CliffLines[j].strip().split(" ")
    CoastLine  = CoastLines[j].strip().split(" ")
    Time = float(CliffLine[0])
    TimeInt = np.rint(Time).astype(int)
    print "Time is", round(Time,1), "years"
        
    #read cliffline
    CliffX = np.array(CliffLine[1:],dtype='float64')
    CliffY = np.array(CliffLines[j+1].strip().split(" ")[1:],dtype='float64')
    
    #read coastline
    CoastX = np.array(CoastLine[1:],dtype='float64')
    CoastY = np.array(CoastLines[j+1].strip().split(" ")[1:],dtype='float64')
    
    SeaX = np.append(CoastX,[xmax,xmax,CoastX[0]])
    SeaY = np.append(CoastY,[ymin,ymax,CoastY[0]])
    BeachX = np.append(CoastX,CliffX[::-1])
    BeachY = np.append(CoastY,CliffY[::-1])
    LandX = np.append(CliffX,[xmin,xmin,CliffX[0]])
    LandY = np.append(CliffY,[ymin,ymax,CliffY[0]])
    
    #setup the figure space, axis and line to animate
    fig = plt.figure(1)
    fig.set_size_inches(3,6)
    ax = plt.axes(xlim=(xmin,xmax), ylim=(ymin,ymax))
    ax.set_aspect('equal')
    
    for ii in range(0,NoNodes-1):
        if (Fixed[ii] == 1) and (Fixed[ii+1] == 1):
            plt.plot(InitX[ii:ii+2],InitY[ii:ii+2],'k-',lw=1.5)
    
    plt.plot(CliffX,CliffY, 'k-', lw=0.5)
    plt.plot(CoastX,CoastY, 'k-', lw=0.5)

    plt.fill(SeaX, SeaY, color=[0.7,0.9,1.0])
    plt.fill(BeachX, BeachY, color=[1.0,0.9,0.6])
    plt.fill(LandX, LandY, color=[0.6,0.9,0.6])
    plt.text(xmin+500, ymax-1000, ("%.1f" % round(Time,1)) + " years")
    plt.xlabel('X [m]')
    plt.ylabel('Y [m]')
    
#    for ii in range(0,len(CoastX)-1):
#        plt.plot(CoastX[ii],CoastY[ii],'k.')
#        plt.text(CoastX[ii], CoastY[ii],str(ii))

    #plt.xlim(xmin,xmax)
    #plt.ylim(ymin,ymax)
    plt.yticks([284000,286000,288000,290000],fontsize=8)
    plt.xticks([652000,654000,656000],fontsize=8)
    locs, labels = plt.yticks()
    plt.setp(labels, rotation=90)
    plt.tight_layout()
    #plt.show()

    #plot all wave data
    ax = plt.axes([0.6,0.15,0.25,0.25],polar=True)
    plt.bar(hist[1][:-1],hist[0],widths,color='white',edgecolor='k')
    ax.set_theta_zero_location("N")
    ax.set_theta_direction(-1)
    ax.set_axis_bgcolor('none')
    plt.axis('off')
    plt.yticks([])
    plt.xticks([])

    f = open("filelist.txt","a")
    f.write(OutputFileName+"_"+str(i)+".png\n")    
    f.close()
    plt.savefig(OutputFileName+"_"+str(i)+".png", dpi=200)
    plt.clf()
    