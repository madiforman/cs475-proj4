#include <stdio.h>
#include <stdlib.h>
#include "adj_mat.h"

#define NLOCK 3
#define NPROC 3

int SIZE = NLOCK + NPROC;

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

    matrix[(SIZE / 2) + pid][lockid] = 1;
}

void rag_alloc(int pid, int lockid)
{
    matrix[lockid][(SIZE / 2) + pid] = 1;
    matrix[(SIZE / 2) + pid][lockid] = 0;
}

void rag_dealloc(int pid, int lockid)
{
    if (matrix[lockid][(SIZE / 2) + pid])
    {
        matrix[lockid][(SIZE / 2) + pid] = 0;
    }
    if (matrix[(SIZE / 2) + pid][lockid])
    {
        matrix[(SIZE / 2) + pid][lockid] = 0;
    }
}
void rag_print()
{
    int i, j;
    printf("\t");
    for (int i = 0; i < NLOCK; i++)
    {
        printf("L%d\t", i);
    }
    for (int i = 0; i < NPROC; i++)
    {
        printf("p%d\t", i);
    }
    printf("\n");
    for (i = 0; i < SIZE; i++)
    {
        if (i < NLOCK)
        {
            printf("L%d\t", i);
        }
        else
        {
            printf("p%d\t", i - NLOCK);
        }

        for (j = 0; j < SIZE; j++)
        {

            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

int dfs(int cur, int white[], int gray[], int black[])
{
    white[cur] = 0;
    gray[cur] = 1;

    for (int i = 0; i < SIZE; i++)
    {
        if (matrix[cur][i])
        {

            if (black[i] == 1)
            {

                continue;
            }
            if (gray[i] == 1)
            {
                // if (cur < NLOCK)
                // {
                //     printf("P%d ", cur);
                // }
                // else
                // {
                //     printf("L%d ", cur - NLOCK);
                // }
                return 1;
            }
            if (dfs(i, white, gray, black))
            {

                return 1;
            }
        }
    }
    gray[cur] = 0;
    black[cur] = 1;
    // printf("%d\n", black[cur]);
    return 0;
}
int deadlock_detect()
{
    int flag = 0;
    int white[NLOCK * NPROC], black[NLOCK * NPROC], gray[NLOCK * NPROC], visited[NLOCK * NLOCK];
    for (int i = 0; i < SIZE; i++)
    {
        white[i] = 0;
        black[i] = 0;
        gray[i] = 0;
    }
    for (int i = 0; i < SIZE; i++)
    {
        if (!white[i])
        {
            if (dfs(i, white, gray, black))
            {

                if (i < NLOCK)
                {
                    printf("L%d\t", i);
                }
                else
                {
                    printf("P%d\t", i - NLOCK);
                }
                flag = 1;
            }
        }
    }

    return flag;
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
                printf("DEADLOCK\n");
            }
            rag_print();
        }
        else
        {
            printf("Error: File cannot be found\n");
        }
    }
    return 0;
}