PROC* find_zombie() {
  PROC *p = proc;

  while (p) {
    if (p->status == ZOMBIE) {
      return p;
    }
  }

  return 0;
}

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

int kexit(int value)
{
  printf("proc %d exit\n", running->pid);
  move_child(running);
  running->exitCode = value;
  running->status = ZOMBIE;

  kwakeup(running->parent);

  tswitch();
}

int kwait(int *status)
{
  PROC* child;
  int pid = -1;
  
  while (1) {
    child = find_zombie();
    if (child) {
      pid = child->pid;
      *status = child->exitCode;
      enqueue(&freeList, child);
      return pid;
    }
    ksleep(running);
  }
}



