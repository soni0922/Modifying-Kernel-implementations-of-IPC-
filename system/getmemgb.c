/* getmem.c - getmem */

#include <xinu.h>

/*------------------------------------------------------------------------
 *  getmem  -  Allocate heap storage, returning lowest word address
 *------------------------------------------------------------------------
 */
char  	*getmemgb(
	  uint32	nbytes		/* Size of memory requested	*/
	)
{
	intmask	mask;			/* Saved interrupt mask		*/
	struct	prcblk	*prev, *second;
	char * allocated_add, *bk_add;
	struct procent *prptr;

	mask = disable();
	if (nbytes == 0) {
		restore(mask);
		return (char *)SYSERR;
	}
	//kprintf("getmem gb start\n");
	allocated_add = (char *)getmem(nbytes);
	if((int32)allocated_add == SYSERR ){
		restore(mask);
		return (char *)SYSERR;
	}
	
	bk_add = (char *)getmem(sizeof(char *) + sizeof(struct prcblk *) + sizeof(uint32));
	if((int32)bk_add == SYSERR ){
			restore(mask);
			return (char *)SYSERR;
	}
	//kprintf("allocated add: %d\n",allocated_add);
	//kprintf("bk_add: %d\n",bk_add);
	prptr=&proctab[currpid];
	prev=&(prptr->prclist);
	
	second = prev->pnext;
	prev->pnext = (struct prcblk *)bk_add;
	(prev->pnext)->pnext = second;
	(prev->pnext)->add = allocated_add;
	(prev->pnext)->plength=nbytes;

	/*curr=(prptr->prclist).pnext;

	while(curr != (struct prcblk *)NULL){
		prev = curr;
		curr = curr->pnext;
	}

	prev->pnext = (struct prcblk *)bk_add;
	prev = (struct prcblk *)bk_add;
	prev->add = allocated_add;
	prev->plength=nbytes;
	prev->pnext=(struct prcblk *)NULL;*/
	
	
	/*prev = &(prptr->prclist);                        Walk along free list 
	prev = prev->pnext;
	print the list
	kprintf("after adding; print the allocated mem list\n");
	while(prev != NULL){
			kprintf("allocated add is: %d\n",prev->add);
			kprintf("add of this node is: %d\n",prev);
			prev = prev->pnext;
	}

	kprintf("getmem gb done\n");*/
	restore(mask);
	return allocated_add;
}
