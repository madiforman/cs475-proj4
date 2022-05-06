typedef struct node
{
    int vertex;
    struct node *next;
    char mode;
} node;

typedef struct RAG
{
    int size;
    struct node **list;
} RAG;
