/**************** uart.c file ****************/
#define UDR   0x00
#define UFR   0x18

#define RXFE 0x10
#define TXFF 0x20

typedef struct uart{
  char *base;
  int n;
}UART;

UART uart[4];

int uart_init()
{
  int i; UART *up;

  for (i=0; i<4; i++){
    up = &uart[i];
    up->base = (char *)(0x101F1000 + i*0x1000);
    up->n = i;
  }
  uart[3].base = (char *)(0x10009000); // uart3 at 0x10009000
}

int ugetc(UART *up)
{
  while (*(up->base + UFR) & RXFE);
  return *(up->base + UDR);
}

int uputc(UART *up, char c)
{
  while(*(up->base + UFR) & TXFF);
  *(up->base + UDR) = c;
}

int ugets(UART *up, char *s)
{
  while ((*s = (char)ugetc(up)) != '\r'){
    uputc(up, *s);
    s++;
  }
 *s = 0;
}

int uputs(UART *up, char *s)
{
  while(*s){
    uputc(up, *s);
    if (*s == '\n')
      uputc(up, '\r');
    s++;
  }
}

/** WRITE YOUR uprintf(UART *up, char *fmt, . . .) for formatted print **/
int uprints(UART *up, char *s)
{
  while(*s)
    uputc(up, *s++);
}


char *tab = "0123456789ABCDEF";
int BASE = 10;

int urpu(UART *up, unsigned int x)
{
    char c;
    if (x){
        c = tab[x%BASE];
        urpu(up, x/BASE);
	    uputc(up, c);
    }
}

// unsinged int as decimal
int uprintu(UART *up, unsigned int x)
    {
    BASE = 10;
    if (x==0)
        uputc(up, '0');
    else
        urpu(up, x);
    //uputc(up, ' ');
}

int uprintd(UART *up, int x) {
    BASE = 10;
    if (x == 0) {
        uputc(up, '0');
    }
    else if (x < 0) {
        uputc(up, '-');
        urpu(up, (unsigned int)(x * -1));
    }
    else {
        urpu(up, (unsigned int)x);
    }
}

// int as hex
int uprintx(UART *up, int x) {
    BASE = 16;
    uprints(up, "0x");
    if (x == 0) {
        uputc(up, '0');
    }
    else {
        urpu(up, (unsigned int)x);
    }
}

//unsigned int as hex

int ufprintf(UART *up, char *fmt, ...) 
{
    char *cp = fmt;
    int *ip = (int *)&fmt;
    int i = 0;
    char temp;

    while(fmt[i] != 0) {
        temp = fmt[i];
        if (temp == '%') {
            ++i;
            temp = fmt[i];
            ++ip;

            switch (temp)
            {
            case 's':
                uprints(up, (char *)ip);
                break;
            case 'c':
                uputc(up, *ip);
                break;
            case 'd':
                uprintd(up, *ip);
                break;
            case 'u':
                uprintu(up, *ip);
                break;
            case 'x':
                uprintx(up, *ip);
                break;
            default:
                break;
            }
        } 
        else if (temp == '\n') {
            uputc(up, '\n');
            uputc(up, '\r');
        }
        else {
            uputc(up, temp);
        }

        i++;
    }
}