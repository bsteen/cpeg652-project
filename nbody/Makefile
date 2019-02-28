# Benjamin Steenkamer
# CISC 372-010
# Homework 6 - Makefile
# 11/11/16
# ssh bsteen@hoek.eecis.udel.edu
# ssh bsteen@cisc372.cis.udel.edu
# svn list svn://cisc372.cis.udel.edu/bsteen -R

LIBS = -lgd -lm
P_LIBS = -pthread -lrt
CC = gcc -std=c11
NUM_THREADS = 4

default: timing

###################MAKE TEST#####################
generate-test: compile_test
	./make_test.out

compile_test: make_test.c
	$(CC) -o make_test.out make_test.c $(LIBS)
################################################

#####################COMPILE#####################
compile-nbody_seq: nbody_seq.c
	$(CC) -o nbody_seq.out nbody_seq.c $(LIBS)

compile-nbody_pthread: nbody_pthread.c
	$(CC) -o nbody_pthread.out nbody_pthread.c $(LIBS) $(P_LIBS)
################################################

####################SEQ TEST#####################
seq-nbodyr: compile-nbody_seq
	./nbody_seq.out nbodyr.gif

seq-nbody1: compile-nbody_seq config1.txt
	./nbody_seq.out config1.txt s-nbody1.gif

seq-nbody2: compile-nbody_seq config2.txt
	./nbody_seq.out config2.txt s-nbody2.gif

seq-nbody3: compile-nbody_seq config3.txt
	./nbody_seq.out config3.txt s-nbody3.gif

seq-nbodybig: compile-nbody_seq bigtest1.txt
	./nbody_seq.out bigtest1.txt s-nbodybig.gif
################################################

################PTHREAD TEST####################
p-nbodyr: compile-nbody_pthread
	./nbody_pthread.out p-nbodyr.gif $(NUM_THREADS)

p-nbody1: compile-nbody_pthread config1.txt
	./nbody_pthread.out config1.txt p-nbody1.gif $(NUM_THREADS)

p-nbody2: compile-nbody_pthread config2.txt
	./nbody_pthread.out config2.txt p-nbody2.gif $(NUM_THREADS)

p-nbody3: compile-nbody_pthread config3.txt
	./nbody_pthread.out config3.txt p-nbody3.gif $(NUM_THREADS)

p-nbodybig: compile-nbody_pthread bigtest1.txt
	./nbody_pthread.out bigtest1.txt p-nbodybig.gif $(NUM_THREADS)
################################################

####################COMPARE#####################
cmp-nbody1: seq-nbody1 p-nbody1
	diff s-nbody1.gif p-nbody1.gif

cmp-nbody2: seq-nbody2 p-nbody2
	diff s-nbody2.gif p-nbody2.gif

cmp-nbody3: seq-nbody3 p-nbody3
	diff s-nbody3.gif p-nbody3.gif
################################################

##################OPEN GIF######################
sgif1: s-nbody1.gif
	xdg-open s-nbody1.gif
pgif1: p-nbody1.gif
	xdg-open p-nbody1.gif
################################################

####################TIMING######################
timing: nbody_seq.c nbody_pthread.c
	$(CC) -o make_test.out make_test.c $(LIBS) -DTIMING
	$(CC) -o nbody_seq.out nbody_seq.c $(LIBS) -DTIMING
	$(CC) -o nbody_pthread.out  nbody_pthread.c $(LIBS) $(P_LIBS) -DTIMING
	srun -c 1 ./make_test.out

	srun -c 1 ./nbody_seq.out bigtest1.txt s-nbodybig.gif
	srun -c 1 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 1
	srun -c 2 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 2
	srun -c 4 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 4
	srun -c 8 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 8
	srun -c 12 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 12
	srun -c 14 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 14
	srun -c 16 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 16
	srun -c 20 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 20
	srun -c 24 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 24
	srun -c 28 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 28
	srun -c 32 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 32
	srun -c 40 ./nbody_pthread.out bigtest1.txt p-nbodybig.gif 40
################################################

clean:
	rm -f *.out *.o *.gif
