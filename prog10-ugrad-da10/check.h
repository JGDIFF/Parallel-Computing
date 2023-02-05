void checkDistributeRows(int * dest, int rows, int cols, int myId, int numP);
void checkDistributeCols(int * dest, int rows, int cols, int myId, int numP);
void checkDistributeBlocks(int * dest, int rows, int cols, int myId, int numP);
void checkDistributeCyclic(int * dest, int rows, int cols, int myId, int numP);
void checkGatherRows(int * dest, int rows, int cols, int myId);
void checkAllGatherRows(int * dest, int rows, int cols, int myId);
void checkGatherStructs(int * dest, int myId, int numP);
