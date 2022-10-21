// pipe.c file

#define PSIZE 8

typedef struct pipe{
  char buf[PSIZE];
  int head, tail, data, room;
  int status;
  // int nreader, nwriter;    // number of reader, writer on this pipe
}PIPE;


#define NPIPE  4
PIPE pipe[NPIPE];

int pipe_init()
{
  // fill in code
}

PIPE *create_pipe()
{
  // fill in code
}

int read_pipe(PIPE *p, char *buf, int n)
{
  // fill in code;
  // add code to handle: no data AND no writer: return 0
}

int write_pipe(PIPE *p, char *buf, int n)
{
  // fill in code;
  // add code to detect BROKEN pipe: print BROKEN pipe message, then exit
}


  
