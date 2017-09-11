/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
#include <string.h>

//..............................................PART 1 Piazza Test Case.............................................
/*
 Functionality of test case: Here, the receiver is resumed fisrt and gets blocked since the buffer will be empty. Then sender1 sends one msg and wakes up receiver. Similarly, all other four senders tries to send msg if the buffer is not full or the writing position of the buffer does not already has some msg, and then unblocks receiver if it was blocked.When the receiver unblocks, it reads msgs in FIFO order. Receiver reads only ten msgs and then exits, so senders fail to send next msgs to killed receiver.
*/
/*sid32 mutex;

void sender(int to_whom, int start, int num) {
    int i;
    umsg32 msg;
    for (i = 0; i < num; i++) {
        msg = start + i;
        if (sendb(to_whom, msg) == SYSERR){
            wait(mutex);
            kprintf("sender %s fails to send msg %d to receiver %s\n", proctab[getpid()].prname, msg, proctab[to_whom].prname);
            signal(mutex);
            return SYSERR;
        }
    }
}

void receiver(int num) {
    int i;
    umsg32 msg;
    for (i = 0; i < num; i++) {
        msg = receiveb();
        wait(mutex);
        kprintf("receiver %s got the msg %d\n", proctab[getpid()].prname, msg);
        signal(mutex);
    }
}

int main(int argc, char **argv) {
    mutex = semcreate(1);
    pid32 r1  = create(receiver, 1024, 20, "R1", 1, 10);
    pid32 s1 = create(sender, 1024, 20, "S1", 3, r1, 100, 10);
    pid32 s2 = create(sender, 1024, 20, "S2", 3, r1, 200, 10);
    pid32 s3 = create(sender, 1024, 20, "S3", 3, r1, 300, 10);
    pid32 s4 = create(sender, 1024, 20, "S4", 3, r1, 400, 10);

    resume(r1);
    resume(s1);
    resume(s2);
    resume(s3);
    resume(s4);

    return OK;
}*/
/* OUTPUT Piazza Test Case
receiver R1 got the msg 100
receiver R1 got the msg 101
receiver R1 got the msg 102
receiver R1 got the msg 103
receiver R1 got the msg 300
receiver R1 got the msg 200
receiver R1 got the msg 301
receiver R1 got the msg 104
receiver R1 got the msg 400
receiver R1 got the msg 201
sender S2 fails to send msg 202 to receiver R1
sender S3 fails to send msg 303 to receiver R1
sender S1 fails to send msg 106 to receiver R1
sender S4 fails to send msg 402 to receiver R1 
*/

//..............................................PART 1 Test 1.............................................
/*
 Functionality of test case:
 First the receiver process resumes, but it will be blocked since the buffer of receiver is empty. Then sender sends one msg which wakes up the receiver and it receives
 the first msg. Then the receiver goes to sleep, which makes the sender to send three msgs in the buffer and then the sender gets blocked. When the receiver wakes up,
 it receives one msg (in FIFO fashion) and unblocks the sender. This continues the flow till receiver has received all msgs. (Note once the buffer is full, sender remains
 unblocks until receiver collects one msg)
 */
void loop_rec() {
    int recv_msg = receiveb();
    kprintf("received: %d\n", recv_msg);
    sleepms(5);

	recv_msg = receiveb();
	kprintf("received: %d\n", recv_msg);

	recv_msg = receiveb();
	kprintf("received: %d\n", recv_msg);

	recv_msg = receiveb();
	kprintf("received: %d\n", recv_msg);

	recv_msg = receiveb();
	kprintf("received: %d\n", recv_msg);
}
void loop_send(pid32 recv) {
    int sendMsg = 1;
    for(sendMsg = 1; sendMsg<6; sendMsg++) {
    	kprintf("sending: %d\n", sendMsg);
        sendb(recv, sendMsg);
    }
}
process   main(void) {
    pid32 receiverId = create(loop_rec, 8192, 100, "rec",0);
    pid32 senderId = create(loop_send, 8192, 100, "sen", 1,receiverId);
    resume(receiverId);
    resume(senderId);

    return OK;
}
/*
 OUTPUT PART 1 Test 1
sending: 1
received: 1
sending: 2
sending: 3
sending: 4
sending: 5
received: 2
received: 3
received: 4
received: 5
 */

