// isr.c, 159
// ISRs called from Kernel()

#include "spede.h"
#include "types.h"
#include "isr.h"
#include "q_mgmt.h"
#include "externs.h"
#include "proc.h"

void MyBzero(void *s, int n) {
	int i;
	for (i = 0; i < n; i++) {
		*((char*)s+n) = '\0';
	}
}

void SpawnISR(int pid)
{
   MyBzero((void *)user_stacks[pid], USER_STACK_SIZE);
// 1st. point to just above of user stack, then drop by 64 bytes (tf_t)
   pcbs[pid].tf_p = (tf_t *)&user_stacks[pid][USER_STACK_SIZE];
   pcbs[pid].tf_p--;    // pointer arithmetic, now points to trapframe

// fill in CPU's register context
   pcbs[pid].tf_p->eflags = EF_DEFAULT_VALUE|EF_INTR;
   pcbs[pid].tf_p->eip = (unsigned int)SimpleProc; // new process code
   pcbs[pid].tf_p->cs = get_cs();
   pcbs[pid].tf_p->ds = get_ds();
   pcbs[pid].tf_p->es = get_es();
   pcbs[pid].tf_p->fs = get_fs();
   pcbs[pid].tf_p->gs = get_gs();

   pcbs[pid].tick_count = pcbs[pid].total_tick_count = 0;
   pcbs[pid].state = READY;

   if(pid != 0) EnQ(pid, &ready_q);  // IdleProc (PID 0) is not queued
}


void KillISR()
{
   if(cur_pid < 1) return; // skip Idle Proc or when cur_pid has been set to -1
      
   pcbs[cur_pid].state = AVAIL;
   EnQ(cur_pid, &avail_q);
   cur_pid = -1;          // no proc running any more
}        

void ShowStatusISR()
{
   int i;
   char *state_names[] = {"AVAIL\0", "READY\0", "RUN\0", "SLEEP\0", "WAIT\0"};

   for(i=0; i<NUM_PROC; i++)
   {
      printf("pid %i, state %s, tick_count %i, total_tick_count %i \n",
             i,
             state_names[pcbs[i].state],
             pcbs[i].tick_count,
             pcbs[i].total_tick_count);
   }

   printf("READY: ");
   for (i=0; i<NUM_PROC; i++)
   {
      printf("%i ", ready_q.q[i]);
   }
   printf("\n       Head: %i  Tail: %i  Count: %i\n", ready_q.head, ready_q.tail, ready_q.count);

   printf("AVAIL: ");
   for (i=0; i<NUM_PROC; i++)
   {
      printf("%i ", avail_q.q[i]);
   }
   printf("\n       Head: %i  Tail: %i  Count: %i\n", avail_q.head, avail_q.tail, avail_q.count);

}

void TimerISR()
{
   outportb(0x20, 0x60); // dismiss timer interrupt
   if(cur_pid == 0) return; // if Idle process, no need to do this on it


   pcbs[cur_pid].tick_count++;

   if(pcbs[cur_pid].tick_count == TIME_SLICE) // running up time, preempt it?
   {
      pcbs[cur_pid].tick_count = 0; // reset (roll over) usage time
      pcbs[cur_pid].total_tick_count += TIME_SLICE; // sum to total

      pcbs[cur_pid].state = READY;  // change its state
      EnQ(cur_pid, &ready_q);       // move it to ready_q

      cur_pid = -1;                 // no longer running
   }
}
