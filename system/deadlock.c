#include <xinu.h>
#include "deadlock.h"

// #include <stdio.h>
// #include <stdlib.h>

#define NLOCK 10
#define NPROC 20

int SIZE = NLOCK + NPROC;
int pids[NLOCK + NPROC];
int RAG[NLOCK + NPROC][NLOCK + NPROC];
int cycles = 0;
void init_mat()
{
    int i, j;
    // RAG = (int **)malloc(SIZE * sizeof(int *));
    // for (i = 0; i <= SIZE; ++i)
    // {
    //     RAG[i] = (int *)malloc(SIZE * sizeof(int));
    // }
    for (i = 0; i < SIZE; i++)
    {
        for (j = 0; j < SIZE; j++)
        {
            RAG[i][j] = 0;
        }
    }
}

void rag_request(int pid, int lockid)
{

    RAG[NLOCK + pid][lockid] = 1;
}

void rag_alloc(int pid, int lockid)
{
    RAG[lockid][NLOCK + pid] = 1;
    RAG[NLOCK + pid][lockid] = 0;
}

void rag_dealloc(int pid, int lockid)
{
    RAG[NLOCK + pid][lockid] = 0;
    RAG[lockid][NLOCK + pid] = 0;
}
void rag_print()
{
    int i, j;
    // kprintf("\t");
    // for (int i = 0; i < NLOCK; i++)
    // {
    //     kprintf("L%d\t", i);
    // }
    // for (int i = 0; i < NPROC; i++)
    // {
    //     kprintf("p%d\t", i);
    // }
    // kprintf("\n");
    for (i = 0; i < SIZE; i++)
    {
        // if (i < NLOCK)
        // {
        //     kprintf("L%d\t", i);
        // }
        // else
        // {
        //     kprintf("p%d\t", i - NLOCK);
        // }

        for (j = 0; j < SIZE; j++)
        {

            kprintf("%d ", RAG[i][j]);
        }
        kprintf("\n");
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
            if (RAG[v][i])
            {
                // kprintf("%d\n", v);
                if (!visited[i] && dfs(i, visited, recur))
                {
                    pids[cycles++] = v;
                    return 1;
                }
                else if (recur[i])
                {
                    pids[cycles++] = v;
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
            kprintf("DEADLOCK ");
            int pid = pids[0] - NLOCK, lockid = pids[1];
            kprintf("%d\n", pids[i]);
            for (int i = 0; i < cycles; i++)
            {
                if (i == 0 || i % 2 == 0)
                {
                    // pid = pids[i] - NLOCK;
                    kprintf("pid=%d ", pids[i] - NLOCK);
                }
                else
                {
                    //  lockid = pids[i];s
                    kprintf("lockid=%d ", pids[i]);
                }
            }
            kprintf("\n");
            deadlock_recover(pid, lockid);
            cycles = 0;
            return 1;
        }
    }

    return 0;
}
void deadlock_recover(int pid, int lockid)
{
    // rag_print();
    struct lockentry *lptr = &locktab[lockid];
    while (nonempty(lptr->wait_queue))
    {
        pid32 temp = dequeue(lptr->wait_queue);
        enqueue(temp, readyqueue, proctab[temp].prprio);
    }
    kill(pid);
    kprintf("pid= %d was killed\n", pid);

    for (int i = 0; i < NLOCK; i++)
    {
        struct lockentry *temp = &locktab[i];
        if (getbypid(pid, temp->wait_queue) != NULL)
        {
            remove(pid, temp->wait_queue);
            kprintf("pid= %d removed\n", pid);
        }
    }
    mutex_unlock(&lptr->lock);
    rag_dealloc(pid, lockid);
    // rag_print();
    kprintf("DEADLOCK RECOVER killing pid=%d to release lock=%d\n", pid, lockid);
}

// int main(int argc, char *argv[])
// {
//     FILE *file;
//     if (argc != 2) // if there are not enough command arguments
//     {
//         kkprintf("Usage: ./tests <file>\n");
//     }
//     else
//     {
//         init_mat();
//         char *arg = argv[1];
//         file = fopen(arg, "r");
//         if (file != NULL)
//         {
//             char event;
//             int pid, lockid;
//             while (fscanf(file, "%d %c %d\n", &pid, &event, &lockid) != EOF)
//             {
//                 if (event == 'R')
//                 {
//                     rag_request(pid, lockid);
//                 }
//                 else if (event == 'A')
//                 {
//                     rag_alloc(pid, lockid);
//                 }
//                 else if (event == 'D')
//                 {
//                     rag_dealloc(pid, lockid);
//                 }
//             }
//             if (deadlock_detect())
//             {
//                 kkprintf("DEADLOCK ");
//                 // kkprintf("num cycles: %d\n", cycles);
//                 for (int i = 0; i < cycles; i++)
//                 {

//                     if (i == 0 || i % 2 == 0)
//                     {
//                         kkprintf("pid=%d ", pids[i] - NLOCK);
//                     }
//                     else
//                     {
//                         kkprintf("lockid=%d ", pids[i]);
//                     }
//                 }
//                 kkprintf("\n");
//             }
//             // ssss rag_print();
//         }
//         else
//         {
//             kkprintf("Error: File cannot be found\n");
//         }
//     }
//     return 0;
// }
// TODO - add in your functions
