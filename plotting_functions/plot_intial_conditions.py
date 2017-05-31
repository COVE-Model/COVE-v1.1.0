# -*- coding: utf-8 -*-
"""
Created on Sun Jul  3 20:48:28 2016

@author: Martin Hurst
"""

"""
Plotting script for Emily Rae, Aberlady Bay COVE runs
This plots the initial model setup

Martin Hurst

3/7/2016
"""

# First, import the modules we will need to handle data (numpy)
# make plots (matplotlib.pyplot) and customise the plotting style
# (matplotlib.rc)
#import modules
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import rc

# Customise figure style
# First we set the default font size for our figures
rc('font',size=8)

# Declare file names for initial conditions
CliffFile = "../driver_files/Spit_Cliffline_Points.xy"
CoastFile = "../driver_files/Spit_Coastline_Points.xy"

# Now that we have declared the file names, let's open the files
# and read in all the data lines into an array

# First read the cliff line
f = open(CliffFile,"r")
CliffLines = f.readlines()
f.close()

#Create the numeric X and Y data
CliffX = np.array(CliffLines[1].strip().split(" ")[1:],dtype='float64')
CliffY = np.array(CliffLines[2].strip().split(" ")[1:],dtype='float64')

# Next the coast line
f = open(CoastFile,"r")
CoastLines = f.readlines()
f.close()

#Create the numeric X and Y data
CoastX = np.array(CoastLines[1].strip().split(" ")[1:],dtype='float64')
CoastY = np.array(CoastLines[2].strip().split(" ")[1:],dtype='float64')

xmin = np.min(CoastX)
xmax = np.max(CoastX)
ymin = np.min(CoastY)
ymax = np.max(CoastY)

# Now let's create some vectors that describe the areas that are sea,
# the areas that are beach and areas that are land. These are only needed
# for colouring in.    
SeaX = np.append(CoastX,[xmin,xmin,CoastX[0]])
SeaY = np.append(CoastY,[ymin,ymax,CoastY[0]])
BeachX = np.append(CoastX,CliffX[::-1])
BeachY = np.append(CoastY,CliffY[::-1])
LandX = np.append(CliffX,[xmax,xmax,CliffX[0]])
LandY = np.append(CliffY,[ymin,ymax,CliffY[0]])

#setup the figure space, axis and line to animate
# first create a figure object
fig = plt.figure(1)
# next set what size the figure will be (in inches)
# the first number is the size in the x-direction (across)
fig.set_size_inches(6,6)
# create an axis on which to plot inside the figure window    
ax = fig.add_axes([0.15,0.15,0.8,0.8])
# set the axis limits based on our min and max variables defined earlier
#ax.set_xlim((xmin,xmax))
#ax.set_ylim((ymin,ymax))
# set the aspect ratio to equal, this makes sure our coast doesn't 
# appear squashed or distorted
ax.set_aspect('equal')

# now plot the evolved cliffline and coastline position but with 
# thinner lines 
plt.plot(CliffX,CliffY, 'k.-', lw=1.0)
plt.plot(CoastX,CoastY, 'b.-', lw=1.0)

#label each node so we can see which order they are in
#for i in range(0,len(CoastX)):
#    plt.text(CoastX[i],CoastY[i],str(i))

#for i in range(0,len(CliffX)):
#    plt.text(CliffX[i],CliffY[i],str(i))
    
# Here do the colouring in bit
#plt.fill(SeaX, SeaY, color=[0.7,0.9,1.0])
#plt.fill(BeachX, BeachY, color=[1.0,0.9,0.6])
#plt.fill(LandX, LandY, color=[0.6,0.9,0.6])

#label the x and y axes with units
plt.xlabel('X [m]')
plt.ylabel('Y [m]')

# Sometimes when working in British National Grid coordinate system
# there ends up being lots of axis tick labels and all the numbers
# look messy. You can control that manually here by telling the plotter
# exactly what numbers you want ticked on your axes. I've commented it out
# for now.
#plt.yticks([275000,280000,285000,290000],fontsize=8)
#plt.xticks([650000,655000],fontsize=8)

# It tends to look neater to rotate the text of the y-axis labels, so the 
# next two lines do that.
locs, labels = plt.yticks()
plt.setp(labels, rotation=90)

# Finally, let's display the results!
plt.show()
