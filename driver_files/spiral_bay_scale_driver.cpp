/*==============================================================

spiral_bay_scale_driver.hpp

A driver function to simulate the evolution of a crenulate or
spiral bay from a straight coastline, forced by a Gaussian
wave climate. Varying the length scale of the coast.

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

/** @file spiral_bay_scale_driver.cpp
@author Martin D. Hurst, British Geological Survey
@brief driver file for evolving a spiral bay from a straight coastline
@details This file generates a coastline object as a straightline and 
evolves it driven by a Gaussian wave climate. This driver allows the
user to vary the length scale of the coastline being evolved
@date 28/10/2015
*/

#include <cmath>
#include <string>
#include <cstring>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <time.h>
#include "./../coastline.hpp"
#include "./../waveclimate.hpp"

using namespace std;


/*

This driver runs a spiral bay under a constant wave climate for a variety of 
different scales and node spacing values. 

Martin Hurst
BGS
20/05/15

*/

string Double2String(double DBL)
{
	ostringstream ss;
	ss << DBL;
	return ss.str();
}
	
int main(int argc, char* argv[])
{
	//read input arguments
	double Length = atof(argv[1]);
	double NodeSpacing = atof(argv[2]);
	double EndTime = atof(argv[3]);
	
	//create output filename
	string FileName = "Spiral";
	string Underscore = "_";
	string Extension = ".xy";
	string LengthString = Double2String(Length);
	string NodeSpacingString = Double2String(NodeSpacing);
	string WriteCoastFile = FileName+Underscore+LengthString+Underscore+NodeSpacingString+Extension;
	
	//declare time control paramters
	//double EndTime = 100.;						          // End time (years)
	double Time = 0.;							                // Start Time (years)
	double PrintTimeDelta = 10./365.;  	          // how often to print coastline (years)
	double PrintTime = PrintTimeDelta;	          // Print time (years)
	double TempTime;
	
	double WaveTimeDelta = 0.01; 			// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.0;				  // Time to get a new wave (days)
	double TimeStep = 0.01;					  // Time step (days)
	double MaxTimeStep = 0.01;				// Maximum timestep (days)	
	
	//initialise coast as straight line with low amp noise
	int MeanNodeSpacing = NodeSpacing; 
	double CoastLength = Length; 
	double Trend = 140.;
	
	//boundary conditions are fixed
	int StartBoundary = 2;
	int EndBoundary = 2;
	
	// Declare wave climate
	double OffshoreMeanWavePeriod = 6.;
	double OffshoreStDWavePeriod = 1.;
  double OffshoreMeanWaveHeight = 1.;
  double OffshoreStDWaveHeight = 0.1;
  double OffshoreMeanWaveDirection = 15.;
  double OffshoreStDWaveDirection = 30.;
  
  
	//initialise wave climates
	GaussianWaveClimate WaveClimate = GaussianWaveClimate(OffshoreMeanWavePeriod, OffshoreStDWavePeriod,
															OffshoreMeanWaveDirection, OffshoreStDWaveDirection,
															OffshoreMeanWaveHeight, OffshoreStDWaveHeight);
	
	Wave MyWave = Wave();
	MyWave = WaveClimate.Get_Wave();

	//initialise the coastline as a straight line
	Coastline CoastVector = Coastline(MeanNodeSpacing, CoastLength, Trend, StartBoundary, EndBoundary);
	
	//Flags for sed transport eq, diffraction and boundary flux
	int CERCFlag = 1;           
	int RefDiffFlag = 1;
	double FluxFraction = 0.0;
	
	//initiate random seed
	//srand (time(NULL));
	srand(1);
	
	//loop through time and evolve the coast
	CoastVector.WriteCoast(WriteCoastFile, Time);
	
	while (Time < EndTime)
	{
		//Get a new wave?
		MyWave = WaveClimate.Get_Wave();
//		if (Time > GetWaveTime) 
//		{
//			MyWave = WaveClimate.Get_Wave();
//			GetWaveTime += WaveTimeDelta/365.;
//		}
		//cout << setprecision(8) << Time << endl;
		
		//Evolve coast
		TempTime = MaxTimeStep;
		CoastVector.TransportSediment(TimeStep, MyWave, CERCFlag, RefDiffFlag, FluxFraction);
		Time += TimeStep/365.;
		if (TimeStep < MaxTimeStep) 
		{
			TempTime = TimeStep;
			while (TempTime < MaxTimeStep)
			{
				//Evolve coast
				CoastVector.TransportSediment(TimeStep, MyWave, CERCFlag, RefDiffFlag, FluxFraction);
				Time += TimeStep/365.;
				TempTime += TimeStep;
			}
		}
		TimeStep = MaxTimeStep;
				
		//Write results to file
		if (Time >= PrintTime)
		{
			CoastVector.WriteCoast(WriteCoastFile, Time);
			PrintTime += PrintTimeDelta;
		}
		//CoastVector.WriteCoast(WriteCoastFile, Time);
	}
	
	cout 	<< endl << "Coastline: Results written to " << WriteCoastFile 
			  << endl << "Coastline: Model run complete." << endl << endl;
			
	return 0;
}

