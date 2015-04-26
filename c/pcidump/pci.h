#ifndef __T_PCI_H
#define __T_PCI_H

#define INDEX_PORT   0xCF8
#define DATA_PORT    0xCFC

#define PCI_MAX_BUS  255
#define PCI_MAX_DEV  31
#define PCI_MAX_FUNC 7

#define REG_REV      0x08
#define REG_CLASS    0x0B
#define REG_H_TYPE   0x0E

#define PCI_DEV(bus,dev,func) \
          (1 << 31)|\
          (bus << 16 )|(dev << 11)|(func <<8)
#define PCI_ADDR(bus,dev,func,reg)\
        PCI_DEV(bus,dev,func)|(reg&~3)

typedef unsigned char         u8;
typedef unsigned short        u16;
typedef unsigned int          u32;
typedef unsigned long long    u64;

struct _pci_dev{
  u8  bus, dev, func;
  u16 vendor_id, device_id;
  u8  rev_id;
  u8  class_code;
  u8  header_type;
  struct _pci_dev *next;
};
typedef struct _pci_dev pci_dev;

struct _pci_dev_header{
  char signature[4];
  int  node_counter;
  pci_dev *pdnode;
};
typedef struct _pci_dev_header pci_dev_header;

int
get_io_ctrl();

u8
pci_read_u8(u64 pci_addr, u8 reg);

u16
pci_read_u16(u64 pci_addr, u8 reg);

u32
pci_read_u32(u64 pci_addr, u8 reg);

void 
free_pci_dev(pci_dev_header *pdhdr);

pci_dev_header *
probe_pci();

#endif
