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
Drive file for exploring the influence of wave climates on the formation and fate of cuspate forelands.

Martin Hurst, March 2015
*/

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << "Program needs 2 input arguments:	 \n\t- Mean Wave Direction"
				  << "\n\t- Std. Dev. Wave Direction" << endl;
		exit(EXIT_FAILURE);
	}
	
	//declare time control paramters
	int EndTime = 20.;									// End time (years)
	double Time = 0.;										// Start Time (years)
	double PrintTimeDelta = 10./365.;		// how often to print coastline (years)
	double PrintTime = PrintTimeDelta;	// Print time (years)
	
	double WaveTimeDelta = 0.1;					// Frequency at which to sample new waves (days)
	double GetWaveTime = 0.;						// Time to get a new wave (days)
	double TimeStep = 0.1;							// Time step (days)
	double MaxTimeStep = 0.1;						// Maximum timestep (days)	
	
	//int a, b; //to hold random numbers
	int CERCFlag = 1;
	int RefDiffFlag = 0;
	double FluxFraction = 0.;
	double LostFluxFraction = 0.01;

  //  //Boundary Conditions
  // 	int StartBoundary = 1;
  //	int EndBoundary = 1;
	
	//initialise coast
  //	double Trend = 180.;
  //	int NodeSpacing = 100;
  //	double CoastLength = 10000;
  //	Coastline CoastVector = Coastline(NodeSpacing, CoastLength, Trend, StartBoundary, EndBoundary);
	
	//intialise coast from file
	string xyfilename = "CuspateCoast.xy";
	float StartTime = 0;
	Coastline CoastVector = Coastline(xyfilename, StartTime);

	string arg1 = argv[1];
	string arg2 = argv[2];
	string underscore = "_";
	string WriteCoastFile = "Cuspate" + underscore + arg1 + underscore + arg2 + ".xy";
	
	//Initialise real wave climate and temporary wave to pass
	double OffshoreMeanWaveDirection = atof(argv[1]);
	double OffshoreStDWaveDirection = atof(argv[2]);
	double OffshoreMeanWavePeriod = 6.;
	double OffshoreStDWavePeriod = 1.;
	double OffshoreMeanWaveHeight = 1.;
	double OffshoreStDWaveHeight = 0.2;
	
	//initialise wave climate
	GaussianWaveClimate WaveClimate = GaussianWaveClimate(
															OffshoreMeanWavePeriod, OffshoreStDWavePeriod,
															OffshoreMeanWaveDirection, OffshoreStDWaveDirection,
															OffshoreMeanWaveHeight, OffshoreStDWaveHeight);
	
	Wave MyWave = Wave();
	MyWave = WaveClimate.Get_Wave();
	
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
		CoastVector.TransportSediment(TimeStep, MyWave, CERCFlag, RefDiffFlag, FluxFraction, LostFluxFraction);
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
