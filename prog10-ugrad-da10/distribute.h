void distributeRowsSendRecv(int * data, int rows, int cols, int myId, int numP);
void distributeRowsIsendIrecv(int * data, int rows, int cols, int myId, int numP);
void distributeRowsScatter(int * data, int rows, int cols, int myId, int numP);
void distributeColsSendRecv(int * data, int rows, int cols, int myId, int numP);
void distributeColsScatter(int * data, int rows, int cols, int myId, int numP);
void distributeBlocksSendRecv(int * data, int rows, int cols, int myId, int numP);
void distributeRowsCyclicSendRecv(int * data, int rows, int cols, int myId, int numP);
