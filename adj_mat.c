#include <stdio.h>
#include <stdlib.h>
#include "adj_mat.h"

#define NLOCK 10
#define NPROC 20

int SIZE = NLOCK + NPROC;
int deadlocked[NLOCK + NPROC];
int num_cycles = 0;
void init_mat()
{
    int i, j;
    matrix = (int **)malloc(SIZE * sizeof(int *));
    for (i = 0; i <= SIZE; ++i)
    {
        matrix[i] = (int *)malloc(SIZE * sizeof(int));
    }
    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            matrix[i][j] = 0;
        }
    }
}

void rag_request(int pid, int lockid)
{

    matrix[NLOCK + pid][lockid] = 1;
}

void rag_alloc(int pid, int lockid)
{
    matrix[lockid][NLOCK + pid] = 1;
    matrix[NLOCK + pid][lockid] = 0;
}

void rag_dealloc(int pid, int lockid)
{
    matrix[NLOCK + pid][lockid] = 0;
    matrix[lockid][NLOCK + pid] = 0;
}
void rag_print()
{
    int i, j;
    // printf("\t");
    // for (int i = 0; i < NLOCK; i++)
    // {
    //     printf("L%d\t", i);
    // }
    // for (int i = 0; i < NPROC; i++)
    // {
    //     printf("p%d\t", i);
    // }
    // printf("\n");
    for (i = 0; i < SIZE; i++)
    {
        // if (i < NLOCK)
        // {
        //     printf("L%d\t", i);
        // }
        // else
        // {
        //     printf("p%d\t", i - NLOCK);
        // }

        for (j = 0; j < SIZE; j++)
        {

            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}

int dfs(int v, int visited[], int recur[])
{
    if (!visited[v])
    {
        visited[v] = 1;
        recur[v] = 1;
        for (int i = 0; i < SIZE; i++)
        {
            if (matrix[v][i])
            {
                if (!visited[i] && dfs(i, visited, recur))
                {
                    deadlocked[num_cycles++] = v;
                    return 1;
                }
                if (recur[i])
                {
                    deadlocked[num_cycles++] = v;
                    return 1;
                }
            }
        }
    }
    recur[v] = 0;
    return 0;
}
int deadlock_detect()
{
    int visited[SIZE], recur[SIZE];
    for (int i = 0; i < SIZE; i++)
    {
        visited[i] = 0;
        recur[i] = 0;
    }
    // int flag = 0;
    for (int i = 0; i < SIZE; i++)
    {
        if (dfs(i, visited, recur))
        {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[])
{
    FILE *file;
    if (argc != 2) // if there are not enough command arguments
    {
        printf("Usage: ./tests <file>\n");
    }
    else
    {
        init_mat();
        char *arg = argv[1];
        file = fopen(arg, "r");
        if (file != NULL)
        {
            char event;
            int pid, lockid;
            while (fscanf(file, "%d %c %d\n", &pid, &event, &lockid) != EOF)
            {
                if (event == 'R')
                {
                    rag_request(pid, lockid);
                }
                else if (event == 'A')
                {
                    rag_alloc(pid, lockid);
                }
                else if (event == 'D')
                {
                    rag_dealloc(pid, lockid);
                }
            }
            if (deadlock_detect())
            {
                printf("DEADLOCK ");
                // printf("num num_cycles: %d\n", num_cycles);
                for (int i = 0; i < num_cycles; i++)
                {

                    if (i == 0 || i % 2 == 0)
                    {
                        printf("pid=%d ", deadlocked[i] - NLOCK);
                    }
                    else
                    {
                        printf("lockid=%d ", deadlocked[i]);
                    }
                }
                printf("\n");
            }
            // ssss rag_print();
        }
        else
        {
            printf("Error: File cannot be found\n");
        }
    }
    return 0;
}