MPICXX= mpic++
MPICXXFLAGS= -c -g -O0 -std=c++11
.C.o:
	$(MPICXX) $(MPICXXFLAGS) $< -o $@

all: mover

mover: mover.o distribute.o collect.o check.o
	$(MPICXX) mover.o distribute.o collect.o check.o -o mover

mover.o: mover.C distribute.h collect.h 

distribute.o: distribute.C distribute.h check.h

collect.o: collect.C collect.h check.h

check.o: check.C check.h

clean:
	rm -rf mover *.o
