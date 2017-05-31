/*==============================================================

spiral_bay_driver.hpp

A driver function to simulate the evolution of a crenulate or
spiral bay from a straight coastline, forced by a UA
wave climate.

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

/** @file spiral_bay_driver.cpp
@author Martin D. Hurst, British Geological Survey
@brief driver file for evolving a spiral bay from a straight coastline
@details This file generates a coastline object as a straightline and 
evolves it driven by a UA wave climate (Ashton et al. 2006). The model 
boundaries are fixed to represent headlands or seawalls. Sediment is
allowed to escape around these fixed ends, but none is provided into the
model domain. 
@date 28/10/2015
*/

#include <cmath>
#include <cstring>
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
coast always on the left of vector as you progress from i=0 to i=NoNodes
Node i=0 and i=NoNodes-1 are the same Node copied
*/

int main(int argc, char* argv[])
{
	if (argc != 7)
	{
		cout << "Program needs 6 input arguments:	 \n\t- MeanWavePeriod\n\t- StDWavePeriod"
												<<	"\n\t- MeanWaveHeight\n\t- StDWaveHeight"
												<<	"\n\t- Wave Highness\n\t- Wave asymmetry" << endl;
		exit(EXIT_FAILURE);
	}
	
	//record start time
	clock_t t1, t2;
	t1 = clock();
		
	//RealWaveClimate WaveClimate = RealWaveClimate(WaveDataFile);
	double 	OffshoreMeanWavePeriod, OffshoreStDWavePeriod, OffshoreMeanWaveHeight, 
			OffshoreStDWaveHeight, U, A;
	
	//create output filename
	string FileName = "Spiral";
	string Underscore = "_";
	string Extension = ".xy";
	string arg1 = argv[1];
	string arg2 = argv[2];
	string arg3 = argv[3];
	string arg4 = argv[4];
	string arg5 = argv[5];
	string arg6 = argv[6];
	string WriteCoastFile = FileName+Underscore+arg1+Underscore+arg2+Underscore+arg3+Underscore+arg4+Underscore+"U"+arg5+Underscore+"A"+arg6+Extension;
			
	//Read wave parameters from argvs
	OffshoreMeanWavePeriod = atof(argv[1]); 
	OffshoreStDWavePeriod = atof(argv[2]);
	OffshoreMeanWaveHeight = atof(argv[3]); 
	OffshoreStDWaveHeight = atof(argv[4]);
	U = atof(argv[5]); 
	A = atof(argv[6]);
		
	//declare time control paramters
	int EndTime = 100.;						// End time (years)
	double Time = 0.;							// Start Time (years)
	double PrintTimeDelta = 10./365.;	// how often to print coastline (years)
	double PrintTime = PrintTimeDelta;	// Print time (years)
	double TempTime;
	
	double WaveTimeDelta = 0.1;			// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.0;				// Time to get a new wave (days)
	double TimeStep = 0.05;					// Time step (days)
	double MaxTimeStep = 0.05;				// Maximum timestep (days)	
	
	//initialise coast as straight line with low amp noise
	int MeanNodeSpacing = 50; 
	double CoastLength = 2000; 
	double Trend = 140.;
	
	//boundary conditions are fixed
	int StartBoundary = 2;
	int EndBoundary = 2;
	
	//initialise wave climates
	UAWaveClimate WaveClimate = UAWaveClimate(U, A, Trend,
															OffshoreMeanWavePeriod, OffshoreStDWavePeriod,
															OffshoreMeanWaveHeight, OffshoreStDWaveHeight);
	
	Wave MyWave = Wave();
	MyWave = WaveClimate.Get_Wave();

	//initialise the coastline as a straight line
	Coastline CoastVector = Coastline(MeanNodeSpacing, CoastLength, Trend, StartBoundary, EndBoundary);

	
	//int a, b; //to hold random numbers
	int CERCFlag = 1;
	int RefDiffFlag = 0;
	double FluxFraction = 0.0;
	
	//initiate random seed
	//srand (time(NULL));
	srand(2);
	
	//loop through time and evolve the coast
	CoastVector.WriteCoast(WriteCoastFile, Time);
	
	while (Time < EndTime)
	{
		//Get a new wave?
		if (Time > GetWaveTime) 
		{
			MyWave = WaveClimate.Get_Wave();
			GetWaveTime += WaveTimeDelta/365.;
		}

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
//		CoastVector.WriteCoast(WriteCoastFile, Time);
		if (Time >= PrintTime)
		{
			CoastVector.WriteCoast(WriteCoastFile, Time);
			PrintTime += PrintTimeDelta;
		}
		//cout << setprecision(8) << "Time is " << Time << endl;
	}

	//write final coastline to file
	//CoastVector.WriteCoast(WriteCoastFile, EndTime);
	
	cout 	<< endl << "Coastline: Results written to " << WriteCoastFile 
			<< endl << "Coastline: Model run complete." << endl << endl;
	
	//get execution time 
	t2=clock();
	float seconds = ((float)t2-(float)t1)/CLOCKS_PER_SEC;
	cout << "Time elapsed: " << seconds << " secs" << endl << endl;
	
	exit(EXIT_SUCCESS);
}
