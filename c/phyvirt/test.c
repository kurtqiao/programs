#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define MEM_DEV "/dev/mem"

int main(){
  int fd_mem = -1;
  void *virt_addr;

  fd_mem = open(MEM_DEV, O_RDWR | O_SYNC);
  if (-1 == fd_mem)
    printf("open /dev/mem fail!\n");
  
  virt_addr = mmap(NULL,0x4000, PROT_WRITE | PROT_READ, MAP_SHARED, fd_mem,\
              (off_t)0xfed1c000);
  if (MAP_FAILED == virt_addr)
    printf ("map failed!\n");
  virt_addr += 0x3800;
  printf("mem %lx: %lx\n", (unsigned long )virt_addr, *(unsigned long *)virt_addr);
  return 0;
}
