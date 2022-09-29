// kernel.c file

#define NPROC 9
/*********** in type.h ***************
typedef struct proc{
  struct proc *next;
  int    *ksp;

  int    pid;
  int    ppid;
  int    priority;
  int    status;
  int    event;
  int    exitCode;

  struct proc *parent;
  struct proc *child;
  struct proc *sibling;
  
  int    kstack[SSIZE];
}PROC;
***************************************/
PROC proc[NPROC], *running, *freeList, *readyQueue, *sleepList;
int procsize = sizeof(PROC);

int body();

void add_child(PROC* parent, PROC* proc);
PROC* remove_child(PROC* parent, int pid);
void move_child(PROC* proc);

int init()
{
  int i; 
  PROC *p;
  kprintf("kernel_init()\n");
  for (i=0; i<NPROC; i++){
    p = &proc[i];
    p->pid = i;
    p->ppid = 0;
    p->status = FREE;
    p->next = p + 1;
  }
  proc[NPROC-1].next = 0; 
  freeList = &proc[0];    // freeList = ALL free procs 
  printList("freeList", freeList);
  
  readyQueue = 0;
  sleepList = 0;
  
  // creat P0 as running process
  p = dequeue(&freeList); // take proc[0] off freeList
  p->priority = 0;
  p->status = READY;
  p->ppid = 0;
  running = p;           // running -> proc[0] with pid=0

  kprintf("running = %d\n", running->pid);
  printList("freeList", freeList);
}

  
int kfork(int func, int priority)
{
  int i;
  PROC *p = dequeue(&freeList);
  if (p==0){
    printf("no more PROC, kfork failed\n");
    return 0;
  }
  p->status = READY;
  p->priority = priority;
  p->ppid = running->pid;
  p->parent = running;
  
  // set kstack for new proc to resume to func()
  // stmfd sp!, {r0-r12, lr} saves regs in stack as
  // stack = lr r12 r11 r10 r9 r8 r7 r6 r5  r4  r3  r2  r1  r0
  // HIGH    -1 -2  -3  -4  -5 -6 -7 -8 -9 -10 -11 -12 -13 -14   LOW
  for (i=1; i<15; i++)
      p->kstack[SSIZE-i] = 0;
  p->kstack[SSIZE-1] = (int)func;  // saved regs in dec address ORDER !!!
  p->ksp = &(p->kstack[SSIZE-14]);
  enqueue(&readyQueue, p);

  //update proc family tree
  add_child(running, p);

  printf("proc %d kforked a child %d\n", running->pid, p->pid);
  printList("readyQueue", readyQueue);
  return p->pid;
}

int scheduler()
{
  // kprintf("proc %d in scheduler ", running->pid);
  if (running->status == READY)
     enqueue(&readyQueue, running);
  if (running->status == SLEEP) {
     enqueue(&sleepList, running);
  }
  running = dequeue(&readyQueue);
  // kprintf("next running = %d\n", running->pid);
  if (running->pid){
     color = running->pid;
  }
}

// code of processes
int body()
{
  char c, cmd[64];
  int status;

  kprintf("proc %d resume to body()\n", running->pid);
  while(1){
    printf("-------- proc %d running -----------\n", running->pid);
    printf("-------- parent %d -----------------\n", running->ppid);
    printList("freeList  ", freeList);
    printList("readyQueue", readyQueue);
    printsleepList(sleepList);
	
    printf("Enter a command [switch|kfork|kwait|exit] : ");
    kgets(cmd);
    printf("\n");
    
    if (strcmp(cmd, "switch")==0)
       tswitch();
    else if (strcmp(cmd, "kfork")==0)
       kfork((int)body, 1);
    else if (strcmp(cmd, "kwait")==0) {
       printf("Zombie child found: %d status: %d",kwait(&status), status);
    }
    else if (strcmp(cmd, "exit")==0){
      printf("exit code: ");
      kgets(cmd);
      kexit(atoi(cmd));
    }
  }
}




void add_child(PROC* parent, PROC* proc) {
	proc->parent = parent;

	if (parent->child == 0) {
		parent->child = proc;
		return;
	}

	PROC* temp = parent->child;
	while (temp->sibling != 0) {
		temp = temp->sibling;
	}
	temp->sibling = proc;
}

PROC* remove_child(PROC* parent, int pid) {
	if (parent->child == 0) {
		return 0;
	}
	
	PROC* child = parent->child;
	PROC* next;
	if (child->pid == pid) {
		parent->child = child->sibling;
		return child;
	}

	next = child->sibling;
	while (child->sibling) {
		if (next->pid == pid) {
			child->sibling = next->sibling;
			return next;
		}

		child = child->sibling;
	}

}

void move_child(PROC* proc) {
	PROC* parent = proc->parent;
	PROC* child = proc->child;
	add_child(parent, child);
	proc->child = 0;
}