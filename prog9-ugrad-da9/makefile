NVCC = /usr/bin/nvcc
CC = g++

#No optmization flags
#--compiler-options sends option to host compiler; -Wall is all warnings
#NVCCFLAGS = -c --compiler-options -Wall

#Optimization flags: -O2 gets sent to host compiler; -Xptxas -O2 is for
#optimizing PTX 
#NVCCFLAGS = -c -O2 -Xptxas -O2 --compiler-options -Wall 

#Flags for debugging
NVCCFLAGS = -c -G --compiler-options -Wall --compiler-options -g

OBJS = wrappers.o knapsack.o h_knapsack.o d_knapsack.o
.SUFFIXES: .cu .o .h 
.cu.o:
	$(NVCC) $(NVCCFLAGS) $(GENCODE_FLAGS) $< -o $@

knapsack: $(OBJS)
	$(CC) $(OBJS) -L/usr/local/cuda/lib64 -lcuda -lcudart -o knapsack

knapsack.o: knapsack.cu h_knapsack.h d_knapsack.h helpers.h wrappers.h

h_knapsack.o: h_knapsack.cu h_knapsack.h helpers.h wrappers.h 

d_knapsack.o: d_knapsack.cu d_knapsack.h helpers.h

wrappers.o: wrappers.cu wrappers.h

clean:
	rm knapsack *.o