//..............................................PART 1 Test 2.............................................
/*
 Functionality of test case:
 First the receiver is resumed and it tries to read from empty buffer, hence it is blocked. Then sender1 sends one msg and unblocks receiver which receives one msg and
 then goes to sleep. Then sender2 sends two msgs and sender3 sends one msg and now, the buf is full with three msgs. Next the receiver wakes up and read three msgs one by one
 in FIFO fashion and since there is no sender in blocking queue, it reads all last sent three msgs.

 */

/*void sender1(pid32 rec) {
    int sendMsg = 1;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);
}

void sender2(pid32 rec) {
    int sendMsg = 21;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);
    sendMsg=22;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);
}

void sender3(pid32 rec) {
    int sendMsg = 3;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);
}

void receiver() {

    int recv_msg = receiveb();
    kprintf("received: %d\n", recv_msg);
    sleep(12);
    recv_msg = receiveb();
    kprintf("received: %d\n", recv_msg);
    recv_msg = receiveb();
    kprintf("received: %d\n", recv_msg);
    recv_msg = receiveb();
    kprintf("received: %d\n", recv_msg);

}
process   main(void) {
    pid32 receiverId = create(receiver, 8192, 100, "rec",0);
    pid32 sender1Id = create(sender1, 8192, 100, "sen1",1, receiverId);
    pid32 sender2Id = create(sender2, 8192, 100, "sen2",1, receiverId);
    pid32 sender3Id = create(sender3, 8192, 100, "sen3",1, receiverId);

    resume(receiverId);
    sleep(5);
    resume(sender1Id);
    resume(sender2Id);
    resume(sender3Id);

    return OK;
}*/
/*OUTPUT PART 1 Test 2
sending: 1
received: 1
sending: 21
sending: 22
sending: 3
received: 21
received: 22
received: 3
*/


//..............................................PART 1 Test 3.............................................
/*
 Functionality of test case:
 First, receiver goes to sleep and till then sender1 sends three msg and gets blocked. and sender 2,3,4 also gets blocked. Before receiver wakes up, sender2 gets killed,
 hence it no longer remains in the blocking queue of receiver(so this msg will be lost).Now when receiver wakes up, it receives foremost msg and unblocks one sender.
 The flow continues where recv picks one msg (in FIFO) and unblocks another sender in blocking queue. Then receiver dies, hence the last sender's msg will be lost.

 */
/*void sender1(pid32 rec) {
    int sendMsg = 11;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);

    sendMsg = 12;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);

    sendMsg = 13;
	kprintf("sending: %d\n", sendMsg);
	sendb(rec, sendMsg);
}

void sender2(pid32 rec) {
    int sendMsg = 21;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);
}

void sender3(pid32 rec) {
    int sendMsg = 31;
    kprintf("sending: %d\n", sendMsg);
    sendb(rec, sendMsg);
}

void sender4(pid32 rec) {
	int sendMsg = 41;
	kprintf("sending: %d\n", sendMsg);
	sendb(rec, sendMsg);
}

void receiver() {
    sleep(15);
    int recvd_msg = receiveb();
    kprintf("received: %d\n", recvd_msg);

	recvd_msg = receiveb();
	kprintf("received: %d\n", recvd_msg);

	recvd_msg = receiveb();
	kprintf("received: %d\n", recvd_msg);

	recvd_msg = receiveb();
	kprintf("received: %d\n", recvd_msg);
    kill(currpid);
}

process  main(void) {
    pid32 receiverId = create(receiver, 8192, 100, "rec",0);
    pid32 sender1Id = create(sender1, 8192, 100, "sen1",1, receiverId);
    pid32 sender2Id = create(sender2, 8192, 100, "sen2",1, receiverId);
    pid32 sender3Id = create(sender3, 8192, 100, "sen3",1, receiverId);
    pid32 sender4Id = create(sender4, 8192, 100, "sen3",1, receiverId);

    resume(receiverId);
    resume(sender1Id);
    resume(sender2Id);
    resume(sender3Id);
    resume(sender4Id);

    sleep(3);
    kill(sender2Id);
    return OK;
}*/
/*OUTPUT - PART 1 Test 3
sending: 11
sending: 12
sending: 13
sending: 21
sending: 31
sending: 41
received: 11
received: 12
received: 13
received: 31*/



