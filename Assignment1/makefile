############################
# Makefile for Assignment 1
############################

CC = mpicc
CCFLAGS = -O3
LIBS = -lm

BINS = 2dintegral

all: $(BINS)

%: %.c
	$(CC) $(CCFLAGS) -o $@ $< $(LIBS)

clean:
	$(RM) $(BINS)

