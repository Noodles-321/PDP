############################
# Makefile for Assignment 2
############################

CC = mpicc
CCFLAGS = -O3
LIBS = -lm

BINS = quicksort

all: $(BINS)

%: %.c
	$(CC) $(CCFLAGS) -o $@ $< $(LIBS)

clean:
	$(RM) $(BINS)

