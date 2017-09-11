#include <xinu.h>

/*------------------------------------------------------------------------
 *  signal  -  Signal a semaphore, releasing a process if one is waiting
 *------------------------------------------------------------------------
 */
syscall	registerrecv( umsg32 *abuf, int (* func) (void) )
{
	intmask mask;			/* Saved interrupt mask		*/
	struct procent *prptr;

	mask = disable();
	if (abuf == NULL && func != NULL) {
		restore(mask);
		return SYSERR;
	}
	if (abuf != NULL && func == NULL) {
			restore(mask);
			return SYSERR;
	}
	prptr = &proctab[currpid];

	prptr->abuf=abuf;
	prptr->func=func;

	restore(mask);
	return OK;
}
