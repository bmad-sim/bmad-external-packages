#!/bin/tcsh
### LSF syntax
#BSUB -n 6                       #number of nodes
#BSUB -R "span[ptile=6]"
#BSUB -W 30                      #walltime in minutes
#BSUB -G guests                  #account
#BSUB -e ctestPerrors.txt        #stderr
#BSUB -o ctestPoutput.txt        #stdout
#BSUB -J hdf5_ctestP             #job
##BSUB -q pbatch                 #queue to use
#BSUB -q pdebug

##date; hostname
##echo -n 'JobID is '; echo $LSB_JOBID

cd @HDF5_BINARY_DIR@
echo "Run parallel test command. Test output will be in build/ctestP.out"
ctest -S ctest_parallel.cmake >& ctestP.out

echo "Done running ctest parallel command."
touch ctestP.done
~                
