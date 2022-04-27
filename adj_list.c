#include <stdlib.h>
#include <stdio.h>

// mtrx[x][y] = 0 -> no edge
// mtrx[x][y] = 1 -> request edge
// mtrxp[x][y] =
#define NLOCK 10
#define NPROC 20
#define SIZE = NLOCK + NPROC

int edges, vertices;
int **adj_mtrx;

int **init_rag()
{

    int **m = (int **)malloc(vertices * sizeof(int *));
    for (int i = 0; i < vertices; i++)
    {
        m[i] = (int *)malloc(vertices * sizeof(int));
    }
    for (int i = 0; i < vertices; i++)
    {
        for (int j = 0; j < vertices; j++)
        {
            m[i][j] = 0;
        }
    }
    return m;
}

void rag_request(int **mtrx, int pid, int lockid)
{
    mtrx[pid][lockid] = 1;
}
void rag_alloc(int **mtrx, int pid, int lockid)
{

    mtrx[lockid][pid] = 1;
    mtrx[pid][lockid] = 0;
}

void rag_dealloc(int **mtrx, int pid, int lockid)
{
    mtrx[lockid][pid] = 0;
    mtrx[pid][lockid] = 0;
}
void print_mtrx(int **mtrx)
{
    for (int i = 0; i < NPROC + NLOCK; i++)
    {
        for (int j = 0; j < NPROC + NLOCK; j++)
        {
            printf("%d\t", mtrx[i][j]);
        }
        printf("\n");
    }
}



int main()
{
 

}