CC = g++
DEBUGFLAGS = -g -std=c++11 -Wall -Werror
NODEBUGFLAGS = -std=c++11 -O2 -Wall -Werror
CFLAGS = $(NODEBUGFLAGS)

matrixMult: matrixMult.C hpc_helpers.h
	$(CC) $(CFLAGS) matrixMult.C -o matrixMult

clean:
	rm matrixMult
	
