// timer.c file
/***** timer confiuguration values *****/
#define CTL_ENABLE          ( 0x00000080 )
#define CTL_MODE            ( 0x00000040 )
#define CTL_INTR            ( 0x00000020 )
#define CTL_PRESCALE_1      ( 0x00000008 )
#define CTL_PRESCALE_2      ( 0x00000004 )
#define CTL_CTRLEN          ( 0x00000002 )
#define CTL_ONESHOT         ( 0x00000001 )

// timer register offsets from base address
/**** byte offsets *******
#define TLOAD   0x00
#define TVALUE  0x04
#define TCNTL   0x08
#define TINTCLR 0x0C
#define TRIS    0x10
#define TMIS    0x14
#define TBGLOAD 0x18
*************************/
/** u32 * offsets *****/
#define TLOAD   0x0
#define TVALUE  0x1
#define TCNTL   0x2
#define TINTCLR 0x3
#define TRIS    0x4
#define TMIS    0x5
#define TBGLOAD 0x6

typedef volatile struct timer{
  u32 *base;            // timer's base address; as u32 pointer
  int tick, hh, mm, ss; // per timer data area
  char clock[16]; 
}TIMER;

typedef struct timer_queue {
  struct timer_queue *next;
  int time;
  PROC* proc;
} tqe;

tqe tqElements[NPROC];
tqe* timerQueue;

void print_timerQueue(tqe* queue);

volatile TIMER timer[4];  // 4 timers; 2 timers per unit; at 0x00 and 0x20

extern int kpchar(char, int, int);
extern int unkpchar(char, int, int);
extern int kputc(char);
extern int strcpy(char *dest, char *src);

int k;

void timer_init()
{
  int i;
  TIMER *tp;
  tqe* t;
  kprintf("timer_init()\n");

  for (i = 0; i < NPROC; i++) {
    t = &tqElements[i];
    t->time = 0;
    t->next = 0;
    t->proc = 0;
  }

  for (i=0; i<4; i++){
    tp = &timer[i];
    if (i==0) tp->base = (u32 *)0x101E2000; 
    if (i==1) tp->base = (u32 *)0x101E2020; 
    if (i==2) tp->base = (u32 *)0x101E3000; 
    if (i==3) tp->base = (u32 *)0x101E3020;

    *(tp->base+TLOAD) = 0x0;   // reset
    *(tp->base+TVALUE)= 0x0;
    *(tp->base+TRIS)  = 0x0;
    *(tp->base+TMIS)  = 0x0;
    *(tp->base+TCNTL) = 0x62; //011-0000=|En|Pe|IntE|-|scal=00|32-bit|0=wrap|
    *(tp->base+TBGLOAD) = 0xE0000/60;

    tp->tick = tp->hh = tp->mm = tp->ss = 0;
    //strcpy(tp->clock, "00:00:00");
    //for some reason fails to compile do to undefined reference to memcpy
    tp->clock[0] = '0';
    tp->clock[1] = '0';
    tp->clock[2] = ':';
    tp->clock[3] = '0';
    tp->clock[4] = '0';
    tp->clock[5] = ':';
    tp->clock[6] = '0';
    tp->clock[7] = '0';
  }
}

int cursorState = 0;

void timer_handler(int n){
    int i;
    TIMER *t = &timer[n];
    tqe* tq;
    t->tick++;

    if (t->tick == 60){
      t->tick = 0;
      t->ss++;
      timerQueue->time--;
      if (t->ss == 60) {
        t->ss = 0;
        t->mm++;
        if (t->mm == 60) {
          t->mm = 0;
          t->hh++;
        }
      }
      
      //handle timer queue;
      if (timerQueue->time == 0) {
        while (timerQueue->time == 0)
        {
          kwakeup(timerQueue->proc);
          timerQueue->proc = 0;
          timerQueue = timerQueue->next;
        }
      }

      for (i = 0; i < 8; i++) {
        unkpchar(t->clock[i], n, 70 + i);
      }

      t->clock[7] = '0' + (t->ss % 10); t->clock[6] = '0' + (t->ss / 10);
      t->clock[4] = '0' + (t->mm % 10); t->clock[3] = '0' + (t->mm / 10);
      t->clock[1] = '0' + (t->hh % 10); t->clock[0] = '0' + (t->hh / 10);
      
      color = n;
      

      for (i = 0; i < 8; i++) {
        
        kpchar(t->clock[i], n, 70 + i);
      }
      
      
      //kputs("timer interrupt\n");
      color = YELLOW;
      print_timerQueue(timerQueue);
      //clrcursor();

    }
    

    timer_clearInterrupt(n);
}

void timer_start(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];

  *(tp->base+TCNTL) |= 0x80;    // set enable bit 7
}

int timer_clearInterrupt(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TINTCLR) = 0xFFFFFFFF;
}

void timer_stop(int n) // timer_start(0), 1, etc.
{
  TIMER *tp = &timer[n];
  *(tp->base+TCNTL) &= 0x7F;  // clear enable bit 7
}


tqe* get_available_tqe() {
  int i;
  tqe* t;
  for (i = 0; i < NPROC; i++) {
    t = &tqElements[i];
    if (t->proc == 0) {
      return t;
    }
  }

  return 0;
}

void timer_enqueue(tqe** queue, int time) {
	tqe* element = get_available_tqe();
  element->time = time;
  element->proc = running;
  
  if (*queue == 0) {
		*queue = element;
		return;
	}
	
	tqe* current = *queue;
	//tqe* prev = 0;
	tqe* temp;
	temp = element;

	// test if time is less than current head of queue
	if (temp->time < current->time) {
		temp->next = current;
		current->time -= temp->time;

		*queue = temp;
		return;
	}

	if (current->next == 0) {
		temp->next = current->next;
		current->next = temp;
		temp->time -= current->time;

		return;
	}

	while (current->next != 0) 
	{
		temp->time -= current->time;

		if (temp->time <= current->next->time) {
			temp->next = current->next;
			current->next = temp;
			if (temp->next != 0) {
				temp->next->time -= temp->time;
			}

			return;
		}
		
		current = current->next;
	}

	temp->next = current->next;
	current->next = temp;
	temp->time -= current->time;

	return;
}

void t(int time) {
  timer_enqueue(&timerQueue, time);
  ksleep(running);
}

void print_timerQueue(tqe *queue) {
  tqe* tq = queue;
  printf("Timer Queue: ");
  while (tq) {
    printf("{%d, %d} ", tq->time, tq->proc->pid);
  }
  printf("\n");
}


