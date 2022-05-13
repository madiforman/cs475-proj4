#include <xinu.h>
#include "deadlock.h"

#define NLOCK 10
#define NPROC 20

int SIZE = NLOCK + NPROC;
int deadlocks[NLOCK + NPROC];          /*holds pids/lockids of rag if cycle was found*/
int RAG[NLOCK + NPROC][NLOCK + NPROC]; /*the RAG*/
int n_cycles = 0;                      /*the number of pid/lockids in the cycle*/

/**
 * This function initializes the rag
 */
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

/**
 * This function adds a request edge to the RAG from pid to lockid
 * @param pid the process id
 * @param lockid the lock id
 */
void rag_request(int pid, int lockid)
{

    RAG[NLOCK + pid][lockid] = 1;
}

/**
 * This function adds an allocation edge to the RAG from lockid to pid
 * @param pid the process id
 * @param lockid the lock id
 */
void rag_alloc(int pid, int lockid)
{
    RAG[lockid][NLOCK + pid] = 1;
    RAG[NLOCK + pid][lockid] = 0;
}

/**
 * This function removes the request and allocation edge from lockid to pid
 * @param pid the process id
 * @param lockid the lock id
 */
void rag_dealloc(int pid, int lockid)
{
    RAG[NLOCK + pid][lockid] = 0;
    RAG[lockid][NLOCK + pid] = 0;
}

/**
 * This function prints out the RAG
 */
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

/**
 * This function does a Depth First Search to find cycles in the RAG
 * @param v starting vertex of search (will be 0 when called)
 * @param visited Array for visited pids/lockids
 * @param recur array to store cycles found in recursive calls
 * @return 1 if cycle is found, 0 if not found
 */
int dfs(int v, int visited[], int recur[])
{
    if (!visited[v])
    {
        visited[v] = 1;
        recur[v] = 1;
        for (int i = 0; i < SIZE; i++)
        {
            if (RAG[v][i]) /*for all adjacent nodes to v*/
            {
                if (!visited[i] && dfs(i, visited, recur)) /*if adjacent node hasnt been visited before and recursive call to dfs returns a 1, cycle detected*/
                {
                    deadlocks[n_cycles++] = v; /*increment num of cycles and add vertex to deadlocked list*/
                    return 1;
                }
                else if (recur[i]) /*else if recur[i] never got set back to 0, a cycle was found*/
                {
                    deadlocks[n_cycles++] = v;
                    return 1;
                }
            }
        }
    }
    recur[v] = 0; /*else reset recur[v]*/
    return 0;
}

/**
 * This function prints out the cycle that caused the deadlock
 */
void print_deadlocks()
{
    for (int i = 0; i < n_cycles; i++)
    {
        if (i == 0 || i % 2 == 0)
        {
            kprintf("pid=%d ", deadlocks[i] - NLOCK);
        }
        else
        {
            kprintf("lockid=%d ", deadlocks[i]);
        }
    }
    kprintf("\n");
}

/**
 * This function detects whether there was a deadlock or not
 */
void deadlock_detect()
{
    int visited[SIZE], recur[SIZE];

    /*  initializes the visited and recursive arrays with zeros */
    for (int i = 0; i < SIZE; i++)
    {
        visited[i] = 0;
        recur[i] = 0;
    }
    /*  searching for cycles in RAG */
    for (int i = 0; i < SIZE; i++)
    {
        if (dfs(i, visited, recur)) /*if a cycle was found*/
        {
            kprintf("DEADLOCK DETECTED\t");
            int pid = deadlocks[0] - NLOCK, lockid = deadlocks[1];
            print_deadlocks();
            deadlock_recover(pid, lockid);
        }
    }
}

/**
 * This function recovers the program from a deadlock
 * @param pid the process id
 * @param lockid the lock id
 */
void deadlock_recover(int pid, int lockid)
{
    struct lockentry *lptr = &locktab[lockid];

    /*  deqeue all processes in the lock's wait queue and
        add them to ready queue*/
    while (nonempty(lptr->wait_queue))
    {
        pid32 temp = dequeue(lptr->wait_queue);
        enqueue(temp, readyqueue, proctab[temp].prprio);
    }

    kill(pid);

    /*  remove the process from all other locks' wait queues    */
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
