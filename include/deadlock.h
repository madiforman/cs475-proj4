// TODO - add your deadlock function prototypes
void init_mat();
void rag_request(int pid, int lockid);
void rag_alloc(int pid, int lockid);
void rag_dealloc(int pid, int lockid);
void rag_print();
int deadlock_detect();
// TODO - add an "extern" declaration for the RAG
extern int RAG[NLOCK + NPROC][NLOCK + NPROC];
