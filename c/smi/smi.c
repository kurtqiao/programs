#include <libgen.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <dos.h>

typedef unsigned char         u8;
typedef unsigned short        u16;
typedef unsigned int          u32;
typedef unsigned long long    u64;

#define help_str  "\
sw smi utility v1.0 2016.8.25\n \
usage: smi options data\n \
options:\n \
-p  sw smi port, default is 0xB2 \n \
-a  eax value \n \
-b  ebx value \n \
-c  ecx value \n "

void
sw_smi(u16 port, union REGS *regs)
{
  u32 eaxdata = regs->x.eax;
  u32 ebxdata = regs->x.ebx;
  u32 ecxdata = regs->x.ecx;
  u32 edxdata = 0;

  printf("trigger sw smi, eax: %x ebx: %x ecx: %x\n", regs->x.eax, regs->x.ebx, regs->x.ecx);
  _asm
  {
    mov eax, eaxdata
    mov ebx, ebxdata
    mov ecx, ecxdata
    mov dx, port
    out dx, al
    out 0xed, al
    mov eaxdata, eax
    mov ebxdata, ebx
    mov ecxdata, ecx
    mov edxdata, edx
  }
  regs->x.eax = eaxdata;
  regs->x.ebx = ebxdata;
  regs->x.ecx = ecxdata;
  regs->x.edx = edxdata;
}
/*
*/
int
main(
  int argc,
  char * argv[]
  )
{
  int option;
  u16 smiport=0xB2;
  u32 dataeax=0, dataebx=0, dataecx=0;
  union REGS regs;
  
  while ((option = getopt(argc, argv, ":p:a:b:c:")) !=-1)
  {
    switch(option)
    {
      default:
        fprintf(stderr, "internal error: unknown case %c\n", optopt);
        return 1;
      case ':':
        fprintf(stderr, "error: missing argument for option -%c\n", optopt);
        return 1;
      case 'p':
        sscanf(optarg, "%x", &smiport);
        break;
      case 'a':
        sscanf(optarg, "%x", &dataeax);
        break;
      case 'b':
        sscanf(optarg, "%x", &dataebx);
        break;
      case 'c':
        sscanf(optarg, "%x", &dataecx);
        break;
        
    }
  }
//  printf("optind %d, argc %d \n", optind, argc);
  if (optind < argc || optind ==  1) {
    printf(help_str);
    return 1;
  }
  regs.x.eax = dataeax;
  regs.x.ebx = dataebx;
  regs.x.ecx = dataecx;

//  printf("scan data eax: %x ebx: %x ecx: %x\n", dataeax, dataebx, dataecx);
//  printf("set_regs eax: %x ebx: %x ecx: %x\n", regs.x.eax, regs.x.ebx, regs.x.ecx);
  sw_smi(smiport, &regs);
  printf("read back \neax: %x \nebx: %x \necx: %x\nedx: %x\n", \
         regs.x.eax, regs.x.ebx, regs.x.ecx, regs.x.edx);
  return 0;
  
}

