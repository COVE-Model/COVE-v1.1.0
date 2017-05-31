/*==============================================================

straight_periodic_driver.hpp

A driver function to simulate the evolution of cusps, capes and
spits from an initially straight coastline.

Developed by:
Martin D. Hurst
Andrew Barkwith
Michael A. Ellis
Christopher W. Thomas
A. Brad Murray

Copyright (C) 2016, Martin Hurst

Developer can be contacted:
martin.hurst@glasgow.ac.uk

Martin D. Hurst
University of Glasgow
Formerly British Geological Survey

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

/** @file straight_periodic_driver.cpp
@author Martin D. Hurst, University of Glasgow
@brief driver file for evolving a straight coastline with periodic boundary
@details This file generates a coastline object as a straightline and
evolves it driven by a UA wave climate. The model are periodic similar
to the setup of Ashton and Murray (2006) runs exploring wave climate control
on coastline morphology.
@date 2/7/2016
*/

// C++ requires us to include various libraries that we want to use
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <time.h>

// We also need to include our own "libraries" these are the header files for the
// coastline object and wave climate objects that we need to run the model
#include "./../coastline.hpp"
#include "./../waveclimate.hpp"

// namespaces reduce the amount of typing we have to do and keeps code neat and tidy
using namespace std;

//This is the main program, it receives a number of input arguments
//argc is the number of arguments (always 1 larger than you think there are because
//the first argument is the progam call itself).
//argv are the arguments themselves
int main(int argc, char* argv[])
{

	// The program requires 2 input arguments, so throw an error with instructions if we don't receive them.
	// The input arguments are for a UA wave climate in this case
	if (argc != 3)
	{
		cout << "Program needs 2 input arguments:	 \n\t- U: Wave \"highness\""
				  << "\n\t- A: Wave asymmetry" << endl;
		exit(EXIT_FAILURE);
	}
	
	//set random seed
	srand(1);
	
	// DECLARATIONS
   // These are the parameters that control the model run
   //declare time control paramters
	int EndTime = 50.;									// End time (years)
	double Time = 0.;										// Start Time (years)
	double PrintTimeDelta = 36.5/365.;	// how often to print coastline (years)
	double PrintTime = PrintTimeDelta;	// Print time (years)
	
	double WaveTimeDelta = 0.1;					// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.;						// Time to get a new wave (days)
	double TimeStep = 0.1;							// Time step (days)
	double MaxTimeStep = 0.1;						// Maximum timestep (days)	
	
	// There are two possible boundary types in COVE, fixed or periodic (at the moment).
	// We want boundary conditions that are periodic, so sediment passed out of one end 
	// of the model is provided to the other. This means the end two nodes are geometric
	// copies of each other and always experience the same amount of flux and change.
	// We use an integer flag to specify boundary conditions, where 1 = periodic and 2 = fixed
 	int StartBoundary = 1;
	int EndBoundary = 1;
	
	// for periodic boundary condition runs initialise coast as straight line with a 
	// node spacing of 100m and orientate the coast nort-south.
	double Trend = 180.; // aximuth in degrees
	int NodeSpacing = 100; // in metres
	double CoastLength = 10000; // in metres
	
	// Initialise the coastline as a straight line. With this call, we are creating a Coastline object
	// which we will call CoastVector. To do this we need to provide this initialisation member function
	// with 5 arguments so it can setup the coastline as we want it.
	Coastline CoastVector = Coastline(NodeSpacing, CoastLength, Trend, StartBoundary, EndBoundary);
	
	// Initialise an empty/dummy cliffline object here. With this call we are creating a Cliffline object
	// but because we do not assign it (there is no = sign), it will be completely empty (i.e. there is no
	// cliff). When COVE checks to see whether there is a cliff to work with, it needs to see an empty cliff
	// object to know that no cliff exists. I'd like to find a way to put thi "under the hood" but this will
	// have to do for now.
	Cliffline CliffVector;

	// setup some other model parameters, including the type of sediment flux, the diffraction flag
	// the shoreface depth and the shoreface slope. All the values here are default values so if
	// don't bother doing this bit, these are the values that will be used anyway. This is to 
	// illustrate how you might change them.
	// Declare variables
	int CERCFlag = 1;
	int RefDiffFlag = 0;
	double ShorefaceDepth = 10.;
	double ShorefaceSlope = 0.02;
	
	// Call the set public member functions to control these parameters
	CoastVector.SetRefDiffFlag(RefDiffFlag);
	CoastVector.SetFluxType(CERCFlag);	
	CoastVector.SetShorefaceDepth(ShorefaceDepth);
	CoastVector.SetShorefaceSlope(ShorefaceSlope);
	
	
	//Initialise real wave climate and temporary wave to pass
	double U = atof(argv[1]);
	double A = atof(argv[2]);
	double OffshoreMeanWavePeriod = 6.;
	double OffshoreStDWavePeriod = 1.;
	double OffshoreMeanWaveHeight = 1.;
	double OffshoreStDWaveHeight = 0.1;
	
	//initialise wave climate
	UAWaveClimate WaveClimate = UAWaveClimate(U, A, Trend,
															OffshoreMeanWavePeriod, OffshoreStDWavePeriod,
															OffshoreMeanWaveHeight, OffshoreStDWaveHeight);
	
	// declare an individual wave (this will be sampled from the wave climate object
	Wave MyWave = Wave();
	// Get a wave from the wave climate object
	MyWave = WaveClimate.Get_Wave();

	//setup output file for writing results based on wave climate params
	string arg1 = argv[1];
	string arg2 = argv[2];
	string underscore = "_";
	string WriteCoastFile = "COVE" + underscore + arg1 + underscore + arg2 + ".xy";
	
	// Write initial coast to file
	CoastVector.WriteCoast(WriteCoastFile, Time);

	//loop through time and evolve the coast
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
		Time += TimeStep/365.;
		TimeStep = MaxTimeStep;
		
		//print to file?
		//CoastVector.WriteCoast(WriteCoastFile, PrintTime);
		if (Time >= PrintTime)
		{
			CoastVector.WriteCoast(WriteCoastFile, PrintTime);
			PrintTime += PrintTimeDelta;
		}
	}

	//write final coastline to file
	CoastVector.WriteCoast(WriteCoastFile, EndTime);
	cout 	<< endl << "Coastline: Results written to " << WriteCoastFile 
			<< endl << "Coastline: Model run complete." << endl << endl;
	
	exit(EXIT_SUCCESS);
}
