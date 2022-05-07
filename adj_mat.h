int **matrix;
int size = 0;
void init_mat();
void rag_request(int pid, int lockid);
void rag_alloc(int pid, int lockid);
void rag_dealloc(int pid, int lockid);
void rag_print();
int deadlock_detect();
