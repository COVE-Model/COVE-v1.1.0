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

// We also need to include our own "libraries" these are the header files for the
// coastline object and wave climate objects that we need to run the model
#include "../coastline.hpp"
#include "../cliffline.hpp"
#include "../waveclimate.hpp"

// namespaces reduce the amount of typing we have to do and keeps code neat and tidy
using namespace std;

//This is the main program, it receives a number of input arguments
//argc is the number of arguments (always 1 larger than you think there are because
//the first argument is the progam call itself).
//argv are the arguments themselves
int main(int argc, char* argv[])
{
	// The program requires 6 input arguments, so throw an error with instructions if we don't receive them.
	// The input arguments are for a Gaussian wave climate in this case
	if (argc != 7)
	{
		cout << "Program needs 6 input arguments to generate a Gaussian waveclimate object:"
                        <<  "\n\t- MeanWavePeriod\n\t- StDWavePeriod"
												<<	"\n\t- MeanWaveHeight\n\t- StDWaveHeight"
												<<	"\n\t- MeanWaveDirection\n\t- StDWaveDirection" << endl;
		exit(EXIT_FAILURE);
	}

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
	// between two fixed headlands. When we initialise the model we will give the coastline object
	// a value for the node spacing, tell it how long the coast will be, and in what direction
	// the coast extends
	int MeanNodeSpacing = 50; 	// in metres
	double CoastLength = 2000;	// in metres
	double Trend = 315.;		// azimuth in degrees

	// There are two possible boundary types in COVE, fixed or periodic (at the moment).
	// For spiral bays, we want boundary conditions that are fixed. This means the end two nodes
	// cannot change position, and thus they allow sediment to escape the model domain by
	// alongshore flux, but do not provide any sediment back into the model.
	// We use an integer flag to specify boundary conditions, where 1 = periodic and 2 = fixed
	int StartBoundary = 2;
	int EndBoundary = 2;

	// Initialise the coastline as a straight line. With this call, we a re creating a Coastline object
	// which we will call CoastVector. To do this we need to provide this initialisation member function
	// with 5 arguments so it can setup the coastline as we want it.
	//Coastline CoastVector = Coastline(MeanNodeSpacing, CoastLength, Trend, StartBoundary, EndBoundary);

	// Alternatively we can initialise with an inputfile
	string InputFile = "spiral.in";
	double StartTime = 0;
	Coastline CoastVector = Coastline(InputFile, StartTime);
	
	// Initialise an empty/dummy cliffline object here. With this call we are creating a Cliffline object
	// but because we do not assign it (there is no = sign), it will be completely empty (i.e. there is no
   // cliff). When COVE checks to see whether there is a cliff to work with, it needs to see an empty cliff
   // object to know that no cliff exists. I'd like to find a way to put thi "under the hood" but this will
   // have to do for now.
	Cliffline CliffVector;

	// Allow refraction/diffraction rules. In our studies of spiral bay formation under different wave climates
	// we experimented with sing some very simple rules for refraction and diffraction of waves around headlands
	// into shadowed regions. We can turn this on by setting the RefDiffFlag = 1 within the model (default is
    // 0 which is turned off). We then need to call the member function to set the RefDiffFlag
	int RefDiffFlag = 1;
	CoastVector.SetRefDiffFlag(RefDiffFlag);

	//initiate random seed
	//srand (time(NULL));
	srand(3);
	
	//Declare parameter for wave conditions
	double 	OffshoreMeanWavePeriod, OffshoreStDWavePeriod, OffshoreMeanWaveDirection,
			OffshoreStDWaveDirection, OffshoreMeanWaveHeight, OffshoreStDWaveHeight;

	// Read wave parameters from argvs
	OffshoreMeanWavePeriod = atof(argv[1]);
	OffshoreStDWavePeriod = atof(argv[2]);
	OffshoreMeanWaveHeight = atof(argv[3]);
	OffshoreStDWaveHeight = atof(argv[4]);
	OffshoreMeanWaveDirection = atof(argv[5]);
	OffshoreStDWaveDirection = atof(argv[6]);

	// initialise the wave climate
	GaussianWaveClimate WaveClimate = GaussianWaveClimate(	OffshoreMeanWavePeriod,
		OffshoreStDWavePeriod, OffshoreMeanWaveDirection, OffshoreStDWaveDirection,
		OffshoreMeanWaveHeight, OffshoreStDWaveHeight);

	// declare an individual wave (this will be sampled from the wave climate object
	Wave MyWave = Wave();
	
	// Get a wave from the wave climate object
	MyWave = WaveClimate.Get_Wave();

	//create output filename based on input conditions
	string FileName = "Spiral";
	string Underscore = "_";
	string Extension = ".xy";
	string arg1 = argv[1];
	string arg2 = argv[2];
	string arg3 = argv[3];
	string arg4 = argv[4];
	string arg5 = argv[5];
	string arg6 = argv[6];
	string WriteCoastFile = FileName+Underscore+arg1+Underscore+arg2+Underscore+arg3+Underscore+arg4+Underscore+arg5+Underscore+arg6+Extension;

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
