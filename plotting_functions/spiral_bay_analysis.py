# -*- coding: utf-8 -*-
"""
Created on Tue Nov  4 15:57:36 2014

Script to analyse coastline spiral bays

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

#Load file and test steady-state?
filename = "Spiral_6_1_1._0.1_55_1.xy"
f = open(filename,"r")
Lines = f.readlines()
NoLines = len(Lines)
Header = Lines[0]

X0 = np.array(Lines[1].strip().split(" ")[1:],dtype='float64')
Y0 = np.array(Lines[2].strip().split(" ")[1:],dtype='float64')

for Line in range(3,NoLines,2):
    
    #Get Lines
    Line1 = np.array(Lines[Line].strip().split(" "),dtype='float64')
    Line2 = np.array(Lines[Line+1].strip().split(" "),dtype='float64')
    
    #Get Time, X and Y
    Time = Line1[0]
    X = Line1[1:]
    Y = Line2[1:]
    
    #Get coast length
    