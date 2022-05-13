/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

#define N 3 // number of philosophers and forks

/*	locks are declared and initialized	*/
lid32 printer_lock;
lid32 forks[N];

/**
 * Delay for a random amount of time
 * @param alpha delay factor
 */
void holdup(int32 alpha)
{
    long delay = rand() * alpha;
    while (delay-- > 0)
        ; // no op
}

/**
 * Eat for a random amount of time
 */
void eat()
{
    holdup(10000);
}

/**
 * Think for a random amount of time
 */
void think()
{
    holdup(1000);
}

/**
 * Philosopher's code
 * @param phil_id philosopher's id
 */
void philosopher(uint32 phil_id)
{
    uint32 right = phil_id;                    // right fork
    uint32 left = N - ((N - phil_id) % N) - 1; // left fork

    while (TRUE)
    {
        // think 70% of the time
        if (rand() % 10 < 7)
        {
            acquire(printer_lock);
            kprintf("Philosopher %d (pid=%d) thinking: zzzzzZZZz\n", phil_id, currpid);
            release(printer_lock);

            think();
        }
        else // eat 30% of the time
        {
            acquire(forks[right]);	// grab the right fork
            acquire(forks[left]);	// grab the left fork 

            acquire(printer_lock);
            kprintf("Philosopher %d (pid=%d) eating: nom nom nom\n", phil_id, currpid);
            release(printer_lock);

            eat();

            release(forks[left]);
            holdup(10000);
            release(forks[right]);
        }
    }
}

int main(uint32 argc, uint32 *argv)
{
    int i;
    printer_lock = lock_create();
    for (int i = 0; i < N; i++)
    {
        forks[i] = lock_create();
    }

    ready(create((void *)philosopher, INITSTK, 15, "Ph1", 1, 0), FALSE);
    ready(create((void *)philosopher, INITSTK, 15, "Ph2", 1, 1), FALSE);
    ready(create((void *)philosopher, INITSTK, 15, "Ph3", 1, 2), FALSE);

    return 0;
}