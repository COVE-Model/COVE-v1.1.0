#/*==============================================================

#spiral_bay_slurm_script

#Example script for launching a job on the cluster using a slurm script

#Developed by:
#Martin D. Hurst
#Andrew Barkwith
#Michael A. Ellis
#Christopher W. Thomas
#A. Brad Murray

#Copyright (C) 2015, Martin Hurst

#Developer can be contacted:
#mhurst@bgs.ac.uk

#Martin D. Hurst
#British Geological Survey,
#Environmental Science Centre,
#Nicker Hill,
#Keyworth,
#Nottingham,
#UK,
#NG12 5GG

#This program is free software: you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.

#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.

#You should have received a copy of the GNU General Public License
#along with this program.  If not, see <http://www.gnu.org/licenses/>.

#==============================================================*/
#!/bin/bash
#
# Script to submit a g++ job

# ----------------------------
# Replace these with the name of the executable 
# and the parameters it needs
							##$ -S /bin/bash


export MYAPP=spiral_bay_multiple.out 	# spiral_bay_multiple.out compiled with g++ compiler
export MYAPP_FLAGS='' 

# ---------------------------
# set the name of the job
#SBATCH -J spiral_bay_multiple	##$ -N spiral_bay_multiple

# set the output and error files - default in slurm is to put both 
# output and errors into slurm-<Job-ID>
#SBATCH -o	./OE_files/spiral_bay_multiple.out				# done by default in SGE
#SBATCH -e	./OE_files/spiral_bay_multiple.err

# set the partition (a queue in SGE)
#SBATCH -p defq

# request the use of 1 cores
#SBATCH -n 1						#$ -pe mvapich 1

# Set to email job status:
#SBATCH --mail-user mhurst@bgs.ac.uk			#$ -M mhurst@bgs.ac.uk
#SBATCH --mail-type=ALL					#$ -m beas

# set the run time hrs:mins:secs
#SBATCH -t 24:0:0


#################################################################
#################################################################
# there shouldn't be a need to change anything below this line

export MPICH_PROCESS_GROUP=no


# ---------------------------
# set up the mpich version to use
# ---------------------------
# load the module
. /etc/profile.d/modules.sh
module purge

# use the Intel compilers and Intel MPI
module load shared slurm


# ---------------------------
# run the job
# ---------------------------
echo "Will run command: srun $MYAPP $MYAPP_FLAGS" 

srun $MYAPP $MYAPP_FLAGS | sort
