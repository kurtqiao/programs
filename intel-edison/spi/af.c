/*
 * Author: kurtqiao
 * Copyright (c) 2016
 *
 * GPLv3, free as freedom.
 */

#include "mraa.h"
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>

#define CMD_READ_JEDEC_ID  0x9F
#define CMD_READ_DATA      0x03

#define CS_PIN             8
mraa_gpio_context cs_pin = NULL;
mraa_spi_context spi;

typedef struct _action {
    uint16_t help:1;
    uint16_t probe:1;
    uint16_t erase:1;
    uint16_t read:1;
    uint16_t write:1;
    uint16_t error:1;
}ACTION;
//file name to save dump data
//max support 256 chars, but i didn't add check procedure
//so please use it with your own risk
char image_name[256]={0};

typedef struct _flashrom {
    const char *vendor;
    const char *model_name;
    uint8_t manufacturer_id;
    uint8_t type;
    uint8_t capacity;
    uint8_t device_id;
    uint32_t rom_size;
    uint32_t page_size;
}FLASHROM;

FLASHROM flash_chips[] = {

  { //do not remove dummy
    .vendor = "dummy",
    .model_name = "dummy",
    .manufacturer_id = 0x00,
    .type = 0x00,
    .capacity = 0x00,
    .device_id = 0x00
  },

  {
    .vendor = "WINBOND",
    .model_name = "W25Q80BV",
    .manufacturer_id = 0xEF,
    .type = 0x40,
    .capacity = 0x14,
    .device_id = 0x13,
    .rom_size = 1024,
    .page_size = 256
  },
  
  {0}
};


void
spi_stop();

int
spi_send_cmd(uint8_t);

uint8_t*
spi_read_data_buf(uint8_t *txdata, int length);

uint8_t
spi_read_data_byte();

int
probe_prameters(int, char **, ACTION *);

int
probe_chip(FLASHROM *, uint32_t);

uint32_t
read_jedec_id(mraa_spi_context , mraa_gpio_context);

int
dump_image(FLASHROM chip);

int
dump_image(FLASHROM chip);

int
print_usage(char *prog_name, FILE *channel, int ret_val)
{
    fprintf(channel, "\nusage: %s [-i | -r filename | -w filename]\n", prog_name);
    fprintf(channel, "Options:\n");
    fprintf(channel, "-i           identify flash rom\n");
    fprintf(channel, "-e           erase flash rom\n");
    fprintf(channel, "-r filename  dump flash rom\n");
    fprintf(channel, "-w filename  program flash rom\n");

    return ret_val;
}

/*
This is an intel edison arduino board base program,
dump/write SPI flash rom by use edison SPI bus.
1. you have to connect your spi flash pin as...
   https://flashrom.org/Serprog/Arduino_flasher
   please refer to part 3: The SPI Flash Chip
2. edison arduino board pin use as ...
   refer to upper link part 1: The Arduino
3. the program use MRAA lib, but there's some limitations
you may need to update edison kernel to fix...
   * read spi by mraa_spi_write_buf would return rubbish data, if 
   you try to read more data
   refer to https://github.com/intel-iot-devkit/mraa/issues/391 
   you have to update edison kernel to fix, or read one byte a time 

   * you have to set SPI bus on, rather then 'auto'
   but use Arduino API would not have this issue
   echo on >/sys/devices//pci0000\:00/0000\:00\:07.1/power/control
4. change your arduino board to 3.3v, by setting jumper 'J9'
   or you have to implement a voltage translation circuit.
*/
int 
main(int argc,char *argv[])
{
    int retv;
    int index=0;
    ACTION action={0};

/*
  prameters probe
*/
    retv = probe_prameters(argc, argv, &action);

    if (retv || action.error || optind < argc || argc == 1)
        return print_usage(basename(argv[0]), stderr, EXIT_FAILURE);
    if (action.help)
        return print_usage(basename(argv[0]), stderr, EXIT_SUCCESS);
/*
  mraa init
  mraa spi init
  chip select pin choose pin8(arduino board)
*/
    mraa_init();

    cs_pin = mraa_gpio_init(CS_PIN);
    spi = mraa_spi_init(1);
    if (spi == NULL){
        printf("init spi fail\n");
        exit(1);
    }
 
    mraa_gpio_dir(cs_pin, MRAA_GPIO_OUT);
    mraa_gpio_write(cs_pin, 1);

    mraa_spi_mode(spi, MRAA_SPI_MODE0);

//check support table
    index = probe_chip(flash_chips, read_jedec_id(spi, cs_pin));
    if (!index){
        printf("flash chip not support!\n");
    }
//identify flash chips
    if (action.probe && index){
        printf("chip support!\n");
        printf("%s  %s\n", flash_chips[index].vendor, flash_chips[index].model_name);
        printf("Manufature ID: 0x%.2X\nMemory Type ID: 0x%.2X\nCapacity ID: 0x%.2X\n", \
               flash_chips[index].manufacturer_id, flash_chips[index].type,flash_chips[index].capacity);
    }
//dump SPI flash
    if (action.read){
        dump_image(flash_chips[index]);
    }
/*
//read SFDP
        recdata = malloc(sizeof(uint8_t)*64);

        mraa_gpio_write(cs_pin, 0);
        //read SFDP command
        mraa_spi_write(spi, 0x5A);
        //24bit address
        mraa_spi_write(spi, 0x00);
        mraa_spi_write(spi, 0x00);
        mraa_spi_write(spi, 0x00);
        //dummy byte to DI
        mraa_spi_write(spi, 0x00);

        //recdata = mraa_spi_write_buf(spi, recdata, 16);
        usleep(2000);
        printf("read data back: \n"); 
        for (j=0; j<16; j++) {
        for (i=0; i<16; i++)
        {
           recdata[i] = 0;
        }
        recdata = mraa_spi_write_buf(spi, recdata, 16);
        usleep(2000);
        for (i=0; i<16; i++)
        {
           printf("0x%.2X ", recdata[i]);
        }
        printf("\n");
        }
        mraa_gpio_write(cs_pin, 1);
*/
    spi_stop();
	return MRAA_SUCCESS;
}

