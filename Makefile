# Benjamin Steenkamer, 2019
# CPEG 652 Semester Project

# Requirements for project:
# sudo apt install gcc
# sudo apt install libomp-dev libomp5
# The LibGD library for gif creation: https://libgd.github.io/
#	sudo apt install libgd-dev

CC = gcc
LIBS = -lgd -lm
P_LIBS = -pthread
O_LIBS = -fopenmp

clean:
	rm -f nbody_seq nbody_seq_O3 
	rm -f nbody_pthread_v1 nbody_pthread_v1_O3 nbody_pthread_v2 nbody_pthread_v2_O3
	rm -f nbody_omp_v1 nbody_omp_v1_O3 nbody_omp_v2 nbody_omp_v2_O3
	rm -f test_maker timer_overhead Output/*.gif

############################## RANDOM TEST GEN #################################

test_maker: test_maker.c
	$(CC) test_maker.c -o test_maker -Wall -O3 -lm
	
timer_overhead: timer_overhead.c
	$(CC) timer_overhead.c -o timer_overhead -Wall -O0 $(O_LIBS)

############################ SEQUENTIAL VERSION ################################

nbody_seq: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq -Wall -O0 $(LIBS)

nbody_seq_no_out: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq -DNO_OUT -Wall -O0 $(LIBS)

run_nbody_seq:
	./nbody_seq Tests/random.txt Output/nbody_seq.gif

nbody_seq_O3: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq_O3 -Wall -O3 $(LIBS)

nbody_seq_O3_no_out: nbody_seq.c
	$(CC) nbody_seq.c -o nbody_seq_O3 -DNO_OUT -Wall -O3 $(LIBS)

run_nbody_seq_O3:
	./nbody_seq_O3 Tests/random.txt Output/nbody_seq_O3.gif

########################### PTHREAD VERSIONS ###################################
# Pthread implementation V1
nbody_pthread_v1: nbody_pthread_v1.c
	$(CC) nbody_pthread_v1.c -o nbody_pthread_v1 -Wall -O0 $(LIBS) $(P_LIBS)

nbody_pthread_v1_no_out: nbody_pthread_v1.c
	$(CC) nbody_pthread_v1.c -o nbody_pthread_v1 -DNO_OUT -Wall -O0 $(LIBS) $(P_LIBS)

# ./nbody_pthread_v1 Tests/random.txt Output/nbody_pthread_v1.gif

nbody_pthread_v1_O3: nbody_pthread_v1.c
	$(CC) nbody_pthread_v1.c -o nbody_pthread_v1_O3 -Wall -O3 $(LIBS) $(P_LIBS)

nbody_pthread_v1_O3_no_out: nbody_pthread_v1.c
	$(CC) nbody_pthread_v1.c -o nbody_pthread_v1_O3 -DNO_OUT -Wall -O3 $(LIBS) $(P_LIBS)

# ./nbody_pthread_v1_O3 Tests/random.txt Output/nbody_pthread_v1_O3.gif

# Pthread implementation V2
nbody_pthread_v2: nbody_pthread_v2.c
	$(CC) nbody_pthread_v2.c -o nbody_pthread_v2 -Wall -O0 $(LIBS) $(P_LIBS)

nbody_pthread_v2_no_out: nbody_pthread_v2.c
	$(CC) nbody_pthread_v2.c -o nbody_pthread_v2 -DNO_OUT -Wall -O0 $(LIBS) $(P_LIBS)

# ./nbody_pthread_v2 Tests/random.txt Output/nbody_pthread_v2.gif

nbody_pthread_v2_O3: nbody_pthread_v2.c
	$(CC) nbody_pthread_v2.c -o nbody_pthread_v2_O3 -Wall -O3 $(LIBS) $(P_LIBS)

nbody_pthread_v2_O3_no_out: nbody_pthread_v2.c
	$(CC) nbody_pthread_v2.c -o nbody_pthread_v2_O3 -DNO_OUT -Wall -O3 $(LIBS) $(P_LIBS)

# ./nbody_pthread_v2_O3 Tests/random.txt Output/nbody_pthread_v2_O3.gif

############################# OMP VERSION ######################################

# OMP implementation V1
nbody_omp_v1: nbody_omp_v1.c
	$(CC) nbody_omp_v1.c -o nbody_omp_v1 -Wall -O0 $(LIBS) $(O_LIBS)

nbody_omp_v1_no_out: nbody_omp_v1.c
	$(CC) nbody_omp_v1.c -o nbody_omp_v1 -DNO_OUT -Wall -O0 $(LIBS) $(O_LIBS)

# ./nbody_omp_v1 Tests/random.txt Output/nbody_omp_v1.gif

nbody_omp_v1_O3: nbody_omp_v1.c
	$(CC) nbody_omp.c -o nbody_omp_O3 -Wall -O3 $(LIBS) $(O_LIBS)

nbody_omp_v1_O3_no_out: nbody_omp.c
	$(CC) nbody_omp_v1.c -o nbody_omp_v1_O3 -DNO_OUT -Wall -O3 $(LIBS) $(O_LIBS)

# ./nbody_omp_v1_O3 Tests/random.txt Output/nbody_omp_v1_O3.gif

# OMP implementation V2
nbody_omp_v2: nbody_omp_v2.c
	$(CC) nbody_omp_v2.c -o nbody_omp_v2 -Wall -O0 $(LIBS) $(O_LIBS)

nbody_omp_v2_no_out: nbody_omp_v2.c
	$(CC) nbody_omp_v2.c -o nbody_omp_v2 -DNO_OUT -Wall -O0 $(LIBS) $(O_LIBS)
	
# ./nbody_omp_v2 Tests/random.txt Output/nbody_omp_v2.gif

nbody_omp_v2_O3: nbody_omp_v2.c
	$(CC) nbody_omp_v2.c -o nbody_omp_v2_O3 -Wall -O3 $(LIBS) $(O_LIBS)

nbody_omp_v2_O3_no_out: nbody_omp_v2.c
	$(CC) nbody_omp_v2.c -o nbody_omp_v2_O3 -DNO_OUT -Wall -O3 $(LIBS) $(O_LIBS)
	
# ./nbody_omp_v2_O3 Tests/random.txt Output/nbody_omp_v2.gif