/*
  a test program lspci in linux by io port r/w
*/
#include <stdio.h>
#include "pci.h"


int
main(
  int argc,
  char *argv[]
)
{
  pci_dev_header *pdptr=NULL;
  pci_dev *pdnode=NULL;
  int counter;

  if(get_io_ctrl()){
    printf("need run as root, open io control fail!\n");
    return 1;
  }

  pdptr = probe_pci();

  counter = pdptr->node_counter;
  pdnode = pdptr->pdnode;
  while(counter--){
  printf("%x.%.2x.%x %X:%.4X ", pdnode->bus, pdnode->dev, pdnode->func,\
	 pdnode->vendor_id, pdnode->device_id);

  pdnode->rev_id?printf("(rev %.2x)\n", pdnode->rev_id):printf("\n");

  pdnode = pdnode->next;

  }
  free_pci_dev(pdptr);
  return 0;
}