//..............................................PART 2 Test 1.............................................
/*
Functionality of test case:
First, sender sends one msg and goes to sleep. For now, receiver neither calls receive nor registers itself. So this first msg is lost. Then receiver registers itself
for callback function and sender wakes up and sends second msg which will be received asynchronously. Again it goes to sleep and wakes up and sends third msg, which is
also received asynchronously (since the sleep call in receiver is making sure that it is not terminated)
 */
/*umsg32 recvbuf;

int myrecvhandler(void) {
      kprintf("In callback function: msg received = %d\n",recvbuf);
      return(OK);
   }
void sender(pid32 rec) {
    int sendMsg = 0;
    send(rec, sendMsg);
    kprintf("sent: %d\n", sendMsg);
    sleep(10);
    sendMsg++;
    send(rec,sendMsg);
    kprintf("sent: %d\n", sendMsg);
    sendMsg++;
    sleep(10);
    send(rec,sendMsg);
    kprintf("sent: %d\n", sendMsg);

}

void receiver() {

    if (registerrecv(&recvbuf,&myrecvhandler) != OK) {
         kprintf("Registration of receiver callback failed\n");
    }else{
       kprintf("Registration of receiver callback successful\n");
   }
   sleep(50);

}
process   main(void) {
    pid32 receiverId = create(receiver, 8192, 100, "rec", 0);
    pid32 senderId = create(sender, 8192, 100, "sen", 1, receiverId);
    resume(senderId);
    resume(receiverId);

    return OK;
}*/
/* OUTPUT PART 2 Test 1
sent: 0
Registration of receiver callback successful
In callback function: msg received = 1
sent: 1
In callback function: msg received = 2
sent: 2
*/

//..............................................PART 2 Test 2.............................................
/*
 Functionality of test case:
Here, Sender2 process sends one msg to receiver and receiver receives the corresponding msg, and sender2 goes to sleep. Now, sender1 sends its msg to receiver and receiver
receives the corresponding msg. And sender1 goes to sleep. Now, the receiver registers itself for callback function. After this, when sender1 wakes up, msg is received
asynchronously and after that sender2 also wakes up and receives the two msgs sent asynchronously.
 */
/*umsg32 recvbuf;

int myrecvhandler(void) {
      kprintf("In callback function: msg received = %d\n",recvbuf);
      return(OK);
   }

void sender1(pid32 rec) {
    int sendMsg= 10;

    send(rec,sendMsg);
    kprintf("sent: %d\n", sendMsg);
    sleep(10);

    sendMsg++;
    send(rec,sendMsg);
    kprintf("sent: %d\n", sendMsg);

}

void sender2(pid32 rec) {
    int sendMsg=20;

    send(rec,sendMsg);
    kprintf("sent: %d\n", sendMsg);
    sleep(20);

    sendMsg++;
    send(rec,sendMsg);
    kprintf("sent: %d\n", sendMsg);

    sendMsg++;
	send(rec,sendMsg);
	kprintf("sent: %d\n", sendMsg);

}

void receiver() {
    int recvd_msg = receive();
    kprintf("received: %d\n", recvd_msg);

    recvd_msg = receive();
    kprintf("received: %d\n", recvd_msg);

    sleep(10);
    if (registerrecv(&recvbuf,&myrecvhandler) != OK) {
         kprintf("Registration of receiver callback failed\n");
   }else
   {
       kprintf("Registration of receiver callback successful\n");
   }
   sleep(70);

}

process   main(void) {
    pid32 receiverId = create(receiver, 8192, 100, "rec",0);
    pid32 senderId1 = create(sender1, 8192, 100, "sen1",1, receiverId);
    pid32 senderId2 = create(sender2, 8192, 100, "sen2",1, receiverId);

    resume(senderId2);
    resume(receiverId);
    resume(senderId1);

    return OK;
}*/
/* OUTPUT PART 2 Test 2
sent: 20
received: 20
received: 10
sent: 10
Registration oIn callback function: msg received = 11
sent:f receiver  11
callback successful
In callback function: msg received = 21
sent: 21
In callback function: msg received = 22
sent: 22

 */


