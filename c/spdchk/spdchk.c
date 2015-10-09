/*
A DOS test program that assume a condition that read SPD fail probabilistic.
1. collect SPD data and save in dimm[4]
2. loop to read spd and compare with dimm[4]

build pass with Turbo C++ 4.0(DOSBOX) Windows 7/8 64Bit Version.
*/


#include <stdlib.h>
#include <stdio.h>
#include <conio.h>

/*added for Warning "Code has no effect in function" pointed to outp()*/
#undef  outp

#define MAX_SPD_WIDTH 	0x80
#define MAX_DIMM 		4


typedef unsigned char int8_t;

typedef struct _dimm_data
{
  int     present;
  int8_t  ddr_type;
  int8_t  spd_data[MAX_SPD_WIDTH];
}DIMM_DATA;

void
delay_t();

int
copy_dimm_spds(const int, int, DIMM_DATA *);

int
read_spd_byte(const int, int,int, int8_t *);

int
compare_spd(int8_t *, int8_t *);

int
verify_dimm_spds(const int, int, DIMM_DATA *);

void
print_buffer(int8_t *, int);

/*
[USAGE]:
spdchk SMBUS_MMIO LOOP_COUNTER
SMBUS_MMIO: check the smbus base address from smbus pci contorller, offset 0x20
LOOP_COUNTER: input loop count number
*/
int
main(
  int argc, char const *argv[])
{
	static int SMBUS_MMIO=0xF040;
	int counter=100;
	int index, tmp=0;
	DIMM_DATA dimm[4]={0};

	int verify_status = 0;

	if (argc >= 2){
		sscanf(argv[1], "%x", &SMBUS_MMIO);
	}
	//
	if (argc >= 3){
		sscanf(argv[2], "%d", &counter);
	}

	copy_dimm_spds(SMBUS_MMIO, MAX_DIMM, dimm);

    printf("SMBUS BAR:%x\n", SMBUS_MMIO);
    //loop xx times to verify dimm spds
    while(counter--) {

    	verify_status = verify_dimm_spds(SMBUS_MMIO, MAX_DIMM, dimm);
    	delay_t();

    	if (verify_status)
   		break;
   		tmp++;
	};
	if (!verify_status)
		printf("\nverify %d loop all pass!\n", tmp);

	return 0;
}


/*
------------------------------------------------------
------------------------------------------------------
*/
int
verify_dimm_spds(const int smbus_bar, int max_dimm, DIMM_DATA *dimm)
{
	DIMM_DATA  tmpdimm;
	int8_t ddrtype;
	int cr_socket_addr;
	int cr_socket, index;
	int sts;
	int ret_status = 0;

	//read dimm data from 0xA0 to 0xA6
	cr_socket_addr = 0xA0;
    for ( cr_socket=0; cr_socket < max_dimm; cr_socket++, cr_socket_addr += 2){

    	sts = read_spd_byte(smbus_bar, cr_socket_addr, 2, &ddrtype);

    	if (sts)
    	{
    		tmpdimm.present = 0;
    		if (dimm[cr_socket].present != 0){
    		printf("verify fail! dimm %d lost!\n", cr_socket);
    	    ret_status = 1;
    	    //first version use break, detect fail then stop test
    		break;
    	    }
    	    continue;
    	}

    	for (index=0; index <= MAX_SPD_WIDTH; index++){
    		read_spd_byte(smbus_bar, cr_socket_addr, index, \
    			&tmpdimm.spd_data[index]);
    	}

    	tmpdimm.present = 1;
    	tmpdimm.ddr_type = ddrtype;

    	//print out verify data
    	gotoxy(1, 5);
    	printf("verify dimm %x at %x\n", cr_socket, cr_socket_addr);
    	print_buffer(tmpdimm.spd_data, MAX_SPD_WIDTH);
    	//verify dimmx, compare every byte of spd
    	sts = compare_spd(tmpdimm.spd_data, dimm[cr_socket].spd_data);
    	if (sts){
    		printf("verify fail! dimm %d spd changed!\n", cr_socket);
    		ret_status = 1;
    		//first version use break, detect fail then stop test
    		break;
    	}

    }

    return ret_status;

}

int
compare_spd(int8_t *spd1, int8_t *spd2)
{
	int i;
	int cmp_rst=0;

	for (i=0; i<MAX_SPD_WIDTH; i++){
		if (spd1[i] != spd2[i]){
			cmp_rst = 1;
			break;
		}
	}
	return cmp_rst;

}

int
copy_dimm_spds(const int smbus_bar, int max_dimm, DIMM_DATA *dimm)
{
	int8_t ddrtype;
	int cr_socket_addr;
	int cr_socket, index;
	int sts;
	//read dimm data from 0xA0 to 0xA6
	cr_socket_addr = 0xA0;
    for ( cr_socket=0; cr_socket < max_dimm; cr_socket++, cr_socket_addr += 2){

    	sts = read_spd_byte(smbus_bar, cr_socket_addr, 2, &ddrtype);

    	if (sts)
    	{
    		dimm[cr_socket].present = 0;
    		continue;
    	}
    	for (index=0; index <= MAX_SPD_WIDTH; index++){
    		read_spd_byte(smbus_bar, cr_socket_addr, index, \
    			&dimm[cr_socket].spd_data[index]);
    	}
    	dimm[cr_socket].present = 1;
    	dimm[cr_socket].ddr_type = ddrtype;
    }
    return sts;
}

void
print_buffer(int8_t *buffer, int len){
	int i;
	for (i=0; i<len; i++){
		if ((i%16)==0){
			if(i !=0)
			printf("\n");
		}
	printf("%.2X ", buffer[i]);
	}
	printf("\n");
}

void
delay_t(){
	int tick=500;
	//int i;
	//for (i=3; i--;)
    while(tick--){
    	inp(0x70);
    }
}


int
read_spd_byte(
	const int	smbus_bar,
	int	dimm_addr,
	int	byte_offset,
	int8_t	*data
	)
{
	int	smbsts=0;

	do {
		outp(smbus_bar, 0x40);
		smbsts = (int) inp(smbus_bar);
		if (smbsts == 0xff){
			break;
		}
		outp(smbus_bar,0x1E);
		outp(smbus_bar+3,byte_offset);
		outp(smbus_bar+4, dimm_addr | 1);
		outp(smbus_bar+2,0x48);
		smbsts = (int) inp(smbus_bar);
		while ((smbsts & 0x1E) == 0) {
			smbsts = (int) inp(smbus_bar);
		};
	}while ((smbsts & 0x08) != 0);

	if ((smbsts & 0x1C) != 0) {
		outp(smbus_bar, 0x5E);
		return 1;
	}

	*data = inp(smbus_bar+5);
    //delay_t();
	return 0;
}
