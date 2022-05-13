#include <xinu.h>
#include "deadlock.h"

#define NLOCK 10
#define NPROC 20

int SIZE = NLOCK + NPROC;
int pids[NLOCK + NPROC];
int RAG[NLOCK + NPROC][NLOCK + NPROC];
int cycles = 0;
void init_mat()
{
    int i, j;
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
    for (i = 0; i < SIZE; i++)
    {
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
    for (int i = 0; i < SIZE; i++)
    {
        if (dfs(i, visited, recur))
        {
            kprintf("DEADLOCK DETECTED\t");
            int pid = pids[0] - NLOCK, lockid = pids[1];
            for (int i = 0; i < cycles; i++)
            {
                if (i == 0 || i % 2 == 0)
                {
                    kprintf("pid=%d ", pids[i] - NLOCK);
                }
                else
                {
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
    struct lockentry *lptr = &locktab[lockid];
    while (nonempty(lptr->wait_queue))
    {
        pid32 temp = dequeue(lptr->wait_queue);
        enqueue(temp, readyqueue, proctab[temp].prprio);
    }

    kill(pid);

    for (int i = 0; i < NLOCK; i++)
    {
        struct lockentry *temp = &locktab[i];
        if (getbypid(pid, temp->wait_queue) != NULL)
        {
            remove(pid, temp->wait_queue);
        }
    }
    mutex_unlock(&lptr->lock);
    rag_dealloc(pid, lockid);
    kprintf("DEADLOCK RECOVER\tkilling pid=%d to release lockid=%d\n", pid, lockid);
}
