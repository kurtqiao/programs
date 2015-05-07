#include <sys/io.h>
#include <stdio.h>
#include <stdlib.h>
#include "pci.h"

/*
use iopl(3) to open all io ports
*/
int
get_io_ctrl(){

  if (iopl(3)){
    return 1;
  }
  return 0;
}

u8
pci_read_u8(u64 pci_addr, u8 reg){
  outl(pci_addr|(reg&~3), INDEX_PORT); 
  return (u8)inb(DATA_PORT + (reg&3));
}

u16
pci_read_u16(u64 pci_addr, u8 reg){
  outl(pci_addr|(reg&~3), INDEX_PORT);
  return (u16)inw(DATA_PORT + (reg&3));
}

u32
pci_read_u32(u64 pci_addr, u8 reg){
  outl(pci_addr|(reg&~3), INDEX_PORT);
  return (u32)inl(DATA_PORT + (reg&3));
}

pci_dev * new_pci_dev(){
  pci_dev *pdnode = malloc(sizeof(*pdnode));
  pdnode->next = NULL;
  return pdnode;
}

void free_pci_dev(pci_dev_header *pdhdr){
  pci_dev *tmpptr=NULL;
  pci_dev *nxtptr=NULL;
  tmpptr = pdhdr->pdnode;
  free(pdhdr);
  do{
    nxtptr = tmpptr;
    tmpptr = tmpptr->next;
    free(nxtptr);
  }while(tmpptr != NULL);

}

/*
*/
pci_dev_header *
probe_pci(){
  int bus, dev, func, locate;
  u8  t_data, t_rev, t_class_code=0xff;
  u8  t_mul_func=0;
  u64 t_pci_dev;
  u32 t_did_vid;
  pci_dev_header *pdptr=NULL;
  pci_dev *tmpptr=NULL, *indexptr=NULL;
 
  //pdptr = malloc(sizeof(*pdptr));
  pdptr = calloc(1, sizeof(pci_dev_header));
  pdptr->pdnode = NULL;
 //strcopy pdptr->signature = ('P','D','E','V');
/* check class code for devices, 
*if not pci bridge, no need to probe next bus
*if class code is 0xff, no deivce
*/
  printf("start probe pci...\n");
  for (bus=0; bus<=PCI_MAX_BUS; bus++){
    for (dev=0; dev<=PCI_MAX_DEV; dev++){
	t_mul_func = 0;
      for (func=0; func<=PCI_MAX_FUNC; func++){
	t_pci_dev=PCI_DEV(bus,dev,func);		

	t_class_code = pci_read_u8(t_pci_dev, REG_CLASS);
	if ((t_class_code == 0xFF)|(t_class_code == 0x00))
		continue;
	t_did_vid = pci_read_u32(t_pci_dev, 0);
	t_rev = pci_read_u8(t_pci_dev, REG_REV);
	//if not multifunction, read bit
        //if yes, leave it as mul_func
	if (!t_mul_func)
	  t_mul_func = pci_read_u8(t_pci_dev, REG_H_TYPE)&0x80;

/*	if (t_class_code = 0x06)
		//host bridge, read bus
		pci_read_u16(t_pci_dev, 0x00);*/

	tmpptr = new_pci_dev();
	tmpptr->bus = bus;
	tmpptr->dev = dev;
	tmpptr->func = func;
	tmpptr->vendor_id = t_did_vid&0xffff;
	tmpptr->device_id = t_did_vid>>16;
	tmpptr->rev_id = t_rev;
	tmpptr->class_code = t_class_code;

	if (pdptr->pdnode == NULL){
	  pdptr->pdnode = tmpptr;
	  indexptr = tmpptr;
	}else{
	  indexptr->next = tmpptr;
	  indexptr = tmpptr;
	}
	pdptr->node_counter++;
/*
	printf("%x.%.2x.%x %.4X:%.4X %.2X %.2x ", \
	bus, dev, func, t_did_vid&0xffff,t_did_vid>>16, \
	t_class_code, t_mul_func);
	t_rev?printf("(rev %.2x)\n", t_rev):printf("\n");
*/
	if (!t_mul_func)
		break;
      }
    }
  }

  return pdptr;
}
