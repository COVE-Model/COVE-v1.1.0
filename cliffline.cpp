/*==============================================================

Cliffline.cpp

The Cliffline object
Cliffline is a vector based object defining the location
and paramaters of a Cliff at the coast line.

Developed by:
Martin D. Hurst
Andrew Barkwith
Michael A. Ellis
Christopher W. Thomas
A. Brad Murray

Copyright (C) 2016, Martin Hurst

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

#ifndef cliffline_CPP
#define cliffline_CPP

#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <omp.h>
#include <queue>
#include "coastline.hpp"
#include "cliffline.hpp"
#include "waveclimate.hpp"

using namespace std;


/********************************************
* Cliffline Object Initialisation Functions *
********************************************/

/**Empty initialiser which will throw an error, arguments are required when initialising a Cliffline object
* MDH 4/1/16 */
void Cliffline::Initialise()
{
	printf("\nCliffline.Initialise: Warning, initialised an empty Cliffline object\n");
	//exit(EXIT_FAILURE);
}
/**Initialise the Cliffline with an xyfile. File format is a headerline containing the start and end boundary 
conditions (white-space delimited) followed by paired rows in which the first element is Time followed by a 
vector of x and y respectively with which to initialise the Cliffline object. Since no start time is specified 
this initialiser defaults to StartTime=0
* MDH 6/1/14 */
void Cliffline::Initialise(string xyfilename)
{

	/*	Read Cliffline.XY text file */
	cout << "\nCliffline.Initialise: Initialising Cliffline from XY file: " << xyfilename << endl;
	;

	float Time=0;
	ReadCliff(xyfilename, Time);

	//Populate empty vectors
	vector<double> EmptyVector(NoNodes, -9999);
	Distance = EmptyVector;
	FluxOrientation = EmptyVector;
	Orientation = EmptyVector;
	CellWidth = EmptyVector;
	e1 = EmptyVector;
	e2 = EmptyVector;
		
	vector<int> Zeros(NoNodes, 0);
	Fixed = Zeros;
	
	//Calculate Orientation, Slope and Curvature
	CalculateMorphology();
	CalculateMeanNodeSpacing();
	DesiredNodeSpacing = MeanNodeSpacing;

}
/**Initialise the Cliffline with an xyfile and a start time. File format is a headerline containing the start
and end boundary conditions (white-space delimited) followed by paired rows in which the first element is Time 
followed by a vector of x and y respectively with which to initialise the Cliffline object. This allows the 
initialisation of a Cliffline from previous model output.
* MDH 6/1/14 */
void Cliffline::Initialise(string xyfilename, float StartTime)
{

	/*	Read Cliffline.XY text file */
	cout << "\nCliffline.Initialise: Initialising Cliffline from XY file: " << xyfilename << " at Time: " << StartTime << endl;
	ReadCliff(xyfilename, StartTime);
	
	//Populate empty vectors
	vector<double> EmptyVector(NoNodes, -9999);
	Distance = EmptyVector;
	FluxOrientation = EmptyVector;
	Orientation = EmptyVector;
	CellWidth = EmptyVector;
	BeachWidth = EmptyVector;
	PositionChange = EmptyVector;
	VolumeChange = EmptyVector;
	NearestBeachIndex = EmptyVector;
	e1 = EmptyVector;
	e2 = EmptyVector;
	
	vector<int> Zeros(NoNodes, 0);
	Fixed = Zeros;
	
	if (StartBoundary == 2) Fixed[0] = 1, Fixed[1] = 1;
	if (EndBoundary == 2) Fixed[NoNodes-1] = 1, Fixed[NoNodes-2] = 1;
		
	//Calculate Orientation, Slope and Curvature
	CalculateMorphology();
	CalculateMeanNodeSpacing();
	DesiredNodeSpacing = MeanNodeSpacing;
}

