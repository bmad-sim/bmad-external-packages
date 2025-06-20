#!/bin/tcsh
### LSF syntax
#BSUB -n 1                       #number of nodes
#BSUB -W 29                      #walltime in minutes
#BSUB -G guests                  #account
#BSUB -e ctestSerrors.txt        #stderr
#BSUB -o ctestSoutput.txt        #stdout
#BSUB -J hdf5_ctestS             #job
##BSUB -q pbatch                 #queue to use
#BSUB -q pdebug

cd @HDF5_BINARY_DIR@
echo "Run command. Test output will be in build/ctestS.out"
ctest -S ctest_serial.cmake >& ctestS.out

echo "Done running command."
touch ctestS.done
