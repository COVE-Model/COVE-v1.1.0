"""
Script to plot a single COVE coastline at a specified time

7/7/16
Martin Hurst
University of Glasgow

@author: mhurst
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

# First thing we need to do is load the data files that contain the 
# position coordinates of the modelled coastline
# We'll start by declaring the file names in their location
filename = "./../driver_files/Spiral_6_1_1._0.1_35_25.xy"

# Next declare the time we want to plot
Time2Plot = 1.

# Now that we have declared the file names, let's open the files
# and read in all the data lines into an array
f = open(filename,"r")
Lines = f.readlines()
NoLines = len(Lines)-1
Header = Lines[0]

# Next we'll build up a vector of all the model output times
Time = np.zeros(NoLines-1)
for i in range(0,NoLines-1,2):
    Time[i] = float(Lines[i+1].strip().split(" ")[0])

# Now we can search through this vector to find the line index for 
# the time we want to plot
Diff = np.abs(Time-Time2Plot)
Line = np.argmin(Diff)+1

# Let's access the data at the time we want to plot and convert the X and
# Y data to numeric form ready for plotting
X = np.array(Lines[Line].strip().split(" ")[1:],dtype='float64')
Y = np.array(Lines[Line+1].strip().split(" ")[1:],dtype='float64')

print(len(X), len(Y))
# setup the figure space, axis and line to animate
# First let's create a figure object, we'll number it figure 1 and 
# make its size 5x5 inches (why inches!? ffs!)
fig = plt.figure(1, figsize=(5,5))

# next let's make sure we set the aspect ratio to equal so that the x and y
# axes have the same scale, we're plotting a map after all.
plt.axis('equal')

# now plot the coastline,we'll plot both the nodes and connect them with lines
plt.plot(X,Y,'k.-')

# lets put the start and end boundaries in bold, since they are fixed. 
# We do this by making the linewidth (lw) thicker
plt.plot(X[0:2],Y[0:2],'k-',lw=2)
plt.plot(X[-2:],Y[-2:],'k-',lw=2)

# Let's neaten up the plot a little by changing the limits
plt.xlim(np.min(X)-100,np.max(X)+100)
plt.ylim(np.min(Y),np.max(Y))

#Label our axes
plt.xlabel("X coord [m]")
plt.ylabel("Y coord [m]")

#Finally let's save the figure as a pdf
plt.savefig("MyFirstCOVEFigure.pdf")

#And we'll display the figure object too.
plt.show()
        