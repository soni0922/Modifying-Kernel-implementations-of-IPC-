/* receive.c - receive */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  receive  -  Wait for a message and return the message to the caller
 *------------------------------------------------------------------------
 */
umsg32	receiveb(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	umsg32	msg;			/* Message to return		*/
	pid32 pid;

	mask = disable();
	prptr = &proctab[currpid];
	if (prptr->prsize==0) {	//buf is empty
		//kprintf("\nR: buf is empty\n");
		prptr->prstate = PR_RECV;
		//kprintf("\nR: call resched\n");
		resched();		/* Block until message arrives	*/
	}
			//buf is not empty
	//kprintf("\nR: buf is not empty\n");
	if(prptr->prhasmsgbuf[prptr->prfront] == FALSE){	//read pos no msg
		//kprintf("\nR: read pos has no msg\n");
		prptr->prstate = PR_RECV;
		resched();
	}
	if(prptr->prhasmsgbuf[prptr->prfront] == TRUE){
		
		msg = prptr->prmsgbuf[prptr->prfront];
		//kprintf("\nR: read pos has msg: %d\n",msg);
		prptr->prhasmsgbuf[prptr->prfront] = FALSE;
		////kprintf("\nR: msg received: %d\n",msg);
		prptr->prmsgbuf[prptr->prfront]=NULL;
		prptr->prfront = (prptr->prfront + 1) %3;
		prptr->prsize--;
		//kprintf("\nR: now size of buf: %d\n",prptr->prsize);

		//unblock if any sender is blocked as we have retrived one msg
		//resched_cntl(DEFER_START);
		if((pid=dequeue(prptr->prqueue)) != EMPTY){
			//kprintf("\nR: unblock one sender id: %d\n",pid);
			ready(pid);
		}
		//resched_cntl(DEFER_STOP);
	}
	restore(mask);
	return msg;
}
