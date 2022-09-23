# create .o files from .bmp image files; to be included as RAW data sections
arm-none-eabi-objcopy -I binary -O elf32-littlearm -B arm wsu.bmp wsu.o

# show exported symbols
nm -S -t d wsu.o

LIB=/usr/lib/gcc/arm-none-eabi/9.2.1/     # for Ubuntu 20.4

# compile-link
arm-none-eabi-as -mcpu=arm926ej-s ts.s -o ts.o
arm-none-eabi-gcc -c -mcpu=arm926ej-s t.c -o t.o 
arm-none-eabi-ld -T t.ld ts.o t.o -o t.elf  -L $LIB -lgcc      # use libgcc.a
arm-none-eabi-objcopy -O binary t.elf t.bin

rm *.o *.elf

echo ready to go?
read dummy

qemu-system-arm -M versatilepb -m 128M -kernel t.bin -serial mon:stdio