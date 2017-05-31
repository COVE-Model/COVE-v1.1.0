/*==============================================================

newgale_debug.hpp

A driver function to simulate the evolution of a cliffed coastline

Developed by:
Martin D. Hurst
Andrew Barkwith
Michael A. Ellis
Christopher W. Thomas
A. Brad Murray

Copyright (C) 2015, Martin Hurst

Developer can be contacted:
mhurst@bgs.ac.uk

Martin D. Hurst
British Geological Survey,
Environmental Science Centre,
Nicker Hill,
Keyworth,
Nottingham,
UK,
NG12 5GG

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

==============================================================*/

/** @file cliffs_driver.cpp
@author Martin D. Hurst, British Geological Survey
@brief driver file for evolving a cliffed coastline
@details This file generates a coastline object as a straightline and 
evolves it driven by a Gaussian distribution wave climate. The model 
boundaries are periodic. Sediment is passed from one end to the other.
@date 4/1/2015
*/

#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "../coastline.hpp"
#include "../cliffline.hpp"
#include "../waveclimate.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	cout << endl << "Coastline/Cliffline: Initialising Model..." << endl;
	
	//declare time control paramters
	int EndTime = 50.;									// End time (years)
	double Time = 0.;										// Start Time (years)
	double PrintTimeDelta = 36.5/365.;  // how often to print coastline (years)
	double PrintTime = PrintTimeDelta;  // Print time (years)
	
	double WaveTimeDelta = 0.1;					// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.;						// Time to get a new wave (days)
	double TimeStep = 0.1;							// Time step (days)
	double MaxTimeStep = 0.1;						// Maximum timestep (days)	
	
	// initialise the coastline and cliffline objects from file
	// first declare the filenames
	string CoastInFile = "newgale_debug_coastline.txt";
	
	// Setup wave climates
	// We know from wave buoy data that this stretch of coastline experiences a bimodal wave climate
	// but that the buoys are in deeper water than we would like. We will use an idealised bimodal
	// wave climate characterised by two Gaussian distributions, and sample from them at random
	
	// Bimodal wave climate
	//Wave climate 1
	double OffshoreMeanWaveDirection1 = 235.;
	double OffshoreStDWaveDirection1 = 20.;
	double OffshoreMeanWavePeriod1 = 6.;
	double OffshoreStDWavePeriod1 = 2.;
	double OffshoreMeanWaveHeight1 = 0.8;
	double OffshoreStDWaveHeight1 = 0.2;
	GaussianWaveClimate WaveClimate1(OffshoreMeanWavePeriod1,OffshoreStDWavePeriod1,OffshoreMeanWaveDirection1,OffshoreStDWaveDirection1,OffshoreMeanWaveHeight1, OffshoreStDWaveHeight1);
	  
  //declare wave	
	Wave MyWave = Wave();
	MyWave = WaveClimate1.Get_Wave();
	
	// Read the coastline and cliffline data from files
	double StartTime = 0;
	Coastline CoastVector = Coastline(CoastInFile, StartTime);
	double ClosureDepth = 10.;
	double ShorefaceSlope = 0.01;
	CoastVector.SetShorefaceDepth(ClosureDepth);
	CoastVector.SetShorefaceSlope(ShorefaceSlope);
	
	//declare output file names
	string WriteCoastFile = "CliffedCoast_Coastline.xy";
															
	// Since this is a gravel coast, we will set the sediment flux equation to be 
	// the CERC equation with a reduced coefficient to reflect gravel transport
	int FluxType = 2;
	CoastVector.SetFluxType(FluxType);
	
	// Holder for random number sampling
	double rand1;
	
  // MAIN PROGRAM LOOP: Loop through time evolving the coast
  while (Time < EndTime)
  {
    //Get a new wave?
		if (Time > GetWaveTime) 
		{
			// Use a random sampler to chose between wave climates
			MyWave = WaveClimate1.Get_Wave();
			GetWaveTime += WaveTimeDelta/365.;
		}
		
		//Evolve coast
		CoastVector.TransportSediment(TimeStep, MyWave);
		Time += TimeStep/365.;
		TimeStep = MaxTimeStep;
		
		//print to file?
		if (Time >= PrintTime)
		{
			CoastVector.WriteCoast(WriteCoastFile, PrintTime);
			PrintTime += PrintTimeDelta;
		}
  }
  //WriteWaveOut.close();
    
  //write final coastline to file
	CoastVector.WriteCoast(WriteCoastFile, EndTime);
	cout << endl << "Coastline/Cliffline: Model run complete." << endl << endl;
			
  exit(EXIT_SUCCESS);
}
