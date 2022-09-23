/****************** vid.c file *******************/
int volatile *fb;  // display buffer

int fbuf_init()
{
  int i;

  /********* for VGA 640x480 ************************/
  *(volatile unsigned int *)(0x1000001c) = 0x2C77;        // LCDCLK SYS_OSCCLK
  *(volatile unsigned int *)(0x10120000) = 0x3F1F3F9C;    // time0
  *(volatile unsigned int *)(0x10120004) = 0x090B61DF;    // time1
  *(volatile unsigned int *)(0x10120008) = 0x067F1800;    // time2
  *(volatile unsigned int *)(0x10120010) = (0x400000);    // panelBaseAddress
  *(volatile unsigned int *)(0x10120018) = 0x82B;         // control register

  // |code|data|bss|wsu.o|stack| may > 3MB; fb must be higher
  fb = (int *)(0x400000);  // fb[ ] at 4MB; for VGA 640x480
  // clear fb[]
  for (i=0; i<640*480; i++)
    fb[i] = 0x00000000;    // clean screen; all pixels are BLACK
}

int show_bmp(char *p, int startRow, int startCol)
{ 
   int h, w, pixel, r2, i, j; 
   unsigned char r, g, b;
   char *pp;
 
   int *q = (int *)(p+14); // skip over 14 bytes file header 
   q++;                    // skip 4 bytes in image header
   w = *q;                 // width in pixels 
   h = *(q + 1);           // height in pixels

   p += 54 + (4*((3*w+3)/4) * h);                // p point at pixels now 

   // AS SHOWN, the picture is up-side DOWN
   r2 = 4*((3*w+3)/4);     // row size is a multiple of 4 bytes  

   for (i=startRow; i<h+startRow; i++){
     pp = p;
     
     for (j=startCol; j<startCol+w; j++){
         b = *pp; g = *(pp+1); r = *(pp+2);
         pixel = (b<<16) + (g<<8) + r;
         fb[i*640 + j] = pixel;
         pp -= 3 * 2;    // back pp by 3 bytes
     }
     p -= r2 * 2;
   }

    /*
    for (i=h+startRow-1; i>=startRow; i-=1){
     pp = p;
     
     for (j=startCol+w-1; j>=startCol; j-=1){
         b = *pp; g = *(pp+1); r = *(pp+2);
         pixel = (b<<16) + (g<<8) + r;
         fb[i*640 + j] = pixel;
         pp += 3;    // back pp by 3 bytes
     }
     p += r2;
   }
  */

   // REQUIRED: use YOUR uprintf() of Part 1 to print h,w to UART0
   //ufprintf( &uart[0],"\nBMP image height=%d width=%d\n", h, w);
   ufprintf( &uart[0],"\nBMP image height=%d width=%d\n", h/2, w/2);
}
