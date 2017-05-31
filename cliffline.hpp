/*==============================================================

cliffline.hpp

The cliffline object
cliffline is a vector based object defining the location
and paramaters of a cliff line at the coast.

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

/** @file cliffline.hpp
@author Martin D. Hurst, British Geological Survey
@brief cliffline object for a rocky coast
@details This file contains the cliffline object for the COVE model,
evolving a vector representing a a cliffed, rocky coast. Cliff recession
is driven by beach width through interaction with the coastline object.
Many functions have been retained from the coastline object, which is a 
friend class.
@date 4/1/2016
*/
#ifndef cliffline_HPP
#define cliffline_HPP

#include <cmath>
#include <cstdlib>
#include <vector>
#include <cstring>
#include <queue>
#include "waveclimate.hpp"
#include "coastline.hpp"

using namespace std;

//Dummy class for Coastline
class Coastline;

/// @brief cliffline class object for evolving a vector cliffline.
/// @details The cliffline class object is the main object used in 
/// evolving a rocky coast. It consists of paired X, Y vectors 
/// which describe the location of the cliff, a series of vectors
/// describing the cliffline and coastal cliff geometry, and a series 
/// of functions with which to calculate beach width and calculate the 
/// amount of cliff retreat and move the cliff position during a model 
/// timestep. The sediment generated is then made available for the 
/// coastline object for redistribution alongshore
/// @author Martin D. Hurst
/// @date 4/1/2016
class Cliffline
{
	friend class Coastline;
	
	private:
	//data members
	int NoNodes;										    //Number of nodes along coastline
	vector<double> X;									  //position in x (m) (private)
	vector<double> Y;									  //position in y (m) (private)
	vector<double> Distance;						//Distance along the vector
	vector<double> FluxOrientation;			//orientation/azimuth of shoreline across i, i+1 (private)
	vector<double> Orientation;					//orientation/azimuth of shoreline across i-1, i+1 (private)
	vector<double> CellWidth;						//Width of individual cells parallel to orientation
	vector<double> VolumeChange;				//VolumeChange from eroded cliff
	vector<double> PositionChange;			//Magnitude of change in clifftop position
	vector<double> NearestBeachIndex;   //Index in Coastline object of nearest beach node
	vector<double> e1;                  //Angle to cliff normal at left cell boundary
	vector<double> e2;                  //Angle to cliff normal at right cell boundary
  vector<double> BeachWidth;          //Beach width (m) in shore normal direction
	vector<int> Fixed;  					      //Is coastline fixed?
		
	int StartBoundary;						//Boundary type periodic=1 /no flux=2 /continous flux=3 /sinks=4 (private)
	int EndBoundary;						  //Boundary type periodic=1 /no flux=2 /continous flux=3 /sinks=4 (private)
	int MeanNodeSpacing;					//average spacing between nodes (m) (private)
	int DesiredNodeSpacing;				//for adding and removing nodes?
	
	double Trend;							  //redundant?
	
	double ShelfSlope;					//Slope of bedrock surface below sea level 

  double MaxRetreatRate;          //Maximum Rate of Cliff Retreat (m/yr)
  double CliffHeight;             //Constant cliff height (m), could later be replaced with vector
	double CriticalWidth;           //Beach width at which cliff retreat is maximised (m)
  double WidthScale;              //Sets the Beach width at which cliff retreat is minimised
	double LostFraction;            //Fraction of material supplied from cliff immediately lost to the sea
	
	/** @brief Calculates the morphological properties of the coastline
	@details Resets the coastline morphological vectors and recalculates a variety
	of geometric properties and metrics. Calculates Distance along the coast, Orientation 
	(3-cell) of the coastline, the orientation of cell boundaries 
	(e1 and e2), and the CellWidth.
	@author Martin D. Hurst
  @date 4/1/2016 */ 
	void CalculateMorphology();
	
	/* @brief Checks nodes haven't got too close together or too far apart.
	@details Checks to see whether there are nodes too close together 
	(<0.66*DesiredNodeSpacing) or too far apart (>1.5*DesiredNodeSpacing). 
	Nodes are added by interpolation or deleted as appropriate
	@author Martin D. Hurst
  @date 4/1/2015 */ 
	void CheckNodeSpacing();

  /* @brief Calculates the mean spacing between adjacent nodes
	@details Calculates the mean spacing between adjacent nodes
	@author Martin D. Hurst
  @date 4/1/2015 */ 
	void CalculateMeanNodeSpacing();
	
	/* @brief Checks the coastline doesn't intersect itself anywhere.
	@details This would indicate formation of a stack!?
	@author Martin D. Hurst
  @date 4/1/2016 */ 
	void IntersectionAnalysis();
	
