/* kill.c - kill */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  kill  -  Kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
syscall	kill(
	  pid32		pid		/* ID of process to kill	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	int32	i;			/* Index into descriptors	*/
	pid32 pidnew;

	mask = disable();
	if (isbadpid(pid) || (pid == NULLPROC)
	    || ((prptr = &proctab[pid])->prstate) == PR_FREE) {
		restore(mask);
		return SYSERR;
	}

	if (--prcount <= 1) {		/* Last user process completes	*/
		xdone();
	}

	send(prptr->prparent, pid);
	for (i=0; i<3; i++) {
		close(prptr->prdesc[i]);
	}
	freestk(prptr->prstkbase, prptr->prstklen);

	//free the memory if the process has not done already
	struct prcblk *prev, *curr, *curr_next;
	prev = &(prptr->prclist);
	curr = prev->pnext;
	while(curr != NULL){
		curr_next = curr->pnext;
		freememgbpid(pid,(char *)curr->add,curr->plength);
		curr=curr_next;
	}

	switch (prptr->prstate) {
	case PR_CURR:
		prptr->prstate = PR_FREE;	/* Suicide */
		//check if process has any blocking processes in 'sending' state
		resched_cntl(DEFER_START);
		while((pidnew=dequeue(prptr->prqueue)) != EMPTY){
			//kprintf("Suicide:unblocking PR_SND process with pid: %d\n",pidnew);
			ready(pidnew);
		}
		resched_cntl(DEFER_STOP);
		resched();

	case PR_SLEEP:
	case PR_RECTIM:
		unsleep(pid);
		prptr->prstate = PR_FREE;
		break;

	case PR_WAIT:
		semtab[prptr->prsem].scount++;
		/* Fall through */

	case PR_READY:
		getitem(pid);		/* Remove from queue */
		/* Fall through */

	case PR_SND:
		getitem(pid);		/* Remove from queue */
		/* Fall through */
	default:
		prptr->prstate = PR_FREE;
	}

	//check if process has any blocking processes in 'sending' state
	resched_cntl(DEFER_START);
	while((pidnew=getfirst(prptr->prqueue)) != EMPTY){
		//kprintf("unblocking PR_SND process with pid: %d\n",pidnew);
		ready(pidnew);
	}
	resched_cntl(DEFER_STOP);

	restore(mask);
	return OK;
}
