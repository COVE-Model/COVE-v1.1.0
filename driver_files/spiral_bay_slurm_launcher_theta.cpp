/*==============================================================

spiral_bay_slurm_launcher_theta.hpp

A launcher function for running the COVE model on a cluster using
slurm. Varies the 

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

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

using namespace std;

int main()
{
	//input params
	int PeriodMean = 6;
	int PeriodStd = 1;
	float HeightMean = 1.;
	float HeightStd = 0.1;
	
	//range of theta values
	int Start_Theta = -10;
	int End_Theta = 50;
	int Theta_Step = 5;
	
	//range of std values
	int Start_Std = 5;
	int End_Std = 45;
	int Std_Step = 5;
		
	for (int Theta=Start_Theta; Theta <= End_Theta; Theta+=Theta_Step)
	{
		for (int ThetaStd=Start_Std; ThetaStd <= End_Std; Theta+=Std_Step)
	  {
	    //****************Create slurm scripts******************
		  ofstream write_sh;
		  char sh_name[128];
		  sprintf(sh_name, "spiral_bay_%i.sh", Theta);
		  write_sh.open(sh_name);
		  write_sh << "#!/bin/bash" << endl;
		  write_sh << "export MYAPP=spiral_bay.out" << endl;
		  write_sh << "export MYAPP_FLAGS='" << PeriodMean << " " << PeriodStd << " " << HeightMean << " " << HeightStd << " " << Theta << " " << ThetaStd << "'" << endl;
		  write_sh << "#SBATCH -J spiral_bay_" << Theta << "_" << ThetaStd << endl;
		  write_sh << "#SBATCH -o	./OE_files/spiral_bay_" << Theta << "_" << ".out" << endl;
		  write_sh << "#SBATCH -e	./OE_files/spiral_bay_" << Theta << "_" << ".err" << endl;
		  write_sh << "#SBATCH -p defq" << endl;
		  write_sh << "#SBATCH -n 1" << endl;											//#$ -pe mvapich 1
		  write_sh << "#SBATCH --mail-user mhurst@bgs.ac.uk" << endl;			//#$ -M mhurst@bgs.ac.uk
		  write_sh << "#SBATCH --mail-type=ALL" << endl;							//#$ -m beas
		  write_sh << "#SBATCH -t 12:0:0" << endl;
		  write_sh << "export MPICH_PROCESS_GROUP=no" << endl;
		  write_sh << ". /etc/profile.d/modules.sh" << endl;
		  write_sh << "module purge" << endl;
		  write_sh << "module load shared slurm" << endl;
		  write_sh << "echo \"Will run command: srun $MYAPP $MYAPP_FLAGS\"" << endl; 
		  write_sh << "srun $MYAPP $MYAPP_FLAGS | sort" << endl;
		  write_sh.close();

		  //*******************Submission of scripts**************************
		  //launch job!
		  char launch[128];
		  sprintf(launch,"sbatch spiral_bay_%i.sh", Theta);
		  system(launch);
    }
	}
	return 0;
}


