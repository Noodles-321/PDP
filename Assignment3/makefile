############################
# Makefile for Assignment 3
############################

CC = mpicc
CCFLAGS = -O3
LIBS = -lm

BINS = matmul

all: $(BINS)

%: %.c
	$(CC) $(CCFLAGS) -o $@ $< $(LIBS)

clean:
	$(RM) $(BINS)

