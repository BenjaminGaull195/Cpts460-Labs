int tswitch();

int ksleep(int event)
{
  printf("ksleep: under construction\n");
}

int kwakeup(int event)
{
  printf("kwakeup: under construction\n");
}

int kexit(int exitValue)
{
  printf("proc %d in kexit(), value=%d\n", running->pid, exitValue);
  running->exitCode = exitValue;
  running->status = ZOMBIE;
  kwakeup(running->parent);
  tswitch();
}


  
