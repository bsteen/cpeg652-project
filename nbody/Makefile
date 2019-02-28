# typical linux/SLURM setup:                                                                    
RUN = srun
FLAGS =
LIBS = -lgd -lm
MPICC = mpicc

# on a Mac:
#RUN = mpiexec
#FLAGS = -I/opt/local/include -L/opt/local/lib
#LIBS = -lgd
#MPICC = mpicc

all: nbody_seq nbody_par nbody_bad

seq: nbodyr.gif nbody1.gif nbody2.gif nbody3.gif

test: ptest1 ptest2 ptest3

clean:
	rm -f *~ nbody_seq nbody_par nbody_bad *.gif make_test bigtest1.txt

############################# SEQUENTIAL VERSION ############################

nbodyr.gif: nbody_seq
	$(RUN) -n 1 ./nbody_seq nbodyr.gif

nbody1.gif: nbody_seq config1.txt
	$(RUN) -n 1 ./nbody_seq config1.txt nbody1.gif

nbody2.gif: nbody_seq config2.txt
	$(RUN) -n 1 ./nbody_seq config2.txt nbody2.gif

nbody3.gif: nbody_seq config3.txt
	$(RUN) -n 1 ./nbody_seq config3.txt nbody3.gif

big.gif: nbody_seq bigtest1.txt
	$(RUN) -n 1 ./nbody_seq bigtest1.txt big.gif

bigtest1.txt: make_test
	./make_test

nbody_seq: nbody_seq.c Makefile
	$(MPICC) -o nbody_seq $(FLAGS) nbody_seq.c $(LIBS)

make_test: make_test.c
	$(MPICC) -o make_test $(FLAGS) make_test.c $(LIBS)

############################### PARALLEL VERSION ##############################

pnbody1_1.gif: nbody_par config1.txt
	$(RUN) -n 1 ./nbody_par config1.txt pnbody1_1.gif

pnbody1_2.gif: nbody_par
	$(RUN) -n 2 ./nbody_par config1.txt pnbody1_2.gif

pnbody1_3.gif: nbody_par
	$(RUN) -n 3 ./nbody_par config1.txt pnbody1_3.gif

pnbody2_1.gif: nbody_par config2.txt
	$(RUN) -n 1 ./nbody_par config2.txt pnbody2_1.gif

pnbody2_2.gif: nbody_par config2.txt
	$(RUN) -n 2 ./nbody_par config2.txt pnbody2_2.gif

pnbody2_3.gif: nbody_par config2.txt
	$(RUN) -n 3 ./nbody_par config2.txt pnbody2_3.gif

pnbody3_1.gif: nbody_par config3.txt
	$(RUN) -n 1 ./nbody_par config3.txt pnbody3_1.gif

pnbody3_2.gif: nbody_par config3.txt
	$(RUN) -n 2 ./nbody_par config3.txt pnbody3_2.gif

pnbody3_3.gif: nbody_par config3.txt
	$(RUN) -n 3 ./nbody_par config3.txt pnbody3_3.gif

pbig.gif: nbody_par bigtest1.txt
	$(RUN) -n 36 ./nbody_par bigtest1.txt pbig.gif

experiment: nbody_par bigtest1.txt
	$(RUN) -n 1 ./nbody_par bigtest1.txt bbig.gif
	$(RUN) -n 2 ./nbody_par bigtest1.txt bbig.gif
	$(RUN) -n 4 ./nbody_par bigtest1.txt bbig.gif
	$(RUN) -n 8 ./nbody_par bigtest1.txt bbig.gif
	$(RUN) -n 16 ./nbody_par bigtest1.txt bbig.gif
	$(RUN) -n 32 ./nbody_par bigtest1.txt bbig.gif
	$(RUN) -n 64 ./nbody_par bigtest1.txt bbig.gif
	$(RUN) -n 72 ./nbody_par bigtest1.txt bbig.gif


nbody_par: nbody_par.c Makefile
	$(MPICC) -o nbody_par $(FLAGS) nbody_par.c $(LIBS)

################################## BAD VERSION ###############################

bnbody1_1.gif: nbody_bad config1.txt
	$(RUN) -n 1 ./nbody_bad config1.txt bnbody1_1.gif

bnbody1_2.gif: nbody_bad
	$(RUN) -n 2 ./nbody_bad config1.txt bnbody1_2.gif

bnbody1_3.gif: nbody_bad
	$(RUN) -n 3 ./nbody_bad config1.txt bnbody1_3.gif

bnbody2_1.gif: nbody_bad config2.txt
	$(RUN) -n 1 ./nbody_bad config2.txt bnbody2_1.gif

bnbody2_2.gif: nbody_bad config2.txt
	$(RUN) -n 2 ./nbody_bad config2.txt bnbody2_2.gif

bnbody2_3.gif: nbody_bad config2.txt
	$(RUN) -n 3 ./nbody_bad config2.txt bnbody2_3.gif

bnbody3_1.gif: nbody_bad config3.txt
	$(RUN) -n 1 ./nbody_bad config3.txt bnbody3_1.gif

bnbody3_2.gif: nbody_bad config3.txt
	$(RUN) -n 2 ./nbody_bad config3.txt bnbody3_2.gif

bnbody3_3.gif: nbody_bad config3.txt
	$(RUN) -n 3 ./nbody_bad config3.txt bnbody3_3.gif

bbig.gif: nbody_bad bigtest1.txt
	$(RUN) -n 36 ./nbody_bad bigtest1.txt bbig.gif

nbody_bad: nbody_bad.c Makefile
	$(MPICC) -o nbody_bad $(FLAGS) nbody_bad.c $(LIBS)

#################################### TESTS ################################

ptest1: ptest1_1 ptest1_2 ptest1_3

ptest2: ptest2_1 ptest2_2 ptest2_3

ptest3: ptest3_1 ptest3_2 ptest3_3


ptest1_1: nbody1.gif pnbody1_1.gif
	diff nbody1.gif pnbody1_1.gif

ptest1_2: nbody1.gif pnbody1_2.gif
	diff nbody1.gif pnbody1_2.gif

ptest1_3: nbody1.gif pnbody1_3.gif
	diff nbody1.gif pnbody1_3.gif


ptest2_1: nbody2.gif pnbody2_1.gif
	diff nbody2.gif pnbody2_1.gif

ptest2_2: nbody2.gif pnbody2_2.gif
	diff nbody2.gif pnbody2_2.gif

ptest2_3: nbody2.gif pnbody2_3.gif
	diff nbody2.gif pnbody2_3.gif


ptest3_1: nbody3.gif pnbody3_1.gif
	diff nbody3.gif pnbody3_1.gif

ptest3_2: nbody3.gif pnbody3_2.gif
	diff nbody3.gif pnbody3_2.gif

ptest3_3: nbody3.gif pnbody3_3.gif
	diff nbody3.gif pnbody3_3.gif

