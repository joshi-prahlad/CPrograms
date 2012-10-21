make; make clean all
./generate 4194304 
mpirun -n 1 -machinefile mfile run
