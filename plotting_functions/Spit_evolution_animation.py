# -*- coding: utf-8 -*-

"""

Plotting script for Emily Rae, Aberlady Bay COVE runs



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



# First thing we need to do is load the data files that contain the 

# position coordinates of the modelled coastline

# We'll start by declaring the file names in their location



# I have commented out the declarations below and added extra ones

# because on my computer the files you emailed me are in the same 

# folder as this python script



#CliffFile = "../driver_files/Aberlady1CliffedCoast_Cliffline.xy"

#CoastFile = "../driver_files/Aberlady1CliffedCoast_Coastline.xy"

#OutputFileName = "../driver_files/Aberlady1CliffedCoast"



CliffFile = "../driver_files/SpitTest1CliffedCoast_Cliffline.xy"

CoastFile = "../driver_files/SpitTest1CliffedCoast_Coastline.xy"

OutputFileName = "../driver_files/SpitCliffedCoast"



# Now that we have declared the file names, let's open the files

# and read in all the data lines into an array

# open coastline

f = open(CoastFile,"r")

CoastLines = f.readlines()

f.close()

# and cliffline

f = open(CliffFile,"r")

CliffLines = f.readlines()

f.close()



# We need to know how many data lines we are dealing with so let's check the

# size of the CoastLines object and read it into a variable we'll call NoLines:

NoLines = len(CoastLines)



# There should be the same number of CoastLines as Clifflines, since they output

# from the model at the same time, so let's double check this, just in case 

if NoLines != len(CliffLines):

    print("Output mismatch!")

    raise



# The top line in each file just contains some header information about the type

# of boundary conditions, so let's read this into a new variable called Header

Header = CoastLines[0]



# OK so if we're going to make an animation, we always want the size and shape of 

# our map to be consistent for each frame. Here we'll fix the limits of the 

# plot extent accordingly, based on the extent of the data in the initial cliffline

# You might need to change this later manually to make the plot more like you want it



# These next two lines read the first CliffLine position X and Y coordinates

# respectively. They convert the data from string format to floating point format

InitX = np.array(CliffLines[1].strip().split(" ")[1:],dtype='float64')

InitY = np.array(CliffLines[2].strip().split(" ")[1:],dtype='float64')

# We will need to know how many XY nodes there are when we come to plot

NoNodes = len(InitX)



# From these positions, we'll find the extent by searching for the minimum and 

# maximum values in X and Y coordinatesand assigne them to variables xmin, ymin,

# xmax and ymax. Later on we'll use these to setup the figure.

xmin = np.min(InitX) - 200

xmax = np.max(InitX) + 200

ymin = np.min(InitY)

ymax = np.max(InitY)

# xmin = 652000

# xmax = 656000     Alternatively, use these lines to set xmin,ymin, xmax, ymax

# ymin = 282000     manually using oyur prefered values

# ymax = 290000



# If you have decided to fix some of the coastline e.g. because it is defended

# groins or there is a sea wall or it is made of very hard rock, then we need

# to read in this data so that we can plot the fixed sections differently to 

# indicate that this stretch of coast is defended.

# ID, Fixed = np.loadtxt("../driver_files/TestBelhaven_Fixed_Cliffs.data",dtype='int32',skiprows=1,unpack=True)



# If you have used a U vs A wave climate, this next section of code will setup

# a 4-bin PDF so that you can visualise the wave climate on the map too.

# UA wave cliamte

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



# Alternatively, if you have used a Gaussian wave climate with two modes

# the next section of code will setup PDFs for this so that we can plot that 

# wave climate instead. I have commented this out for now because I don't know

# what type of wave climate you are using.

##bin all wave data and plot!

Dir1 = 340.

Std1 = 20.

Dir2 = 345.

Std2 = 20.

Waves1 = np.random.normal(Dir1,Std1,10000)

Waves2 = np.random.normal(Dir2,Std2,10000)

Waves = np.concatenate((Waves1,Waves2))

Waves[Waves<0] +=360

widths = 10.*np.pi/180.0

hist = np.histogram(Waves*np.pi/180.0,bins=np.arange(0,361*np.pi/180.0,widths))



# Alternatively, if the model is setup to output the waves it has used during the

# model runs, you can just bin and plot the actual wave data

#modelled wave data

#Waves, Height, Period = np.loadtxt("../driver_files/Waves.txt",unpack=True)

#widths = 10.*np.pi/180.0

#hist = np.histogram(Waves*np.pi/180.0,bins=np.arange(0,361*np.pi/180.0,widths))



# To make an animation, we are going to create a whole series of graphics files 

# in the *.png format. These can then all be stitched together using some 

# movie software such as mencoder to produce an animation of your coastline

# model evolving. This process is made easier if we create a list of all the 

# graphics files. So let's create that file and leave it empty for now.

f = open("filelist.txt","w")

f.close()



# GREAT! We're all setup and ready to make our plots for animation. We will

# loop through the cliffline and coastline two lines at a time (1st is X, 2nd 

# is Y data). Each time we'll gather the X and Y data for both the cliff and

# the coast and make a plot those lines. We'll also shade in the region

# between them yellow (for the beach), seaward blue (for the sea), and landward

# green (for the land) so the plots look pretty.



# First let's loop through all the lines in the files until we reach the end,

# skipping two at a time (X and Y)

for i in range(1,NoLines,2):

    

    #first let's read the cliffline data

    CliffX = np.array(CliffLines[i].strip().split(" ")[1:],dtype='float64')

    CliffY = np.array(CliffLines[i+1].strip().split(" ")[1:],dtype='float64')



    #now read coastline

    CoastX = np.array(CoastLines[i].strip().split(" ")[1:],dtype='float64')

    CoastY = np.array(CoastLines[i+1].strip().split(" ")[1:],dtype='float64')

    

    # Let's read in the time, this is the first element on each line

    Time = float(CliffLines[i].split(" ")[0])

    TimeInt = np.rint(Time).astype(int)

    print("Time is", round(Time,1), "years")



    # Now let's create some vectors that describe the areas that are sea,

    # the areas that are beach and areas that are land. These are only needed

    # for colouring in.    

    SeaX = np.append(CoastX,[xmin,xmin,CoastX[0]])

    SeaY = np.append(CoastY,[ymax,ymin,CoastY[0]])

    BeachX = np.append(CoastX,CliffX[::-1])

    BeachY = np.append(CoastY,CliffY[::-1])

    LandX = np.append(CliffX,[xmax,xmax,CliffX[0]])

    LandY = np.append(CliffY,[ymax,ymin,CliffY[0]])

    

    #setup the figure space, axis and line to animate

    # first create a figure object

    fig = plt.figure(1)

    # next set what size the figure will be (in inches)

    # the first number is the size in the x-direction (across)

    fig.set_size_inches(6,6)

    # create an axis on which to plot inside the figure window    

    ax = fig.add_axes([0.15,0.15,0.8,0.8])

    # set the axis limits based on our min and max variables defined earlier

    ax.set_xlim((xmin,xmax))

    ax.set_ylim((ymin,ymax))

    # set the aspect ratio to equal, this makes sure our coast doesn't 

    # appear squashed or distorted

    ax.set_aspect('equal')

    

    # Plot the sections of the coast that are fixed with a thicker line

    # We use the initial cliff position data since we know these sections

    # don't move in the model.    

#    'k-' tells it to plot with a solid black line, lw=1.5 is line width

#    for ii in range(0,NoNodes-1):

#        if (Fixed[ii] == 1) and (Fixed[ii+1] == 1):

#            plt.plot(InitX[ii:ii+2],InitY[ii:ii+2],'k-',lw=1.5)

    

    # now plot the evolved cliffline and coastline position but with 

    # thinner lines 

    plt.plot(CliffX,CliffY, 'k-', lw=1.0)

    plt.plot(CoastX,CoastY, 'k-', lw=1.0)



    # Here do the colouring in bit, your aberlady bay setup is more complicated

    # than my previous settings so I've turned this off for now until one of us

    # has time to fix it! 

    plt.fill(SeaX, SeaY, color=[0.7,0.9,1.0])
    plt.fill(BeachX, BeachY, color=[1.0,0.9,0.6])
    plt.fill(LandX, LandY, color=[0.6,0.9,0.6])

    

    # Let's plot the model time as text on the figure

    plt.text(xmin+500, ymax-1000, ("%.1f" % round(Time,1)) + " years")

    #label the x and y axes with units

    plt.xlabel('X [m]')

    plt.ylabel('Y [m]')

    

    # Sometimes when working in British National Grid coordinate system

    # there ends up being lots of axis tick labels and all the numbers

    # look messy. You can control that manually here by telling the plotter

    # exactly what numbers you want ticked on your axes. I've commented it out

    # for now.

    #plt.yticks([682205.9696,682250.1682],fontsize=8)

    #plt.xticks([345939.9354,345974],fontsize=8)

    

    # It tends to look neater to rotate the text of the y-axis labels, so the 

    # next two lines do that.

    locs, labels = plt.yticks()

    plt.setp(labels, rotation=90)

    

    # Finally plot the wave data as a rose diagram, I've commented this out for

    # now but you can play with it once you're more familiar with plotting

    # in python

    ax = plt.axes([0.6,0.15,0.25,0.25],polar=True)

    plt.bar(hist[1][:-1],hist[0],widths,color='white',edgecolor='k')

    ax.set_theta_zero_location("N")

    ax.set_theta_direction(-1)

    ax.set_axis_bgcolor('none')

    plt.axis('off')

    plt.yticks([])

    plt.xticks([])



    # Let's add the filename to our filelist.txt file

    f = open("filelist.txt","a")

    f.write(OutputFileName+"_"+str(round(Time,1))+".png\n")    

    f.close()

    

    #finally, we will save the figure as a png file in our working directory 

    plt.savefig(OutputFileName+"_"+str((i+1)/2)+".png", dpi=200)

    #and clear the figure ready to make the next one

    plt.clf()

    

