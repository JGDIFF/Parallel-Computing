CC = g++
DEBUGFLAGS = -g -c -std=c++11 -Wall -Werror
NODEBUGFLAGS = -c -std=c++11 -O2 -Wall -Werror
CFLAGS = $(NODEBUGFLAGS)
OBJS = Dict.o boggle.o SequentialBoggle.o BoggleBoard.o ThreadedBoggle.o \
 ThreadPool.o Boggle.o

boggle: $(OBJS)
	$(CC) $(OBJS) -o boggle -lpthread

boggle.o: boggle.C Boggle.h Dict.h SequentialBoggle.h ThreadedBoggle.h\
    BoggleBoard.h ThreadPool.h
	$(CC) $(CFLAGS) boggle.C -o boggle.o

Dict.o: Dict.C Dict.h
	$(CC) $(CFLAGS) Dict.C -o Dict.o

SequentialBoggle.o: SequentialBoggle.C SequentialBoggle.h helpers.h \
    Boggle.h BoggleBoard.h Dict.h
	$(CC) $(CFLAGS) SequentialBoggle.C -o SequentialBoggle.o

ThreadedBoggle.o: ThreadedBoggle.C ThreadedBoggle.h helpers.h \
    Boggle.h BoggleBoard.h Dict.h ThreadPool.h 
	$(CC) $(CFLAGS) ThreadedBoggle.C -o ThreadedBoggle.o

Boggle.o: Boggle.C Boggle.h Dict.h helpers.h
	$(CC) $(CFLAGS) Boggle.C -o Boggle.o

BoggleBoard.o: BoggleBoard.C BoggleBoard.h
	$(CC) $(CFLAGS) BoggleBoard.C -o BoggleBoard.o

ThreadPool.o: ThreadPool.C ThreadPool.h
	$(CC) $(CFLAGS) ThreadPool.C -o ThreadPool.o

clean:
	rm *.o boggle

