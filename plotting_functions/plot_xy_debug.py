# -*- coding: utf-8 -*-
"""
Created on Fri Jun 14 19:04:58 2013

@author: mhurst
"""

#import modules
import numpy as np
import matplotlib.pyplot as plt

X = np.array([346131.74709999998, 
  346089.97960000002, 346062.82612682926, 346032.42983484745, 346007.69277782878, 
  345983.99590335396, 345959.61927720864, 345933.44786878285, 345908.45375422289, 
  345879.37152777455, 345839.47425200837, 345793.91919720086, 345749.97050570149, 
  345687.4511496257, 345673.86960446712, 345779.0904507594, 345768.70882089844, 
  345781.15215039614, 345801.33683474467, 345820.79771125288, 345843.27498722891, 
  345866.04126638285, 345892.89339792205, 345920.91630479007, 345945.60964937968, 
  345972.06143877836, 345999.62277685263, 346025.35467395093, 346051.43269017077, 
  346076.65250734164, 346102.60810372635, 346126.51074275986, 346147.81292590854, 
  346165.01463843515, 346163.58630000002, 346168.03700000001])
  
Y = np.array([681220.96750000003, 
  681256.23030000005, 681309.01850854594, 681360.79617221991, 681407.40681653994, 
  681456.75818901218, 681502.97969460883, 681548.20506902155, 681594.84442569793, 
  681642.69862329704, 681640.46715979197, 681623.1736253819, 681604.15455257241, 
  681628.11977901997, 681697.45397324581, 681792.11214986048, 681775.2567889737, 
  681845.55732787738, 681901.8710131835, 681955.40192429, 682005.78367020341, 
  682053.95781119645, 682100.30437283078, 682146.07514127064, 682191.68570131599, 
  682235.02108738641, 682278.00149241253, 682322.77145947493, 682369.32683468016, 
  682416.64197991951, 682464.81460186886, 682512.42690661224, 682561.77903267404, 
  682614.07067439135, 682668.79489999998, 682719.80610000005])

CliffX = np.array([346135.15769999998, 
  346098.80365697015, 346068.83626583748, 346040.029651877, 346014.2923021926, 
  345993.05769479339, 345969.54137648817, 345944.100832742, 345917.51135549485, 
  345895.92812231911, 345874.75921338896, 345840.52500170644, 345813.2112824088, 
  345779.0904507594, 345781.52925411327, 345799.03252453316, 345815.32814646262, 
  345835.87209629349, 345857.50840876164, 345879.32352884795, 345909.8862643484, 
  345933.79775976151, 345958.08584935026, 345987.63704194094, 346016.14341306116, 
  346042.17995726736, 346066.10945236398, 346091.05553993402, 346115.9356419752, 
  346139.03091682482, 346158.24179336691, 346171.79911333602, 346182.60634738515, 
  346160.54450000002])
  
CliffY = np.array([681220.09939999995, 
  681262.56268127274, 681302.18433308159, 681347.1916825379, 681393.34427217557, 
  681438.68606172618, 681485.59593174432, 681530.19170931191, 681575.16363715718, 
  681622.06678427348, 681668.98953282321, 681706.30075329856, 681752.07100030757, 
  681792.11214986048, 681844.96007107059, 681893.45218184486, 681941.26869002939, 
  681988.36303758947, 682034.03700460657, 682079.60129123274, 682122.75461235968, 
  682167.6876282678, 682212.48801143619, 682257.04797519464, 682302.96500528045, 
  682351.79113865399, 682397.121538557, 682443.29245558649, 682489.62429062359, 
  682539.42151915934, 682588.29147005663, 682639.82547267934, 682692.62898774142, 
  682738.69099999999])
  
#setup the figure space, axis and line to animate
fig = plt.figure(1, figsize=(5,5))
plt.axis('equal')
plt.plot(X,Y,'b.-')
#plt.plot(X2,Y2,'r.-')
plt.plot(CliffX,CliffY,'k.-')

for i in range(0,len(X)):
    plt.text(X[i]+50.,Y[i],str(i))
#
#plt.plot([27.816252252716946,-38705.66598372133],[-3153.4144290955851,89040.497267260376],'k--')
#
##plt.plot(X2,Y2,'r.')
#for i in range(0,len(X)):
#    plt.text(X[i],Y[i],str(i))
#    if Shadows1[i] == 1:
#        plt.plot(X[i],Y[i],'ko', ms=10)
#    if Shadows2[i] == 1:
#        plt.plot(X[i],Y[i],'go', ms=5)
#    elif Shadows2[i] == 2:
#        plt.plot(X[i],Y[i],'ro', ms=5)
#    elif Shadows2[i] > 2:
#        plt.plot(X[i],Y[i],'bo', ms=5)
#               
#plt.plot(X[0:2],Y[0:2],'k-',lw=2)
#plt.plot(X[-2:],Y[-2:],'k-',lw=2)
plt.show()
        