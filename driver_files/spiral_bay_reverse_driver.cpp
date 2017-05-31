/*==============================================================

spiral_bay_driver.hpp

A driver function to simulate the evolution of a crenulate or
spiral bay from a straight coastline, forced by a Gaussian
wave climate.

Developed by:
Martin D. Hurst
Andrew Barkwith
Michael A. Ellis
Christopher W. Thomas
A. Brad Murray

Copyright (C) 2015, Martin Hurst

Developer can be contacted:
martin.hurst@glasgow.ac.uk

Martin D. Hurst
University of Glasgow
British Geological Survey

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
evolves it driven by a Gaussian distribution wave climate. The model
boundaries are fixed to represent headlands or seawalls. Sediment is
allowed to escape around these fixed ends, but none is provided into the
model domain.
@date 28/10/2015
*/

// C++ requires us to include various libraries that we want to use
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "../coastline.hpp"
#include "../cliffline.hpp"
#include "../waveclimate.hpp"

// namespaces reduce the amount of typing we have to do and keeps code neat and tidy
using namespace std;

int main()
{
	// DECLARATIONS
	// These are the parameters that control the model run
    // Time control parameters
	int EndTime = 30.;					// End time (years)
	double Time = 0.;					// Start Time (years)
	double PrintTimeDelta = 36.5/365.;  // how often to output the coastline to file (years) (make monthly?)
	double PrintTime = PrintTimeDelta;	// When will we next print time (years)

  double TimeStep = 0.1;				// Time step (days)
	double WaveTimeDelta = 0.1;			// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.0;			// Time to get a new wave (years)

	// For spiral bay runs, we initialise the coast as straight line with low amp noise
	// between two fixed headlands. 
	int MeanNodeSpacing = 50; 	// in metres
	double CoastLength = 2000;	// in metres
	double Trend = 315.;		// azimuth in degrees

	// Specify boundary conditions, where 1 = periodic and 2 = fixed
	int StartBoundary = 2;
	int EndBoundary = 2;

	// Initialise the coastline as a straight line. 
	Coastline CoastVector = Coastline(MeanNodeSpacing, CoastLength, Trend, StartBoundary, EndBoundary);

	// Alternatively we can initialise with an inputfile
//	string InputFile = "spiral_in.in";
//	double StartTime = 0;
//	Coastline CoastVector = Coastline(InputFile, StartTime);
	
	// Initialise an empty/dummy cliffline object here. 
	Cliffline CliffVector;

	// Allow refraction/diffraction rules. 
	int RefDiffFlag = 1;
	CoastVector.SetRefDiffFlag(RefDiffFlag);

	//initiate random seed
	//srand (time(NULL));
	srand(3);
	
	//Declare parameter for wave conditions
	double OffshoreMeanWavePeriod = 6.;
	double OffshoreStDWavePeriod = 1.;
	double OffshoreMeanWaveHeight = 1.;
	double OffshoreStDWaveHeight = 0.1;
	double OffshoreMeanWaveDirection = 205.;
	double OffshoreStDWaveDirection = 25.;

	// initialise the wave climate
	GaussianWaveClimate WaveClimate = GaussianWaveClimate(	OffshoreMeanWavePeriod,
		OffshoreStDWavePeriod, OffshoreMeanWaveDirection, OffshoreStDWaveDirection,
		OffshoreMeanWaveHeight, OffshoreStDWaveHeight);

	// declare an individual wave (this will be sampled from the wave climate object
	Wave MyWave = Wave();
	
	// Get a wave from the wave climate object
	MyWave = WaveClimate.Get_Wave();

	//create output filename based on input conditions
	string WriteCoastFile = "Spiral_Test.xy";

	// Write initial coast to file
	CoastVector.WriteCoast(WriteCoastFile, Time);

	// loop through time and evolve the coast
	while (Time < EndTime)
	{
		//Get a new wave?
		if (Time > GetWaveTime)
		{
			MyWave = WaveClimate.Get_Wave();
			GetWaveTime += WaveTimeDelta/365.;
		}

		//Evolve coast
		CoastVector.TransportSediment(TimeStep, MyWave, CliffVector);

		//update time
		Time += TimeStep/365.;

		//Write results to file
		if (Time >= PrintTime)
		{
			CoastVector.WriteCoast(WriteCoastFile, Time);
			PrintTime += PrintTimeDelta;
		}
	}

	cout 	<< endl << "Coastline: Results written to " << WriteCoastFile
			  << endl << "Coastline: Model run complete." << endl << endl;

	exit(EXIT_SUCCESS);
}