/** Initialise the Cliffline object as a straight line (with small amplitude random noise) by specifying
the mean node spacing (in metres) the total length of the coast (in metres), the trend of the Cliffline 
(azimuth direction with sea to the left as you look down this line) and the type of start and end boundary 
conditions.
* MDH 6/1/14 */
void Cliffline::Initialise(int NodeSpacing, double CoastLength, double TheTrend, int StartBoundaryInput, int EndBoundaryInput)
{

	/*	Initialises the coast as a straight segment with low amplitude noise
		Coast has a fixed length and an orientation/trend. Coordinates of the
		first node in the arrays is [0][0].
		Boundary conditions must be specified */

	//setup boundary conditons
	StartBoundary = StartBoundaryInput;
	EndBoundary = EndBoundaryInput;
	
	cout 	<< "\nCliffline: Initialising Cliffline as straight segment"
			<< "\n\t MeanNodeSpacing = " << NodeSpacing
			<< "\n\t CoastLength = " << CoastLength
			//<< "\n\t Trend = " << Trend
			<< "\n\t Boundary Conditions = " << StartBoundary << ", " << EndBoundary << endl << endl;

	//declare temporary variables for setting up Cliffline object
	Trend = TheTrend;
	double tempX, tempY;
	NoNodes = 0;
	double Dist = 0;
	MeanNodeSpacing = (double)NodeSpacing;
	DesiredNodeSpacing = MeanNodeSpacing;
  double NoiseAmplitude =(MeanNodeSpacing/10.);
	
	//initialise coast
	while (Dist<CoastLength)
	{
		if (Dist==0)
		{
			//First node
			tempX = 0; tempY = 0;
			X.push_back(tempX);
			Y.push_back(tempY);
			NoNodes += 1;
			
			if (StartBoundary == 2)
			{
			  //fix the sea wall and length relative to the bay size
			  tempX = tempX + (CoastLength/5.)*sin(Trend*M_PI/180); // + NoiseAmplitude*(-0.5+((double)rand()/RAND_MAX));
			  tempY = tempY + (CoastLength/5.)*cos(Trend*M_PI/180); // + NoiseAmplitude*(-0.5+((double)rand()/RAND_MAX));
			  Dist += 1.0;
			  X.push_back(tempX);
			  Y.push_back(tempY);
			  NoNodes += 1;
			}
			else
			{
        //sort out random addition
	      tempX = tempX + MeanNodeSpacing*sin(Trend*M_PI/180) + NoiseAmplitude*(-0.5+((double)rand()/RAND_MAX));
	      tempY = tempY + MeanNodeSpacing*cos(Trend*M_PI/180) + NoiseAmplitude*(-0.5+((double)rand()/RAND_MAX));
	      X.push_back(tempX);
	      Y.push_back(tempY);
	      NoNodes += 1;
	      Dist += MeanNodeSpacing;
			}
		}
		else
		{
			//sort out random addition
			tempX = tempX + MeanNodeSpacing*sin(Trend*M_PI/180) + NoiseAmplitude*(-0.5+((double)rand()/RAND_MAX));
			tempY = tempY + MeanNodeSpacing*cos(Trend*M_PI/180) + NoiseAmplitude*(-0.5+((double)rand()/RAND_MAX));
			X.push_back(tempX);
			Y.push_back(tempY);
			NoNodes += 1;
			Dist += MeanNodeSpacing;
		}
	}
	
	if (EndBoundary == 2)
	{
    //Make sure downcoast boundary is in the right place
	  tempX = (CoastLength+(CoastLength/5.))*sin(Trend*M_PI/180);
	  tempY = (CoastLength+(CoastLength/5.))*cos(Trend*M_PI/180);
	  X.push_back(tempX);
	  Y.push_back(tempY);
	  NoNodes += 1;
	
	  //Add final Node
    tempX = tempX + (CoastLength/5.)*sin(Trend*M_PI/180);
    tempY = tempY + (CoastLength/5.)*cos(Trend*M_PI/180);
    X.push_back(tempX);
    Y.push_back(tempY);
    NoNodes += 1;
  }
	
	//Populate empty vectors
	vector<double> EmptyVector(NoNodes, -9999);
	Distance = EmptyVector;
	FluxOrientation = EmptyVector;
	Orientation = EmptyVector;
	PositionChange = EmptyVector;
	VolumeChange = EmptyVector;
	CellWidth = EmptyVector;
	BeachWidth = EmptyVector;
	NearestBeachIndex = EmptyVector;
	e1 = EmptyVector;
	e2 = EmptyVector;
	
	vector<int> Zeros(NoNodes, 0);
	Fixed = Zeros;
	
	if (StartBoundary == 2) Fixed[0] = 1, Fixed[1] = 1;
	if (EndBoundary == 2) Fixed[NoNodes-1] = 1, Fixed[NoNodes-2] = 1;
	
	//Calculate Orientation, Slope and Curvature
	CalculateMorphology();
}

