CC = g++
DEBUGFLAGS = -g -c -std=c++11 -Wall -Werror
NODEBUGFLAGS = -c -std=c++11 -O2 -Wall -Werror
CFLAGS = $(NODEBUGFLAGS)
OBJS = scan.o SequentialScan.o ThreadedScan.o ThreadPool.o

all: scan runScan

runScan: runScan.C
	$(CC) -std=c++11 -O2 runScan.C -o runScan

scan: $(OBJS)
	$(CC) $(OBJS) -o scan -pthread

scan.o: scan.C SequentialScan.h ThreadedScan.h
	$(CC) $(CFLAGS) scan.C -o scan.o

SequentialScan.o: SequentialScan.C SequentialScan.h helpers.h
	$(CC) $(CFLAGS) SequentialScan.C -o SequentialScan.o

ThreadedScan.o: ThreadedScan.C ThreadedScan.h helpers.h ThreadPool.h
	$(CC) $(CFLAGS) ThreadedScan.C -o ThreadedScan.o

ThreadPool.o: ThreadPool.C ThreadPool.h
	$(CC) $(CFLAGS) ThreadPool.C -o ThreadPool.o

clean:
	rm scan *.o

