/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

#define N 5 // number of philosophers and forks

// TODO - locks must be declared and initialized here
//  mutex_t	lock = FALSE;
//  mutex_t	fork[N] = {FALSE, FALSE, FALSE, FALSE, FALSE};
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
	uint32 right = phil_id;					   // right fork
	uint32 left = N - ((N - phil_id) % N) - 1; // left fork

	while (TRUE)
	{
		// think 70% of the time
		if ((rand() * 1.0) / RAND_MAX < 0.7)
		{
			acquire(printer_lock);
			kprintf("Philosopher %d thinking: zzzzzZZZz\n", phil_id);
			release(printer_lock);

			think();
		}
		else // eat 30% of the time
		{
			// mutex_lock(&fork[right]);	//grab the right fork (or wait)
			acquire(forks[right]);
			
			if (!locktab[forks[left]].lock)
			{
				// mutex_lock(&fork[left]);	//grab the left fork
				acquire(forks[left]);

				// mutex_lock(&lock);
				acquire(printer_lock);
				kprintf("Philosopher %d eating: nom nom nom\n", phil_id);
				// mutex_unlock(&lock);
				release(printer_lock);

				eat();

				// mutex_unlock(&fork[left]);
				// mutex_unlock(&fork[right]);
				release(forks[left]);
				release(forks[right]);
			}
			else
			{
				// mutex_unlock(&fork[right]);	//relinquish right fork
				release(forks[right]);
			}
		}
	}
}

int main(uint32 argc, uint32 *argv)
{
	printer_lock = lock_create();
	for (int i = 0; i < N; i++)
	{
		forks[i] = lock_create();
	}
	ready(create((void *)philosopher, INITSTK, 15, "Ph1", 1, 0), FALSE);
	ready(create((void *)philosopher, INITSTK, 15, "Ph2", 1, 1), FALSE);
	ready(create((void *)philosopher, INITSTK, 15, "Ph3", 1, 2), FALSE);
	ready(create((void *)philosopher, INITSTK, 15, "Ph4", 1, 3), FALSE);
	ready(create((void *)philosopher, INITSTK, 15, "Ph5", 1, 4), FALSE);

	return 0;
}