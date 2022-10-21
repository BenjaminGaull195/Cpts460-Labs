// wait.c file

extern PROC *running;
extern PROC *sleepList;

PROC* remove_from_list(PROC** list, int event) {
	PROC* current = *list;
	PROC* next;
	if (current == 0) {
		return 0;
	}
	else if (current->event == event) {
		*list = current->next;
		return current;
	}
	else {
		while (current->next)
		{
			next = current->next;
			if (next->event == event) {
				current->next = next->next;
				return next;
			}
			current = current->next;
		}
		return 0;
	}
}

PROC* find_zombie(PROC* parent) {
  PROC* child = parent->child;
  do {
    if (child->status == ZOMBIE) {
      return child;
    }

    child = child->sibling;
  } while (child != 0);
  return 0;
}

int kwait(int *status) {
  PROC* child;
  int pid;

  if (running->child == 0) {
    return -1;
  }
  while (1) {
    child = find_zombie(running);
    if (child) {
      pid = child->pid;
      *status = child->exitCode;
      enqueue(&freeList, remove_child(running, pid));
      return pid;
    }
    ksleep(running);
  }
}

int kexit(int exitValue)  // SIMPLE kexit() for process to terminate
{
  printf("proc %d exit\n", running->pid);
  move_child(running);
  running->exitCode = exitValue;
  running->status = ZOMBIE;

  kwakeup(running->parent);

  tswitch();
}

int ksleep(int event)
{
  // implement this
  int SR = int_off();

  running->event = event;
  running->status = SLEEP;

  tswitch();
  int_on(SR);
}

int kwakeup(int event)
{
  // implement this
  int SR = int_off();
  int i;
  PROC *p;
  for (i = 0; i < NPROC; i++) {
    p = &proc[i];
    if (p->status == SLEEP && p->event == event) {
      p->status = READY;
      enqueue(&readyQueue, remove_from_list(&sleepList, event));
    }
  }

  int_on(SR);
}
