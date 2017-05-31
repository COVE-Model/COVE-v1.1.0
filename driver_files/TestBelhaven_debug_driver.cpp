/*==============================================================

benacre_driver.hpp

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
	
	double WaveTimeDelta = 0.2;					// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.;						// Time to get a new wave (days)
	double TimeStep = 0.2;							// Time step (days)
	double MaxTimeStep = 0.2;						// Maximum timestep (days)	
	
	// initialise the coastline and cliffline objects from file
	// first declare the filenames
	string CliffInFile = "TestBelhaven_Cliffline_Points.xy";
	string CoastInFile = "Belhaven_Debug_Coastline.xy";
	string FixedFileName = "TestBelhaven_Fixed_Cliffs.data";
	
	// Setup wave climates
	// We know from wave buoy data that this stretch of coastline experiences a bimodal wave climate
	// but that the buoys are in deeper water than we would like. We will use an idealised bimodal
	// wave climate characterised by two Gaussian distributions, and sample from them at random
	
	// Bimodal wave climate
	//Wave climate 1
	double OffshoreMeanWaveDirection1 = 45.;
	double OffshoreStDWaveDirection1 = 20.;
	double OffshoreMeanWavePeriod1 = 6.;
	double OffshoreStDWavePeriod1 = 2.;
	double OffshoreMeanWaveHeight1 = 0.8;
	double OffshoreStDWaveHeight1 = 0.2;
	GaussianWaveClimate WaveClimate1(OffshoreMeanWavePeriod1,OffshoreStDWavePeriod1,OffshoreMeanWaveDirection1,OffshoreStDWaveDirection1,OffshoreMeanWaveHeight1, OffshoreStDWaveHeight1);
	
	//Wave climate 2
	double OffshoreMeanWaveDirection2 = 90.;
	double OffshoreStDWaveDirection2 = 20.;
	double OffshoreMeanWavePeriod2 = 5.;
	double OffshoreStDWavePeriod2 = 1.;
	double OffshoreMeanWaveHeight2 = 1.1;
	double OffshoreStDWaveHeight2 = 0.2;
	GaussianWaveClimate WaveClimate2(OffshoreMeanWavePeriod2,OffshoreStDWavePeriod2,OffshoreMeanWaveDirection2,OffshoreStDWaveDirection2,OffshoreMeanWaveHeight2, OffshoreStDWaveHeight2);
  
  //declare wave	
	Wave MyWave = Wave();
	MyWave = WaveClimate1.Get_Wave();
	
	// Read the coastline and cliffline data from files
	double StartTime = 0;
	Cliffline CliffVector; // = Cliffline(CliffInFile, StartTime);
	Coastline CoastVector = Coastline(CoastInFile, StartTime);
	
	// Load data on cliff type (fixed vs erodible)
	//CliffVector.ReadCliffType(FixedFileName);
	
	//declare output file names
	string WriteCoastFile = "TestBelhavenCliffedCoast_Coastline.xy";
	//string WriteCliffFile = "TestBelhavenCliffedCoast_Cliffline.xy";
															
	// Set Maximum Retreat Rate (m/yr)
	// This is E0 for the Valvo et al. (2006) or Limber and Murray (2011) models of cliff erosion
  //double RetreatRate = 5.;
	//CliffVector.SetMaxRetreatRate(RetreatRate);

	// Set Cliff Height (m)
	// Fixed for now, but could in the future grab this from a DEM
	//double CliffHeight = 10.;
	//CliffVector.SetCliffHeight(CliffHeight);

  // Set critical beach width (m)
  // The beach width that maximises the rate of cliff retreat E0
  //double CriticalBeachWidth = 5.;
  //CliffVector.SetCriticalWidth(CriticalBeachWidth);

  // For Valvo Style Cliff Retreat ErosionType = 1, 
  // for Limber and Murray ErosionType = 2
  int ErosionType = 1;
  
  // Set lost cliff fraction
  // The proportion of cliff material that is lost offshore by suspension
  //double LostFraction = 0.2;
  //CliffVector.SetLostFraction(LostFraction);
  

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
			rand1 = (double)rand()/RAND_MAX;
			if (rand1 < 0.5) MyWave = WaveClimate1.Get_Wave();
			else MyWave = WaveClimate2.Get_Wave();
			GetWaveTime += WaveTimeDelta/365.;
		}
		
		//Evolve coast
		CoastVector.TransportSediment(TimeStep, MyWave, CliffVector);
		//CliffVector.ErodeCliff(TimeStep, CoastVector, ErosionType);
		Time += TimeStep/365.;
		TimeStep = MaxTimeStep;
		
		//print to file?
		if (Time >= PrintTime)
		{
			CoastVector.WriteCoast(WriteCoastFile, PrintTime);
			//CliffVector.WriteCliff(WriteCliffFile, PrintTime);
			PrintTime += PrintTimeDelta;
		}
  }
  //WriteWaveOut.close();
    
  //write final coastline to file
	CoastVector.WriteCoast(WriteCoastFile, EndTime);
	//CliffVector.WriteCliff(WriteCliffFile, EndTime);
	cout << endl << "Coastline/Cliffline: Model run complete." << endl << endl;
			
  exit(EXIT_SUCCESS);
}