int
probe_prameters(int argc, char *argv[], ACTION *action)
{
    int option=0;

    while ( (option = getopt(argc, argv, ":hier:w:")) != -1){
        switch(option){
            case '?':
                fprintf(stderr, "error: unknow option: -%c\n", optopt);
                action->error = 1;
                break;
            case ':':
                fprintf(stderr, "error: missing argument for option: -%c\n", optopt);
                action->error = 1;
                break;
            case 'h':
                action->help = 1;
                break;
            case 'i':
                action->probe = 1;
                break;
            case 'e':
                action->erase = 1;
                break;
            case 'r':
                action->read = 1;
                sscanf(optarg, "%s", image_name);
                break;
            case 'w':
                action->write = 1;
                break;
            default: 
                return EXIT_FAILURE;
        }
    }
    return 0;
}

int
probe_chip(FLASHROM *flash_chips, uint32_t chip_id)
{
    int found_chip = 0;   //index
    FLASHROM *chips = NULL;
    for (chips=flash_chips;chips->vendor != 0; chips++,found_chip++)
    {
            //manufaturer_id + type_id <<8 + capacity_id <<16
            if (chip_id == (chips->manufacturer_id +\
                    (chips->type << 8)+\
                    (chips->capacity << 16))){
                    break;
            }
    }

    if (chips->vendor == 0)
            found_chip = 0;
    return found_chip;
}

uint32_t
read_jedec_id(mraa_spi_context spi, mraa_gpio_context cs_pin)
{
    uint8_t *rxdata=calloc(4, sizeof(uint8_t));
    //manufaturer_id + type_id <<8 + capacity_id <<16
    uint32_t rc_val;
    
    //drive cs low to start
    spi_send_cmd(CMD_READ_JEDEC_ID);
    //send 00 to read data back
    rxdata = spi_read_data_buf(rxdata, 3);
    
    usleep(100);
    rc_val = rxdata[0] + (rxdata[1]<<8) + (rxdata[2]<<16);
    //drive cs high to stop
    mraa_gpio_write(cs_pin, 1);  //do we need this line?

    free(rxdata);
    return rc_val;
}

int
spi_send_cmd(uint8_t command)
{
    //drive cs high, prevent command conflict 
    mraa_gpio_write(cs_pin, 1);
    //drive cs low to start
    mraa_gpio_write(cs_pin, 0);

    return mraa_spi_write(spi, command);
}

uint8_t
spi_read_data_byte()
{
    uint8_t retval;
    retval = (uint8_t) mraa_spi_write(spi, 0);
    return retval;

}

uint8_t*
spi_read_data_buf(uint8_t *txdata, int length)
{
//mraa_spi_write_buf is another API which could be used.
    uint8_t *data=calloc(length, sizeof(uint8_t));
//    data = mraa_spi_write_buf(spi, data, length);
    mraa_spi_transfer_buf(spi, data, txdata, 3);
//    memcpy(txdata, data, length);
    free(data);
    return txdata;

}

void
spi_stop()
{

    mraa_gpio_write(cs_pin, 1);
    mraa_spi_stop(spi);
}

int
dump_image(FLASHROM chip){

        int i, j;
        uint8_t *buf = calloc(chip.rom_size*1024, sizeof(uint8_t));
        FILE *image=NULL;
        
        //read data
        spi_send_cmd(CMD_READ_DATA);
        //send 24bit address, read from address 0
        mraa_spi_write(spi, 0x00);
        mraa_spi_write(spi, 0x00);
        mraa_spi_write(spi, 0x00);
        usleep(100);

        for (i = 0; i<chip.rom_size*1024; i++){
          buf[i]= mraa_spi_write(spi, 0x00);
          
          if (i%0x3000 == 0)
               fprintf(stderr, ".");
        }
        if (image_name[0] == 0)
            return 1;

        image = fopen(image_name, "wb");

        if (image == NULL)
            return 1;
        fwrite(buf, 1, chip.rom_size*1024, image);
        fclose(image);

        printf("dump BIOS ok!\n");
        return 0;
}