	//Initialisation functions	
	void Initialise();
	void Initialise(string xyfilename);
	void Initialise(string xyfilename, float StartTime);
	void Initialise(int MeanNodeSpacing, double CoastLength, double Trend, int StartBoundary, int EndBoundary);
	
	//Writing functions for debugging
  void WriteXYFile();
  
  public:
	
	/* @brief Initialise function. This is an empty intialisation and thus throws an error	
	@author Martin D. Hurst
	@date 4/1/2016 */
	Cliffline()	{ Initialise(); }
	
	/* @brief Initialise function. Creates a Cliffline object from a file containing x and y data
	@return Cliffline
	@param xyfilename String, the file containing the coastline x and y coordinates
	@author Martin D. Hurst
	@date 4/1/2016 */
	Cliffline(string xyfilename) { Initialise(xyfilename); }
	
	/* @brief Initialise function. Creates a Cliffline object from a file containing x and y data at a specified time.
	@details File format is the same as that outputted by the WriteCliff method.
	@return Cliffline
	@param xyfilename String, the file containing the coastline x and y coordinates
	@param StartTime Float, the time to get at which to get x and y data from file
	@author Martin D. Hurst
	@date 4/1/2016 */
	Cliffline(string xyfilename, float StartTime) { Initialise(xyfilename, StartTime); }
	
	/* @brief Initialise function. Creates a Coastline object as a straight line following an azimuth.
	@return Coastline
	@param MeanNodeSpacing Int, the average distance between coastal nodes (metres)
	@param CoastLength Double, the length of the coastline to be created
	@param Trend Double, the azimuth direction in which to create the coast.
	@param StartBoundary, the boundary condition at the start of the line (1=fixed, 2=periodic)
	@param EndBoundary, the boundary condition at the end of the line (1=fixed, 2=periodic), must be same as StartBoundary (for the moment).
	@author Martin D. Hurst
	@date 4/1/2016 */
	Cliffline(int MeanNodeSpacing, double CoastLength, double Trend, int StartBoundary, int EndBoundary)
	{
		Initialise(MeanNodeSpacing, CoastLength, Trend, StartBoundary, EndBoundary);
	}
	
	/// @brief Reads whether cliff is fixed
	/// @details Reads a file containing a list of nodes and whether or not the are fixed cliff
	/// nodes or can be eroded. 1 = Fixed, 0 = Erodible. Assigns values to the Fixed vector
	/// @param InputFileName String, the file containing the fixed information
	/// @author Martin D. Hurst
	/// @date 18/1/2016
	void ReadCliffType(string InputFileName);
	
	/// @brief Function to read a Cliffline from a file at a given time.
	/// @details File format is the same as that outputted by the WriteCliff method.
	/// @param InputFileName String, the file containing the coastline x and y coordinates
	/// @param Time Double, the time to get at which to get x and y data from file
	/// @author Martin D. Hurst
	/// @date 4/1/2016
	void ReadCliff(string InputFileName, double Time);
	
	/// @brief Function to write a coastline to a file at a given time.
	/// @param OutputFileName String, the file to write the coastline x and y coordinates
	/// @param Time Double, the model time written to file
	/// @author Martin D. Hurst
	/// @date 4/1/2016
	void WriteCliff(string OutputFileName, double Time);
	
	/* @brief Measure the beach width by comparison to 
	@details Fucntion to determine the shortest distance between cliffline nodes and the coastline
	object as an measure of beach width. Beach width then inflences the rate of cliff reterat
	@param Coastline Coastline, the Coastline object
	@author Martin D. Hurst
	@date 2/2/2016 */
	void DetermineBeachWidth(Coastline Coastline);
	
	/* @brief Erode the Cliffline
	@details Function to calculate how much cliff to erode, and move the cliffline. Cliff erosion is a 
	function of	beach width following Valvo et al (2006) OR Limber and Murray (2011). Eroded material 
	is supplied to the nearest adjacent beach cell in the coastline object.
	@param TimeDelta double, the time step length (days)
	@param LostFraction, double, the fraction of cliff material lost instantly to the sea (in suspension).
	@param ErosionType, integer, flag for Valvo et al. (2006) style cliffs (ErosionType = 1)
	@author Martin D. Hurst
	@date 4/1/2016 */
	void ErodeCliff(double &TimeDelta, Coastline MyCoastline, int ErosionType);

	/* @brief Determine amount of cliff retreat
	@details Calculate how much cliff to erode following Limber and Murray (2011).
	@param i, integer index for cell
	@author Martin D. Hurst
	@date 28/1/2016 */
	void HumpedCliffRetreat(int i);
  

	/*****************************************\
	| Set Functions to assign private members |
	\*****************************************/
	