//Reads a vector of whether the cliff is fixed or unfixed (i.e. protected)
void Cliffline::ReadCliffType(string InputFileName)
{
  int Temp;
  
  //Open the file in a filestream, check the file exists and read data into X and Y
	ifstream ReadCliffFile;
	ReadCliffFile.open(InputFileName.c_str());
	if (ReadCliffFile.is_open())
	{
		cout << "Cliffline.ReadCliffType: Reading cliff conditions" << endl << endl;
		for (int i=0; i<NoNodes; ++i)
		{
		  ReadCliffFile >> Temp >> Fixed[i];
		}
	}
	else
	{
		cout << "Cliffline.ReadCliffType: No Cliff Type File" << endl << endl;
		exit(EXIT_FAILURE);
	}
}

/**Reads a Cliffline x and y coordinates from an inputfile at the specified time. Called by the initialiser functions.
* MDH 6/1/14 */
void Cliffline::ReadCliff(string InputFileName, double Time)
{
	/*	Reads a Cliffline object from a text file for a specific time (default is 0)
		File format is
								Start Boundary | End Boundary
								Time | X[0] | X[1] | X[2] =====> X[NoNodes]
								Time | Y[0] | Y[1] | Y[2] =====> Y[NoNodes]	*/

	//Params
	int FindTime = 1;
	float TempTime;
	double TempX, TempY;
	string LineString, line;
	NoNodes = 0;

	//Open the file in a filestream, check the file exists and read data into X and Y
	ifstream ReadCoastFile;
	ReadCoastFile.open(InputFileName.c_str());
	if (ReadCoastFile.is_open())
	{
		ReadCoastFile >> StartBoundary >> EndBoundary;

		//Write Boundary Conditions to screen
		if 			(StartBoundary == 1 && EndBoundary == 1) cout << "Cliffline.ReadCoast: Boundaries are periodic" << endl << endl;
		else if 	(StartBoundary == 2 && EndBoundary == 2) cout << "Cliffline.ReadCoast: Boundaries are fixed" << endl << endl;
		else if 	(StartBoundary == 3 && EndBoundary == 3) cout << "Cliffline.ReadCoast: Boundaries are prescribed" << endl << endl;
		else
		{
			cout 	<< "Cliffline.ReadCoast: Error in boundary conditions:"
					<< "\n\tBoundaries must be specified as either"
					<< "\n\t\t1 = periodic, \n\t\t2 = fixed, \n\t\t3 = prescribed"
					<< "\n\tCurrently both start and end boundary must be the same type." << endl << endl;
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		cout << "Cliffline.ReadCoast: Error, the file " << InputFileName << " has not been read correctly." << endl;
		exit(EXIT_FAILURE);
	}

	//Loop through lines looking for the desired time
	while (FindTime == 1)
	{
		if (ReadCoastFile.is_open())
		{
			ReadCoastFile >> TempTime;

			//search for first instance of correct time
			if (Time == TempTime)
			{
				getline(ReadCoastFile, line);
				stringstream iss;
				iss << line;

				while (iss >> TempX)
				{
					X.push_back(TempX);
					NoNodes += 1;
				}

				iss << "";
				iss.clear();

				ReadCoastFile >> TempTime;
				getline(ReadCoastFile, line);

				iss << line;

				while (iss >> TempY) Y.push_back(TempY);

				FindTime = 0;
			}

			//otherwise discard lines
			else
			{
				getline(ReadCoastFile, line);
				getline(ReadCoastFile, line);
			}
		}
		else
		{
			cout << "Cliffline.ReadCoast: Error, the file " << InputFileName << " has not been read correctly." << endl;
			exit(EXIT_FAILURE);
		}
	}
}

/**Writes the time and a Cliffline x and y coordinates from the Cliffline object to an outputfile. For saving model results.
* MDH 6/1/14 */
void Cliffline::WriteCliff(string OutputFileName, double Time)
{
	/*	Writes a Cliffline object X and Y coordinates to file for a given time
		If the file already exists the data will be ed else a new file is
		created.

		File format is 	Time | X[0] | X[1] | X[2] =====> X[NoNodes]
								Time | Y[0] | Y[1] | Y[2] =====> Y[NoNodes]			*/

	//Print to screen
	cout.flush();
	cout << "Cliffline: Time is " << setprecision(2) << fixed << Time << " years\r";

	//test if output file already exists
	int FileExists = 0;
	ifstream oftest(OutputFileName.c_str());
	if (oftest) FileExists = 1;
	oftest.close();

	//open the output filestream and write headers
	ofstream WriteCoastFile;
	if (FileExists == 0)
	{
		WriteCoastFile.open(OutputFileName.c_str());
		if (WriteCoastFile.is_open()) WriteCoastFile << StartBoundary << " " << EndBoundary << endl;
	}
	WriteCoastFile.close();

	//open output filestream again to  Cliffline data
	WriteCoastFile.open(OutputFileName.c_str(), fstream::app|fstream::out);

	//Check if file exists if not open a new one and write headers
	if (WriteCoastFile.is_open())
	{
		//write X
		WriteCoastFile << setprecision(4) << Time;
		for (int i=0; i<NoNodes; ++i) WriteCoastFile << setprecision(10) << " " << X[i];
		WriteCoastFile << endl;

		//write Y
		WriteCoastFile << setprecision(4) << Time;
		for (int i=0; i< NoNodes; ++i) WriteCoastFile << setprecision(10) << " " << Y[i];
		WriteCoastFile << endl;
	}
	
	else
	{
		//report errors
		cout << "Cliffline.WriteCoast: Error, the file " << OutputFileName << " is not open or cannot be read." << endl;
		exit(EXIT_FAILURE);
	}
}

void Cliffline::CalculateMorphology()
{
	/************************************************************
	Resets the coastline morphological vectors and recalculates a variety
	of geometric properties and metrics. Calculates Distance along the coast, Orientation 
	(3-cell) of the coastline, the orientation of cell boundaries 
	(e1 and e2), and the CellWidth from the X and Y vectors.
	Orientation is the direction of vector i-1 to i+1
	************************************************************/

	//declare some temporary variables
	double dX = 0;
	double dY = 0;
	double SpatialGradient;
	double LeftDiff, RightDiff;

	//Reset all vectors (could builda function for this)
	vector<double> EmptyVector(NoNodes, -9999);
	Distance = EmptyVector;
	FluxOrientation = EmptyVector;
	Orientation = EmptyVector;
	CellWidth = EmptyVector;
	PositionChange = EmptyVector;
	VolumeChange = EmptyVector;
	BeachWidth = EmptyVector;
	NearestBeachIndex = EmptyVector;
	e1 = EmptyVector;
	e2 = EmptyVector;
	
	for (int i=0; i<NoNodes; ++i)
	{
		//Handle StartBoundary
		if (i==0)
		{
			//First get flux orientation (2-node)
			dX = X[1]-X[0];
			dY = Y[1]-Y[0];
			Distance[i] = 0;
			Distance[i+1] = sqrt(dX*dX + dY*dY);
			
			SpatialGradient = dY/dX;
			//convert to azimuths
			if (dX == 0 && dY < 0) FluxOrientation[i] = 180.;
			else if (dX == 0 && dY > 0) FluxOrientation[i] = 0.;
			else if (dX > 0) FluxOrientation[i] = (180./M_PI)*(M_PI*0.5 - atan(SpatialGradient));
			else if (dX < 0) FluxOrientation[i] = (180./M_PI)*(M_PI*1.5 - atan(SpatialGradient));
			else 
			{
				cout << "This is impossible!" << endl;
			}
			
			//Next get orientation (3-node)
			//PERIODIC BOUNDARY
			if (StartBoundary == 1)
			{
				dX = X[NoNodes-1]-X[NoNodes-2]+X[1]-X[0];
				dY = Y[NoNodes-1]-Y[NoNodes-2]+Y[1]-Y[0];
			}
			else if (StartBoundary == 2)
			{
				dX = 2*dX;
				dY = 2*dY;
			}			
			
			//convert to azimuths
			SpatialGradient = dY/dX;
			if (dX == 0 && dY < 0) Orientation[i] = 180.;
			else if (dX == 0 && dY > 0) Orientation[i] = 0.;
			else if (dX > 0) Orientation[i] = (180./M_PI)*(M_PI*0.5 - atan(SpatialGradient));
			else if (dX < 0) Orientation[i] = (180./M_PI)*(M_PI*1.5 - atan(SpatialGradient));
			else 
			{
				cout << "This is impossible!" << endl;		
			}
			
			//get other properties
			if (StartBoundary == 1)
			{
				//e1 and Cell Width will be calculated when handling the end boundary for periodic
				e2[i] = FluxOrientation[i] - Orientation[i];
			}
			else if (StartBoundary == 2)
			{
        e1[i] = 0;
        e2[i] = 0;
        LeftDiff = (0.5*sqrt(pow(X[i+1]-X[i],2.0) + pow(Y[i+1]-Y[i],2.0)))/cos((M_PI/180.)*(e1[i]));
        RightDiff = LeftDiff;
        CellWidth[i] = LeftDiff+RightDiff;
			}
		}

		//Deal with EndBoundary
		else if (i==NoNodes-1)
		{
			//PERIODIC BOUNDARY
			if (EndBoundary == 1)
			{
				//First get flux orientation (2-node)
				FluxOrientation[i] = FluxOrientation[0];
				Orientation[i] = Orientation[0];
				e1[i] = Orientation[i]-FluxOrientation[i-1];
				e2[i] = e2[0];
				LeftDiff = (0.5*sqrt(pow(X[i]-X[i-1],2.0) + pow(Y[i]-Y[i-1],2.0)))/cos((M_PI/180.)*(e1[i]));
				RightDiff = (0.5*sqrt(pow(X[1]-X[0],2.0) + pow(Y[1]-Y[0],2.0)))/cos((M_PI/180.)*(-e2[i]));
				
				//Cell Width parallel to orientation AT the node position
				CellWidth[i] = LeftDiff+RightDiff;
			}
			
			//FIXED BOUNDARY
			else if (EndBoundary == 2)
			{
        e1[i] = 0;
        e2[i] = 0;
        FluxOrientation[i] = FluxOrientation[i-1];
        Orientation[i] = Orientation[i-1];

        LeftDiff = (0.5*sqrt(pow(X[i]-X[i-1],2.0) + pow(Y[i]-Y[i-1],2.0)))/cos((M_PI/180.)*(e1[i]));
        RightDiff = LeftDiff;
        CellWidth[i] = LeftDiff+RightDiff;
			}
		}
		
		//Finally do all the bits inbetween
		else
		{
			//First get flux orientation (2-node)
			dX = X[i+1]-X[i];
			dY = Y[i+1]-Y[i];
			Distance[i+1] = Distance[i] + sqrt(dX*dX + dY*dY);
			
			//don't allow divide by zero, just make spatial gradient very large
			if (dX == 0) dX = 0.0001;
			SpatialGradient = dY/dX;
			//convert to azimuths
			if (dX == 0 && dY < 0) FluxOrientation[i] = 180.;
			else if (dX == 0 && dY > 0) FluxOrientation[i] = 0.;
			else if (dX > 0) FluxOrientation[i] = (180./M_PI)*(M_PI*0.5 - atan(SpatialGradient));
			else if (dX < 0) FluxOrientation[i] = (180./M_PI)*(M_PI*1.5 - atan(SpatialGradient));
			else 
			{
				printf("Failed to compute cell flux orientation in %s at line %d\n",__func__,__LINE__);
			  exit(EXIT_FAILURE);
			}

			//Next get orientation (3-node)
			dX = (X[i+1]-X[i-1]);
			dY = (Y[i+1]-Y[i-1]);
			//convert to azimuths
			if (dX == 0 && dY < 0) Orientation[i] = 180.;
			else if (dX == 0 && dY > 0) Orientation[i] = 0.;
			else if (dX > 0) Orientation[i] = (180./M_PI)*(M_PI*0.5 - atan(dY/dX));
			else if (dX < 0) Orientation[i] = (180./M_PI)*(M_PI*1.5 - atan(dY/dX));
			else 
			{
			  printf("Failed to compute cell orientation in %s at line %d\n",__func__,__LINE__);
			  exit(EXIT_FAILURE);
			}

      //Get cell geometry angles
      e1[i] = Orientation[i]-FluxOrientation[i-1];
      e2[i] = FluxOrientation[i] - Orientation[i];
      
      //Get MidPoints
      LeftDiff = (0.5*sqrt((X[i]-X[i-1])*(X[i]-X[i-1]) + ((Y[i]-Y[i-1])*(Y[i]-Y[i-1]))))/cos((M_PI/180.)*(e1[i]));
      RightDiff = (0.5*sqrt((X[i+1]-X[i])*(X[i+1]-X[i]) + ((Y[i+1]-Y[i])*(Y[i+1]-Y[i]))))/cos((M_PI/180.)*(-e2[i]));
      
      //Cell Width parallel to orientation AT the node position
      CellWidth[i] = (0.5*sqrt(pow(X[i]-X[i-1],2.0) + pow(Y[i]-Y[i-1],2.0)))/cos(M_PI/180.*(Orientation[i]-FluxOrientation[i-1]));
      CellWidth[i] += (0.5*sqrt(pow(X[i+1]-X[i],2.0) + pow(Y[i+1]-Y[i],2.0)))/cos(M_PI/180.*(Orientation[i]-FluxOrientation[i]));
	  }
  }
}

void Cliffline::CheckNodeSpacing()
{
	/* Function to check the spacing bewteen the nodes is optimal and add or delete nodes 
		as appropriate. Where nodes are <0.6*DesiredNodeSpacing a new node interpolated
		between them will be created. Where nodes are >1.5*DesiredNodeSpacing apart then
		a new node is created between them		*/
		
	//declare variables
	//double OptimalArea = MeanNodeSpacing*(ClosureDepth/ShorefaceSlope);
	int SpacingFlag = 0;
	int NodeFlag = 0;
	double Distance,X0,Y0;//MeanOrientation; //,M1,M2; //, VolumeOld, VolumeNew, VolChange, a, b, c;
	//need to recalculate morphology and start check again after each new node is added
	while (SpacingFlag == 0)
	{
		for (int i=0; i<NoNodes-1; ++i)
		{
			if (i<1 || i>NoNodes-3) continue;
			if (i == NoNodes-1) {}
			else
			{
				Distance = sqrt(pow(X[i+1]-X[i],2.0) + pow(Y[i+1]-Y[i],2.0));
				
				//if distance between two cells is too small, 
				//replace with a linearly interpolated point between
				if (Distance < 0.4*DesiredNodeSpacing)
				{
					//if cell is too narrow remove its adjacent nodes
					if ((StartBoundary == 2 && EndBoundary == 2) & (i == 1 || i == NoNodes-3))
					{
						//cout << "Erasing Node" << endl;
						X.erase(X.begin()+i+1); 
						Y.erase(Y.begin()+i+1); 
						Fixed.erase(Fixed.begin()+i+1);
						NoNodes -= 1;
						SpacingFlag = 0;
						NodeFlag = 1;
						CalculateMorphology();
					}
					else
					{
						//cout << "Erasing Node" << endl;
						X[i] = ((X[i+1]+X[i])/2.);
						Y[i] = ((Y[i+1]+Y[i])/2.);
						X.erase(X.begin()+i+1); 
						Y.erase(Y.begin()+i+1); 
						Fixed.erase(Fixed.begin()+i+1);
						NoNodes -= 1;
						SpacingFlag = 0;
						NodeFlag = 1;
						CalculateMorphology();
					}
					break;
				}
				else if (Distance > 1.5*DesiredNodeSpacing)
				{
					//if cell too far apart add new node
					X0 = (X[i]+X[i+1])/2.;
					Y0 = (Y[i]+Y[i+1])/2.;
					X.insert(X.begin()+i+1,X0);
					Y.insert(Y.begin()+i+1,Y0);
					Fixed.insert(Fixed.begin()+i+1,0.0);
					NoNodes += 1;
					SpacingFlag = 0;
					NodeFlag = 1;
					CalculateMorphology();
					break;
				}
				else SpacingFlag = 1;
			}
		}
	}
	if (NodeFlag == 1) CalculateMorphology();
}

void Cliffline::CalculateMeanNodeSpacing()
{
	//Returns an integer of the mean node spacing from X and Y data
	double Distance;
	double dx, dy;
	double TotalDistance = 0;

	for (int i=0; i<NoNodes; ++i)
	{
		//For the last cell Distance is the same as for cell i=0
		if (i==NoNodes-1)
		{
			//Calculate distance between 0 and 1
			dx = fabs(X[1]-X[0]);
			dy = fabs(Y[1]-Y[0]);
			Distance = sqrt(dx*dx + dy*dy);
		}
		else
		{
			//Calculate distance between i and i+1
			dx = fabs(X[i+1]-X[i]);
			dy = fabs(Y[i+1]-Y[i]);
			Distance = sqrt(dx*dx + dy*dy);
		}
		TotalDistance += Distance;
	}

	//Calculate MeanNodeSpacing
	MeanNodeSpacing = (int)(TotalDistance/NoNodes);
}

void Cliffline::IntersectionAnalysis()
{
	/*description goes here
	
	Function to loop through the Cliffline vector and check for interesctions due 
	to shoreline meeting up or coast eating itself.
	
	Finds the cross product between two line segments and the line connecting
	the start of the two segments in order to determine if they intersect
	
	No logic to deal with this yet, just a detection flag
	
	Martin Hurst, 4/1/2016
	
	*/
	
	//declare temporary variables
	double X1, X2, X3, X4, Y1, Y2, Y3, Y4, dX12, dY12, dX34, dY34, dX31, dY31, XProd, S, T;
	int XProdPos = 0;
	
	//#pragma omp parallel for
	for (int i=0; i<NoNodes; ++i)
	{
		//assign first segment
		X1 = X[i];
		Y1 = Y[i];
		X2 = X[i+1];
		Y2 = Y[i+1];
		dX12 = X2-X1;
		dY12 = Y2-Y1;
		
		for (int j=i+2; j<NoNodes-1; ++j)
		{
			//assign second segment
			X3 = X[j];
			Y3 = Y[j];
			X4 = X[j+1];
			Y4 = Y[j+1];
			dX34 = X4-X3;
			dY34 = Y4-Y3;
			
			//Find the cross product of the two vectors
			XProd = dX12*dY34 - dX34*dY12;
			
			if (XProd == 0) {} 	//lines are colinear/parallel
			else
			{
				if (XProd > 0) XProdPos = 1;
				else XProdPos = 0;
				
				//assign third test segment
				dX31 = X1-X3;
				dY31 = Y1-Y3;
				
				//get cross products
				S = dX12*dY31 - dY12*dX31;
				T = dX34*dY31 - dY34*dX31;
				
				//logic for collision occurence
				if ((S < 0) == XProdPos) {}
				else if ((T < 0) == XProdPos) {}
				else if (((S > XProd) == XProdPos) || ((T > XProd) == XProdPos)) {}
				else
				{
					if ((Fixed[i] == 1) && (Fixed[j] == 1)) continue;
					printf("Intersection in %s at line %d\n",__func__,__LINE__);
				}
			}
		}
	}
}

void Cliffline::DetermineBeachWidth(Coastline Coastline)
{
  //Declare parameters
  //StartSearch and EndSearch define where to look up and down the vector from the last 
  //successful measurement to find the shortest beach width
  int SearchDistance = 5;
  int StartSearch = 0;
  int EndSearch = 0;
  
  //Parameters for dot product search
  double dX12, dY12, dX13, dY13, DotProduct, t, lastt, DistanceToLine, XLine, YLine;

  //Reset beach width  
  fill(BeachWidth.begin(),BeachWidth.end(),9999);
  
  //loop through cliffline and compare to coastline to get beachwidth
  for (int i=0; i<NoNodes-1; ++i)
  {
		if (i == 0) NearestBeachIndex[i] = i, StartSearch = i, EndSearch = SearchDistance;
		else if (NearestBeachIndex[i-1] > Coastline.NoNodes-SearchDistance) StartSearch = NearestBeachIndex[i-1], EndSearch = Coastline.NoNodes-1;
		else StartSearch = NearestBeachIndex[i-1], EndSearch = NearestBeachIndex[i-1]+SearchDistance;
	
	  //reset t
	  t = 0;
	  
	  for (int j=StartSearch; j<EndSearch; ++j)
	  {
		  //Get vector x and y
	    dX12 = Coastline.X[j+1]-Coastline.X[j];
  		dY12 = Coastline.Y[j+1]-Coastline.Y[j];
	
	    //Get 2nd Vector properties
		  dX13 = X[i]-Coastline.X[j];
		  dY13 = Y[i]-Coastline.Y[j];
		
		  //Find Dot Product
		  DotProduct = dX12*dX13 + dY12*dY13;
		
		  //calculate fraction of distance along line
		  lastt = t;
		  t = DotProduct/(dX12*dX12 + dY12*dY12);
		  if ((lastt < 0. && t > 1.) || (lastt > 1. && t < 0))
		  {
			  XLine = Coastline.X[j];
  			YLine = Coastline.Y[j];
			}
			else if (t<0. || t>1.) continue;
			else
		  {
		    //Find point along line
  		  XLine = Coastline.X[j] + t*dX12;
	  	  YLine = Coastline.Y[j] + t*dY12;
	    }
	    		
		  //find distance to point
		  DistanceToLine = sqrt((XLine-X[i])*(XLine-X[i]) + (YLine-Y[i])*(YLine-Y[i]));
		  if (DistanceToLine < BeachWidth[i]) 
		  {
		    BeachWidth[i] = DistanceToLine;
		    NearestBeachIndex[i] = j;
		  }
	  }
	  if (BeachWidth[i] == 9999) NearestBeachIndex[i] = 0;
  }
}

void Cliffline::ErodeCliff(double &TimeDelta, Coastline Coastline, int ErosionType)
{
  //Calculate Cliffline Morphology
  CalculateMorphology();
  
  //Calculate Beach Width
  DetermineBeachWidth(Coastline);

  //Loop along coast
  for (int i=0;i<NoNodes-1;++i)
  {
    //if nearest beach node is in shadow, do nothing
    if (Coastline.Shadows[NearestBeachIndex[i]] != 0) continue;
    
    //get position change
    if (ErosionType == 1) PositionChange[i] = -MaxRetreatRate*exp(-BeachWidth[i]/CriticalWidth)*TimeDelta/365.; 
    else if (ErosionType == 2)
    {
      if (BeachWidth[i] < CriticalWidth) PositionChange[i] = -(MaxRetreatRate/2.)*(1+BeachWidth[i]/CriticalWidth)*TimeDelta/365.; 
      else PositionChange[i] = -MaxRetreatRate*exp(-(BeachWidth[i]-CriticalWidth)/(WidthScale))*TimeDelta/365.;
    }
    else 
    {
    	cout << "Erosion Type not set!" << endl;
    	exit(EXIT_SUCCESS);
    }
    
    //Determine volume change in cliff
    VolumeChange[i] = PositionChange[i]*CellWidth[i]*CliffHeight;
    
    //Update cliff position
    X[i] -= PositionChange[i]*cos((M_PI/180.)*Orientation[i]);
		Y[i] += PositionChange[i]*sin((M_PI/180.)*Orientation[i]);
		
		//Add volume to the beach (this is where things could get complicated!
	  Coastline.AddVolume(VolumeChange[i], NearestBeachIndex[i]);	
  }
    
  //Check for intersections and check node spacing
	IntersectionAnalysis();
	CheckNodeSpacing();
}

#endif