//..............................................PART 2 Test 3.............................................
/*
 Functionality of test case:
 First, receiver process registers the callback function and goes to sleep, then sender process sends one msg. This received msg will be printed by the callback function.
 Now, sender process goes to sleep. Till then receiver process wakes up and de-registers the callback function. Now, sender wakes up but second msg will not be sent because
 the receiver already has the earlier msg with it.So, it prints the earlier msg only(i.e msg1)
 */
/*umsg32 recvbuf;

int myrecvhandler(void) {
      kprintf("In callback function: msg received = %d\n",recvbuf);
      return(OK);
   }

void sender(pid32 to_whom) {
    int i = 0;
    send(to_whom,i);
    kprintf("sent: %d\n", i);
    sleep(10);
    i++;
    send(to_whom,i);
    kprintf("sent: %d\n", i);
}

void receiver() {

	if (registerrecv(&recvbuf,&myrecvhandler) != OK) {
		kprintf("Registration of receiver callback failed\n");
	 }else{
	   kprintf("Registration of receiver callback successful\n");
   }
	sleep(10);

	if (registerrecv(NULL,NULL) != OK) {
			kprintf("De-registration of receiver callback failed\n");
	 }else{
	   kprintf("De-registration of receiver callback successful\n");
   }
	sleep(10);

     int recv_msg = receive();
     kprintf("received: %d\n", recv_msg);
}

process   main(void) {
    pid32 receiverId = create(receiver, 8192, 100, "rec",0);
    pid32 senderId = create(sender, 8192, 100, "sen", 1,receiverId);
    resume(receiverId);
    resume(senderId);
    return OK;
}*/
/* OUTPUT PART 2 Test 3
Registration of receiver callback successful
In callback function: msg received = 0
sent: 0
De-registratiosent: 1
n of receiver callback successful
received: 0

 */


//..............................................PART 3 Test 1.............................................
/*
 Functionality of test case:
 Here, the main process has requested for 200 number of bytes through gb collection call. And then it tries to de-allocate an address which it never requested. So, the
 freememgb calls returns an error. (But the allocated memory from getmemgb will be de-allocated once the process is terminated, this is verified by internally printing
 the associated data structure for the process)
 */
/*process main(void)
{
    char * allc_add =(char *)getmemgb(200);
    char a = 'A';
    char * var;
    var = &a;
    int free_status=freememgb((char *)var,200);
    kprintf("Status of freememgb call: %d\n",free_status);
    return OK;
}*/
/* OUTPUT PART 3 Test 1
Status of freememgb call: -1
 */

//..............................................PART 3 Test 2.............................................
/*
 Functionality of test case:
 Here, the main process has requested for 200 number of bytes through gb collection call. And then it tries to de-allocate 100 bytes from this allocated_address
 (which is not equal to the no. of bytes it requested). So, the freememgb calls returns an error. (But the allocated memory from getmemgb will be de-allocated
 once the process is terminated, this is verified by internally printing the associated data structure for the process)
 */
/*process main(void)
{
    char * allc_add =(char *)getmemgb(200);
    int free_status=freememgb(allc_add,100);
    kprintf("Status of freememgb call: %d\n",free_status);
    return OK;
}*/
/* OUTPUT PART 3 Test 2
 Status of freememgb call: -1
 */

//..............................................PART 3 Test 3.............................................
/*
Functionality of test case:
Here, receiver process requests three memory blocks and then goes to sleep. And then the main process terminates this process, so these three memory blocks will
be de-allocated even though the respective process doesn't call freememgb explicitly (This is verified by internally printing the associated data structure for the
process. Note the output depends on the memory location allocated for the three memory blocks.
 */
/*void procs(){
    char * allc_add, *allc_add2, *allc_add3;
    allc_add =(char *)getmemgb(40);
    kprintf("Main: allocated add: %d\n",allc_add);

    allc_add2 = (char *)getmemgb(80);
    kprintf("Main: allocated add: %d\n",allc_add2);

    allc_add3 = (char *)getmemgb(100);
    kprintf("Main: allocated add: %d\n",allc_add3);
    sleepms(20);
}

process main(void)
{
    pid32 recId = create(procs, 8192, 100, "rec", 0);
    resume(recId);
    sleepms(7);
    kill(recId);

    return OK;
}*/
/* OUTPUT PART 3 Test 3
Main: allocated add: 1417696
Main: allocated add: 1417752
Main: allocated add: 1417848

 */

