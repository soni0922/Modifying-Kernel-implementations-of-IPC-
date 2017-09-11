/* freemem.c - freemem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  freemem  -  Free a memory block, returning the block to the free list
 *------------------------------------------------------------------------
 */
syscall	freememgb(
	  char		*blkaddr,	/* Pointer to memory block	*/
	  uint32	nbytes		/* Size of block in bytes	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	prcblk	*next, *prev, *del_add;
	struct procent *prptr;

	mask = disable();
	if ((nbytes == 0) || ((uint32) blkaddr < (uint32) minheap)
			  || ((uint32) blkaddr > (uint32) maxheap)) {
		restore(mask);
		return SYSERR;
	}
	//kprintf("free gb start\n");
	prptr=&proctab[currpid];	

	/*prev = &(prptr->prclist);			 Walk along free list	
	prev = prev->pnext;
	//print the list
	//kprintf("print the allocated mem list\n");
	while(prev != NULL){
		//kprintf("allocated add is: %d\n",prev->add);
		//kprintf("add of this node is: %d\n",prev);
		prev = prev->pnext;
	}*/

	prev = &(prptr->prclist);
	next = (prptr->prclist).pnext;
	while ((next != NULL) && ((next->add) != (blkaddr))) {
		prev = next;
		next = next->pnext;
	}

	if (next == NULL) {		/* could not find address*/
		//kprintf("user trying to free wrong add\n");
		restore(mask);
		return SYSERR;
	}
	if(next->plength != nbytes){	//free nbytes is not equal to the one that was requested
		//kprintf("user trying to free wrong no of bytes\n");
		restore(mask);
		return SYSERR;
	}
	del_add = next->pnext;
	//prev->pnext = next->pnext;

	if(freemem(blkaddr,nbytes) == SYSERR){
		//kprintf("error while trying to free allocated add\n");
		restore(mask);
		return SYSERR;
	}
	if(freemem((char *)next,(sizeof(char *) + sizeof(struct prcblk *) + sizeof(uint32))) == SYSERR){
		//kprintf("error while trying to free bk add\n");
		restore(mask);
		return SYSERR;
	}
	prev->pnext = del_add;


	//print the list
	/*prev = &(prptr->prclist);                        Walk along free list 
	prev = prev->pnext;
	//kprintf("after free; print the allocated mem list\n");
	while(prev != NULL){
			//kprintf("allocated add is: %d\n",prev->add);
			//kprintf("add of this node is: %d\n",prev);
			prev = prev->pnext;
	}

	//kprintf("free gb done\n");*/
	restore(mask);
	return OK;
}
