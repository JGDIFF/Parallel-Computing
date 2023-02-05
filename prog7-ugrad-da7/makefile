NODEBUGFLAGS = -std=c++14 -fopenmp -O2 -Wall -Werror
DEBUGFLAGS = -g -std=c++14 -fopenmp -Wall -Werror
CFLAGS = $(NODEBUGFLAGS)
OBJS = sorter.o Sorts.o ParaMergeSort.o ParaQuickSort.o SeqMergeSort.o SeqQuickSort.o
CC = g++
.C.o: 
	scl enable devtoolset-7 'bash --rcfile <(echo "  \
	$(CC) -c $(CFLAGS) -o $@ $<; \
	exit")'
     

all: 
	make sorter 

sorter: $(OBJS)
	scl enable devtoolset-7 'bash --rcfile <(echo "  \
	$(CC) $(OBJS) -fopenmp -o sorter; \
	exit")'

sorter.o: Sorts.h SeqMergeSort.h SeqQuickSort.h ParaMergeSort.h ParaQuickSort.h 

Sorts.o: Sorts.h Sorts.C

ParaMergeSort.o: Sorts.h ParaMergeSort.h ParaMergeSort.C helpers.h

ParaQuickSort.o: Sorts.h ParaQuickSort.h ParaQuickSort.C helpers.h

SeqMergeSort.o: Sorts.h SeqMergeSort.h SeqMergeSort.C helpers.h

SeqQuickSort.o: Sorts.h SeqQuickSort.h SeqQuickSort.C helpers.h

clean:
	rm sorter *.o

