# -*- coding: utf-8 -*-
"""
Created on Fri Jun 14 19:04:58 2013

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animate

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

WaveFile = "../driver_files/Waves.txt"
Dir = np.loadtxt(WaveFile,skiprows=1)

plt.hist(Dir,bins=4)
plt.show()

    
        
        