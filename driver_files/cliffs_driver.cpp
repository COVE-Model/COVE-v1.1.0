/*==============================================================

cliffs_driver.hpp

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
#include <vector>
#include <time.h>
#include "../coastline.hpp"
#include "../cliffline.hpp"
#include "../waveclimate.hpp"

using namespace std;

int main(int argc, char* argv[])
{
	cout << endl << "Coastline/Cliffline: Initialising Model..." << endl;
	
	//declare time control paramters
	int EndTime = 20.;									// End time (years)
	double Time = 0.;										// Start Time (years)
	double PrintTimeDelta = 10./365.;	// how often to print coastline (years)
	double PrintTime = 0;	              // Print time (years)
	
	double WaveTimeDelta = 0.1;					// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.;						// Time to get a new wave (days)
	double TimeStep = 0.1;							// Time step (days)
	double MaxTimeStep = 0.1;						// Maximum timestep (days)	
	
	//initialise the coastline and cliffline objects from file
	string CliffInFile = "CuspateCliff.xy";
	string CoastInFile = "CuspateCoast.xy";
	double StartTime = 0;
	Cliffline CliffVector = Cliffline(CliffInFile, StartTime);
	Coastline CoastVector = Coastline(CoastInFile, StartTime);
	
	//declare output files
	string WriteCoastFile = "CliffedCoast_Coastline.xy";
	string WriteCliffFile = "CliffedCoast_Cliffline.xy";
	
	//Setup wave climate
	double U = 0.6;
	double A = 0.75;
	double Trend = 180.;
	double OffshoreMeanWavePeriod = 6.;
	double OffshoreStDWavePeriod = 0.;
	double OffshoreMeanWaveHeight = 1.;
	double OffshoreStDWaveHeight = 0.;
	UAWaveClimate WaveClimate = UAWaveClimate(U, A, Trend,
															OffshoreMeanWavePeriod, OffshoreStDWavePeriod,
															OffshoreMeanWaveHeight, OffshoreStDWaveHeight);
	Wave MyWave = Wave();
	MyWave = WaveClimate.Get_Wave();
															
	//Set Maximum Retreat Rate (m/yr)
  double RetreatRate = 3.;
	CliffVector.SetMaxRetreatRate(RetreatRate);

	//Set Cliff Height (m)
	double CliffHeight = 5.;
	CliffVector.SetCliffHeight(CliffHeight);

  //Set critical beach width (m)
  double CriticalBeachWidth = 10.;
  CliffVector.SetCriticalWidth(CriticalBeachWidth);

  //Set lost cliff fraction
  double LostFraction = 0.5;
  CliffVector.SetLostFraction(LostFraction);
  
  //Write waves
  //write list of shadows to file
	ofstream WriteWaveOut;
	WriteWaveOut.open("Waves.txt");
	
  //MAIN PROGRAM LOOP: Loop through time evolving the coast
  while (Time < EndTime)
  {
    //Get a new wave?
		if (Time > GetWaveTime) 
		{
			MyWave = WaveClimate.Get_Wave();
			GetWaveTime += WaveTimeDelta/365.;
		}
//		WriteWaveOut << MyWave.Get_WaveDirection() << endl;
		
		//Evolve coast
		CoastVector.TransportSediment(TimeStep, MyWave, CliffVector);
		//CliffVector.ErodeCliff(TimeStep, CoastVector);
		Time += TimeStep/365.;
		TimeStep = MaxTimeStep;
		
		//print to file?
		if (Time >= PrintTime)
		{
			CoastVector.WriteCoast(WriteCoastFile, PrintTime);
			CliffVector.WriteCliff(WriteCliffFile, PrintTime);
			PrintTime += PrintTimeDelta;
		}
		
  }
  WriteWaveOut.close();
  //write final coastline to file
	CoastVector.WriteCoast(WriteCoastFile, EndTime);
	CliffVector.WriteCliff(WriteCliffFile, EndTime);
	cout << endl << "Coastline/Cliffline: Model run complete." << endl << endl;
			
  exit(EXIT_SUCCESS);
}
