/********************************************************************
Copyright 2010-2017 K.C. Wang, <kwang@eecs.wsu.edu>
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

int exec(char *cmdline) // cmdline=VA in Uspace
{
  //printf("exec: under construction\n");
  //return -1;
  // exec.c code as in Chpater 7.7.7: REPLACE loadelf() with load()

  int i, upa, usp;
  char *cp, kline[128], file[32], filename[32];
  PROC *p = running;

  strcpy(kline, cmdline);
  cp = kline; i = 0;
  while (*cp != ' ') {
    filename[i] = *cp;
    i++; cp++;
  }

  filename[i] = 0;
  file[0] = 0;

  if (filename[0] != '/') {
    strcpy(file, "/bin/");
  }
  kstrcat(file, filename);

  upa = p->pgdir[2048] & 0xffff0000;

  if (!load(file, p)) {
    return -1;
  }

  usp = upa + 0x100000 - 128;
  strcpy((char *)usp, kline);
  p->usp = (int *)VA(0x100000 - 128);

  for (i = 2; i < 14; i++) {
    p->kstack[SSIZE - i] = 0;
  }
  p->kstack[SSIZE - 1] = (int)VA(0x0);
  return (int)p->usp;
}
