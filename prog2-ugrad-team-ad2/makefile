CC = g++
DEBUGCFLAGS = -g -std=c++11 -mavx
CFLAGS = -std=c++11 -O1 -mavx

matrixMult: matrixMult.C hpc_helpers.h
	$(CC) $(DEBUGCFLAGS) matrixMult.C -o matrixMult

clean:
	rm matrixMult
	