	/* @brief Set Maximum Retreat Rate
	@details Function to assign the maximum cliff retreat rate (m/yr)
	@param RetreatRate double, the max retreat rate (m/yr)
	@author Martin D. Hurst
	@date 13/1/2016 */
	void SetMaxRetreatRate(double RetreatRate) {MaxRetreatRate = RetreatRate;}
	
	/* @brief Set Cliff Height
	@details Function to assign a constant cliff height value. 
	This could later be replaced with a vector derived from a DEM
	@param CliffHeight double, the height of the cliff (m)
	@author Martin D. Hurst
	@date 13/1/2016 */
	void SetCliffHeight(double TheCliffHeight) {CliffHeight = TheCliffHeight;}
	
	/* @brief Set Critical Beach Width
	@details Function to assign the critical beach width that maximises cliff retreat
	@param CriticalBeachWidth double, the retreat maximising beach width (m)
	@author Martin D. Hurst
	@date 13/1/2016 */
	void SetCriticalWidth(double CriticalBeachWidth)
	{
	  CriticalWidth = CriticalBeachWidth;
	  WidthScale = CriticalWidth;
	}

  /* @brief Set Fraction of Cliff Material lost
	@details Function to assign the fraction of cliff material lost to the sea
	@param LostFraction  double, The fraction of material lost
	@author Martin D. Hurst
	@date 14/1/2016 */
  void SetLostFraction(double LostFraction) {LostFraction = LostFraction;}

	/*****************************************\
	| Get Functions to return private members |
	\*****************************************/
	
	/// @brief Return number of nodes in Cliffline.
	/// @details Function to return the private member NoNodes value, 
	/// the number of nodes in the coastline object
	/// @returns Int, Number of nodes
	/// @author Martin D. Hurst
  /// @date 4/1/2016
	int get_NoNodes() const						{return NoNodes;}			//get Number of nodes along coastline
	
	/// @brief Return Cliffline X-values.
  /// @details Function to return the private member X values
	/// @returns vector<double>, X coordinate values of the coastline 
	/// @author Martin D. Hurst
  /// @date 4/1/2016
  vector<double> get_X() const				{return X;}					//get X vector position in x (m)
	
	/// @brief Return Cliffline Y-values.
  /// @details Function to return the private member Y values
	/// @returns vector<double>, Y coordinate values of the coastline 
	/// @author Martin D. Hurst
  /// @date 4/1/2016
	vector<double> get_Y() const				{return Y;}					//get Y vectir position in y (m)
	
	/// @brief Return BeachWidth values.
  /// @details Function to return the private member BeachWidth values
	/// @returns vector<double> 
	/// @author Martin D. Hurst
  /// @date 13/1/2016
	vector<double> get_BeachWidth() const				{return BeachWidth;}			//get BeachWidth vector (m)
		
	/// @brief Return Cliffline orientation.
	/// @details Function to return the private member Orientation, 
	/// the azimuth direction of the line connecting the two adjacent 
	/// nodes to the node of interest (3-cell orientation). The evolving 
	/// cosatal node advances or retreats perpendicular to this orientation.
	/// @returns vector<double>, Orientation values of the Cliffline 
	/// @author Martin D. Hurst
  /// @date 4/1/2016
	vector<double> get_Orientation() const 	 	{return Orientation;}		//get orientation of shoreline between i-1 and i+1
	
	/// @brief Return start boundary condition.
	/// @Function to return the private member StartBoundary, the boundary 
	/// condition at the start end of the vector. Currently, StartBoundary 
	/// and EndBoundary must have the same value. 1 = Periodic boundary, 
	/// 2 = Fixed Boundary (with or without sediment flux).
	/// @returns Int, StartBoundary condition
	/// @author Martin D. Hurst
  /// @date 4/1/2016
	int get_StartBoundary() const				{return StartBoundary;}		//get Boundary type start
	
	/// @brief Return end boundary condition.
	/// @Function to return the private member EndBoundary, the boundary 
	/// condition at the end of the vector. Currently, StartBoundary 
	/// and EndBoundary must have the same value. 1 = Periodic boundary, 
	/// 2 = Fixed Boundary (with or without sediment flux).
	/// @returns Int, EndBoundary condition
	/// @author Martin D. Hurst
  /// @date 4/1/2016
	int get_EndBoundary() const					{return EndBoundary;}		//get Boundary type end

	/// @brief Return desired mean node spacing.
  /// @details Function to return the private member MeanNodeSpacing, 
  /// the desired mean node spacing for the model.
	/// @returns Int, MeanNodeSpacing
	/// @author Martin D. Hurst
  /// @date 4/1/2016
	int get_MeanNodeSpacing() const				{return MeanNodeSpacing;}	//get average spacing between nodes (m)

};

#endif
