CC = g++
DEBUGFLAGS = -g -c -std=c++11 -Wall -Werror
NODEBUGFLAGS = -c -std=c++11 -O2 -Wall -Werror
CFLAGS = $(NODEBUGFLAGS)
OBJS = scan.o SequentialScan.o ThreadedScan.o

scan: $(OBJS)
	$(CC) $(OBJS) -o scan -pthread

scan.o: scan.C SequentialScan.h ThreadedScan.h
	$(CC) $(CFLAGS) scan.C -o scan.o

SequentialScan.o: SequentialScan.C SequentialScan.h helpers.h
	$(CC) $(CFLAGS) SequentialScan.C -o SequentialScan.o

ThreadedScan.o: ThreadedScan.C ThreadedScan.h helpers.h
	$(CC) $(CFLAGS) ThreadedScan.C -o ThreadedScan.o

clean:
	rm scan *.o

