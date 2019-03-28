# Benjamin Steenkamer, 2019
# CPEG 652 Semester Project

# Requirements for project:
# sudo apt install gcc
# sudo apt install openmpi-bin libopenmpi-dev
# sudo apt install libomp-dev

CC = gcc
MPICC = mpicc
RUN = mpirun -np
FLAGS = -O4 -Wall
LIBS = -lgd -lm
P_LIBS = -pthread -lrt

clean:
	rm -f test_maker nbody_seq nbody_mpi nbody_omp *.gif

############################## RANDOM TEST GEN #################################

test_maker: test_maker.c
	$(CC) test_maker.c -o test_maker $(FLAGS) -lm

############################ SEQUENTIAL VERSION ################################

nbodyr.gif: nbody_seq random.txt
	./nbody_seq random.txt nbodyr.gif

nbody1.gif: nbody_seq config1.txt
	./nbody_seq config1.txt nbody1.gif

nbody2.gif: nbody_seq config2.txt
	./nbody_seq config2.txt nbody2.gif

nbody3.gif: nbody_seq config3.txt
	./nbody_seq config3.txt nbody3.gif

nbody_seq: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq $(FLAGS) $(LIBS)
	
########################### PTHREAD VERSION ####################################

nbody_pthread: nbody_pthread.c
	$(CC) nbody_pthread.c -o nbody_pthread $(FLAGS) $(LIBS) $(P_LIBS)
	
############################# OMP VERSION ######################################

nbody_omp: nbody_omp.c
	$(CC) nbody_omp.c -o nbody_omp $(FLAGS) $(LIBS)

############################# MPI VERSION ######################################

nbody_mpi_1_1.gif: nbody_mpi config1.txt
	$(RUN) 1 ./nbody_mpi config1.txt nbody_mpi_1_1.gif

nbody_mpi_1_2.gif: nbody_mpi
	$(RUN) 2 ./nbody_mpi config1.txt nbody_mpi_1_2.gif

nbody_mpi_1_3.gif: nbody_mpi
	$(RUN) 3 ./nbody_mpi config1.txt nbody_mpi_1_3.gif

nbody_mpi_2_1.gif: nbody_mpi config2.txt
	$(RUN) 1 ./nbody_mpi config2.txt nbody_mpi_2_1.gif

nbody_mpi_2_2.gif: nbody_mpi config2.txt
	$(RUN) 2 ./nbody_mpi config2.txt nbody_mpi_2_2.gif

nbody_mpi_2_3.gif: nbody_mpi config2.txt
	$(RUN) 3 ./nbody_mpi config2.txt nbody_mpi_2_3.gif

nbody_mpi_3_1.gif: nbody_mpi config3.txt
	$(RUN) 1 ./nbody_mpi config3.txt nbody_mpi_3_1.gif

nbody_mpi_3_2.gif: nbody_mpi config3.txt
	$(RUN) 2 ./nbody_mpi config3.txt nbody_mpi_3_2.gif

nbody_mpi_3_3.gif: nbody_mpi config3.txt
	$(RUN) 3 ./nbody_mpi config3.txt nbody_mpi_3_3.gif

nbody_mpi: nbody_mpi.c Makefile
	$(MPICC) -o nbody_mpi $(FLAGS) nbody_mpi.c $(LIBS)