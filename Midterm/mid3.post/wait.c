// wait.c file: ksleep(), kwakeup(), kexit()

extern PROC *running;
extern PROC *readyQueue;
extern PROC *sleepList;

int ksleep(int event)
{
  // your ksleep()
}

int kwakeup(int event)
{
  // your kwakeup()
}

int kexit(int exitCode)
{
  running->exitCode = exitCode;
  running->status = ZOMBIE;
  running->priority = 0;
  kwakeup(running->parent);
  tswitch();
}
