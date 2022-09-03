/*******************************************************
*                  @t.c file                          *
*******************************************************/

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define TRK 18
#define CYL 36
#define BLK 1024

#include "ext2.h"
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;

int prints(char *s)
{
    while(*s) {
        uputc(*s++);
    }
}

int gets(char *s)
{ 
    while((s = getc()) != '\r') {
        putc(*s);
        s++;
    }
    *s = 0;
}


u16 NSEC = 2;
char buf1[BLK], buf2[BLK];

u16 getblk(u16 blk, char *buf)
{
    readfd( (2*blk)/CYL, ( (2*blk)%CYL)/TRK, ((2*blk)%CYL)%TRK, buf);

    // readfd( blk/18, ((blk)%18)/9, ( ((blk)%18)%9)<<1, buf);
}

u16 search(INODE *ip, char *name)
{
  //search for name in the data block of INODE; 
  //return its inumber if found
  //else error();
    DIR *dp;
    int ino = -1;

    getblk(ip->i_block[0], buf2);
    
    dp = (DIR *)buf2;
    while(strcmp(dp->name, name) != 0 && (dp < dp + BLK)) {
        dp += dp->rec_len;
    }

    ino = dp->inode;

    return ino;
}

main()
{ 
    u16 index, iblk, ino;
    INODE *ip;
    GD *gp;
    DIR *dp;
    int *indir;

    puts("BRG Booter:");
1//. Write YOUR C code to get the INODE of /boot/mtx
 //  INODE *ip --> INODE

 //  if INODE has indirect blocks: get i_block[12] u32 buf2[256]

    getblk(2, buf1);
    gp = (gD *)buf1;
    iblk = (u16)gp->bg_inode_table;

    getblk(iblk, buf1);
    ip = (INODE *)buf1 + 1;
    iblk = (u16)ip->i_block[0];

    dp = (DIR *)buf1;
    ino = search(ip, "boot");
    getblk(ino, buf1);
    ip = (INODE *)buf1;

    ino = search(ip, "mtx");
    getblk(ino, buf1);
    ip = (INODE *)buf1;


//2. setes(0x1000);  // MTX loading segment = 0x1000
    setes(0x1000);

//3. load 12 DIRECT blocks of INODE into memory
    for (index = 0; index < 12; index++) {
        getblk(ip->i_block[index], 0);
        inces();
        putc('*');
    }

//4. load INDIRECT blocks, if any, into memory
    getblk(ip->i_block[12], buf2);
    indir = (u32 *)buf2;

    while (*indir)
    {
        /* code */
        getblk((u16)*indir, 0);
        inces();
        putc('.');
        indir++;
    }
    

   prints("go?"); getc();
}  
