/* sendb.c - sendb */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  sendb  -  Pass a message to a process and start recipient if waiting
 *------------------------------------------------------------------------
 */
syscall	sendb(
	  pid32		pid,		/* ID of recipient process	*/
	  umsg32	msg		/* Contents of message		*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	procent *prptr;		/* Ptr to process's table entry	*/
	struct  procent *currptr;

	mask = disable();
	if (isbadpid(pid)) {
		restore(mask);
		return SYSERR;
	}

	prptr = &proctab[pid];
	
	if(prptr->prstate== PR_FREE){		//receiver is terminated
		//kprintf("\nS: recv is killed\n");
		//kprintf("S: receiver died: q has msgs: %d %d %d with status: %d %d %d\n",prptr->prmsgbuf[0],prptr->prmsgbuf[1],prptr->prmsgbuf[2],prptr->prhasmsgbuf[0],prptr->prhasmsgbuf[1],prptr->prhasmsgbuf[2]);
		restore(mask);
		return SYSERR;
	}
	if (prptr->prsize==3) {		//buf is full
		//kprintf("\nS: buf is full\n");
		currptr = &proctab[currpid];
		currptr->prstate=PR_SND;
		enqueue(currpid,prptr->prqueue);
		//kprintf("\nS: enqueue to sending queue\n");
		resched();
        }
        	//kprintf("\nS: buf not full\n");
		if(prptr->prstate== PR_FREE){
			//kprintf("\nS: recv is killed\n");
			//kprintf("S: receiver died: q has msgs: %d %d %d with status: %d %d %d\n",prptr->prmsgbuf[0],prptr->prmsgbuf[1],prptr->prmsgbuf[2],prptr->prhasmsgbuf[0],prptr->prhasmsgbuf[1],prptr->prhasmsgbuf[2]);
			restore(mask);
			return SYSERR;
		}
		//kprintf("\nS: msg to send: %d\n",msg);
		if(prptr->prhasmsgbuf[prptr->prrear] == TRUE){	//write pos already has msg
			//kprintf("\nS: write pos already has msg: %d\n",msg);
			currptr = &proctab[currpid];
			currptr->prstate=PR_SND;
			enqueue(currpid,prptr->prqueue);
			resched();
		}
		if(prptr->prstate== PR_FREE){
                        //kprintf("\nS: recv is killed\n");
                        //kprintf("S: receiver died: q has msgs: %d %d %d with status: %d %d %d\n",prptr->prmsgbuf[0],prptr->prmsgbuf[1],prptr->prmsgbuf[2],prptr->prhasmsgbuf[0],prptr->prhasmsgbuf[1],prptr->prhasmsgbuf[2]);
                        restore(mask);
                        return SYSERR;
                }

		if(prptr->prhasmsgbuf[prptr->prrear] == FALSE){
			//kprintf("\nS: write pos has no msg\n");
			prptr->prmsgbuf[prptr->prrear]= msg;
			prptr->prhasmsgbuf[prptr->prrear] = TRUE;
			prptr->prrear = (prptr->prrear + 1)%3;
			prptr->prsize++;
			//kprintf("\nS: now size of buf is: %d\n",prptr->prsize);
		
			/* If recipient waiting or in timed-wait make it ready */

        		if (prptr->prstate == PR_RECV) {
        			//kprintf("\nS: ready recvr\n");
        	       		ready(pid);
        		} else if (prptr->prstate == PR_RECTIM) {
               			unsleep(pid);
               			//kprintf("\nS: ready recvr\n");
               			ready(pid);
        		}	
		}	
	
	restore(mask);          /* Restore interrupts */
        return OK;

}
