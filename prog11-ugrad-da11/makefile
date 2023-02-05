MPICXX = mpic++
MPICXXFLAGS = -O2 -g -c -Wall -Wno-unused-variable -Wno-unused
.C.o:
	$(MPICXX) $(MPICXXFLAGS) $< -o $@

all: seqMB staticParaMB dynamicParaMB

seqMB: seqMB.o
	$(MPICXX) seqMB.o -o seqMB -ljpeg

staticParaMB: staticParaMB.o
	$(MPICXX) staticParaMB.o -o staticParaMB -ljpeg

dynamicParaMB: dynamicParaMB.o
	$(MPICXX) dynamicParaMB.o -o dynamicParaMB -ljpeg

seqMB.o: seqMB.C

staticParaMB.o: staticParaMB.C

dynamicParaMB.o: dynamicParaMB.C

clean:
	rm -rf dynamicParaMB staticParaMB seqMB *.o

