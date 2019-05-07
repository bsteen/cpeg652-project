# Benjamin Steenkamer, 2019
# CPEG 652 Semester Project

# Requirements for project:
# sudo apt install gcc
# sudo apt install openmpi-bin libopenmpi-dev
# sudo apt install libomp-dev

CC = gcc
MPICC = mpicc
RUN = mpirun -np
LIBS = -lgd -lm
P_LIBS = -pthread -lrt

clean:
	rm -f test_maker nbody_seq nbody_seq_3 Output/*.gif
	rm -f nbody_mpi nbody_mpi_3 nbody_omp nbody_omp_3

############################## RANDOM TEST GEN #################################

test_maker: test_maker.c
	$(CC) test_maker.c -o test_maker -Wall -O3 -lm

############################ SEQUENTIAL VERSION ################################

nbody_seq: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq -Wall -O0 $(LIBS)

nbody_seq_no_gif: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq -DNO_GIF -Wall -O0 $(LIBS)

run_nbody_seq:
	./nbody_seq Tests/random.txt Output/nbody_seq.gif

nbody_seq_3: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq_3 -Wall -O3 $(LIBS)

nbody_seq_3_no_gif: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq_3 -DNO_GIF -Wall -O3 $(LIBS)

run_nbody_seq_3:
	./nbody_seq_3 Tests/random.txt Output/nbody_seq_3.gif

########################### PTHREAD VERSION ####################################

nbody_pthread: nbody_pthread.c
	$(CC) nbody_pthread.c -o nbody_pthread -Wall -O0 $(LIBS) $(P_LIBS)

nbody_pthread_no_gif: nbody_pthread.c
	$(CC) nbody_pthread.c -o nbody_pthread -DNO_GIF -Wall -O0 $(LIBS) $(P_LIBS)

# run_nbody_pthread:
	# ./nbody_pthread Tests/random.txt Output/nbody_pthread.gif X

nbody_pthread_3: nbody_pthread.c
	$(CC) nbody_pthread.c -o nbody_pthread_3 -Wall -O3 $(LIBS) $(P_LIBS)

nbody_pthread_3_no_gif: nbody_pthread.c
	$(CC) nbody_pthread.c -o nbody_pthread_3 -DNO_GIF -Wall -O3 $(LIBS) $(P_LIBS)
	
# run_nbody_pthread_3:
	# ./nbody_pthread_3 Tests/random.txt Output/nbody_pthread_3.gif X

############################# OMP VERSION ######################################

nbody_omp: nbody_omp.c
	$(CC) nbody_omp.c -o nbody_omp -Wall -O0 $(LIBS)
	
nbody_omp_no_gif: nbody_omp.c
	$(CC) nbody_omp.c -o nbody_omp -DNO_GIF -Wall -O0 $(LIBS)

# run_nbody_omp:
	# ./nbody_omp Tests/random.txt Output/nbody_omp.gif X

nbody_omp_3: nbody_omp.c
	$(CC) nbody_omp.c -o nbody_omp_3 -Wall -O3 $(LIBS)
	
nbody_omp_3_no_gif: nbody_omp.c
	$(CC) nbody_omp.c -o nbody_omp_3 -DNO_GIF -Wall -O3 $(LIBS)
	
# run_nbody_omp_3:
	# ./nbody_omp_3 Tests/random.txt Output/nbody_omp_3.gif X

############################# MPI VERSION ######################################

nbody_mpi: nbody_mpi.c
	$(MPICC) nbody_mpi.c -o nbody_mpi -Wall -O0 $(LIBS)
	
nbody_mpi_no_gif: nbody_mpi.c
	$(MPICC) nbody_mpi.c -o nbody_mpi -DNO_GIF -Wall -O0 $(LIBS)
	
# run_nbody_mpi:
	# mpirun -np X ./nbody_mpi Tests/random.txt Output/nbody_omp.gif
	
nbody_mpi_3: nbody_mpi.c
	$(MPICC) nbody_mpi.c -o nbody_mpi_3 -Wall -O3 $(LIBS)	

nbody_mpi_3_no_gif: nbody_mpi.c
	$(MPICC) nbody_mpi.c -o nbody_mpi_3 -DNO_GIF -Wall -O3 $(LIBS)

# run_nbody_mpi_3:
	# mpirun -np X ./nbody_mpi_3 Tests/random.txt Output/nbody_omp.gif
