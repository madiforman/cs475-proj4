#include <xinu.h>

local lid32 newlock(void);

/**
 * Create a new semaphore and return the ID to the caller
 * @return ID of the mutex to caller, or SYSERR
 */
lid32 lock_create()
{
	intmask mask; /* saved interrupt mask	*/
	lid32 lockid; /* lock ID to return	*/

	mask = disable();

	if ((lockid = newlock()) == SYSERR)
	{
		restore(mask);
		return SYSERR;
	}

	restore(mask);
	return lockid;
}

/**
 * Allocate an unused lock and return its index
 * @return	ID of free lock
 */
local lid32 newlock(void)
{
	// static lid32 nextlock = 0; /* next lockid to try	*/
	lid32 lockid;			/* ID to return	*/
	int32 i;				/* iterate through # entries	*/
	struct lockentry *lptr; /*	ptr to lock table	*/

	/*	loop through each element in the lock table and find a lock 
		that is free to use*/
	for (i = 0; i < NLOCK; i++)
	{
		lptr = &locktab[i];
		if (lptr->state == LOCK_FREE)
		{
			lockid = (lid32)i;
			lptr->state = LOCK_USED;
			lptr->lock = FALSE;

			return lockid;
		}
	}
	return SYSERR;
}

/**
 * Delete a lock by releasing its table entry
 * @param ID of lock to release
 */
syscall lock_delete(lid32 lockid)
{
	intmask mask;			/* saved interrupt mask		*/
	struct lockentry *lptr; /* ptr to lock table entry	*/

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}

	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}
	lptr->state = LOCK_FREE;
	lptr->lock = FALSE;

	struct queue *lock_q = locktab[lockid].wait_queue;
	pid32 temp = dequeue(lock_q);

	/*	remove all processes waiting on its queue, send them to 
	the ready queue, and remove all RAG edges to and from this lock	*/
	while (nonempty(lock_q))
	{
		rag_dealloc(temp, lockid);
		temp = dequeue(lock_q);
		enqueue(temp, readyqueue, proctab[temp].prprio);
	}

	resched();
	restore(mask);
	return OK;
}

/**
 * Acquires the given lock, or cause current process to wait until acquired
 * @param lockid	lock on which to wait
 * @return status
 */
syscall acquire(lid32 lockid)
{
	intmask mask;			// saved interrupt mask
	struct lockentry *lptr; // ptr to sempahore table entry

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}

	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// enqueue the current process ID on the lock's wait queue
	enqueue(currpid, lptr->wait_queue, proctab[currpid].prprio);

	// add a request edge in the RAG
	rag_request(currpid, lockid);

	restore(mask); // reenable interrupts

	// lock the mutex!
	mutex_lock(&lptr->lock);

	mask = disable(); // disable interrupts

	// transform request edge to allocation edge
	rag_alloc(currpid, lockid);

	restore(mask); // reenable interrupts
	return OK;
}

/**
 * Unlock, releasing a process if one is waiting
 * @param lockid ID of lock to unlock
 * @return status
 */
syscall release(lid32 lockid)
{
	intmask mask;			/* saved interrupt mask		*/
	struct lockentry *lptr; /* ptr to lock table entry	*/

	mask = disable();
	if (isbadlock(lockid))
	{
		restore(mask);
		return SYSERR;
	}
	lptr = &locktab[lockid];
	if (lptr->state == LOCK_FREE)
	{
		restore(mask);
		return SYSERR;
	}

	// remove current process' ID from the lock's queue
	remove(currpid, lptr->wait_queue);

	// unlock the mutex
	mutex_unlock(&lptr->lock);

	// remove allocation edge from RAG
	rag_dealloc(currpid, lockid);

	restore(mask);
	return OK;
}
