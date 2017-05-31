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
	double UStart = 0.;
	double UEnd = 1.;
	double UStep = 0.05;
	
	//range of std values
	double AStart = 0.5;
	double AEnd = 1.;
	double AStep = 0.05;
		
	for (double U=UStart; U <= UEnd; U+=UStep)
	{
	  for (double A=AStart; A <= AEnd; A+=AStep)
	  {
		  //****************Create slurm scripts******************
		  ofstream write_sh;
		  char sh_name[128];
		  sprintf(sh_name, "spiral_bay_%.2f.sh", U);
		  write_sh.open(sh_name);
		  write_sh << "#!/bin/bash" << endl;
		  write_sh << "export MYAPP=spiral_bay_UA.out" << endl;
		  write_sh << "export MYAPP_FLAGS='" << PeriodMean << " " << PeriodStd << " " << HeightMean << " " << HeightStd << " " << U << " " << A << "'" << endl;
		  write_sh << "#SBATCH -J spiral_bay_" << U << "_" << A << endl;
		  write_sh << "#SBATCH -o	./OE_files/spiral_bay_" << U << "_" << ".out" << endl;
		  write_sh << "#SBATCH -e	./OE_files/spiral_bay_" << U << "_" << ".err" << endl;
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
		  sprintf(launch,"sbatch spiral_bay_%.2f.sh", U);
		  system(launch);
    }
	}
	return 0;
}


