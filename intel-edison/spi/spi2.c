/*
 * Author: kurtqiao
 * Copyright (c) 2016
 *
 * GPLv3, free as freedom.
 */

#include "mraa.h"
#include <stdint.h>
#include <unistd.h>

int main()
{
        uint8_t recv=0;
        uint8_t rec1[5]={0};
        uint8_t *recdata=rec1;
        uint8_t data[]={0x9f};

        mraa_gpio_context cs_pin = NULL;

        int spifreq=1000000;
        uint8_t spicmd=0xAB;
        int i = 0;
        
        cs_pin = mraa_gpio_init(8);
        mraa_gpio_dir(cs_pin, MRAA_GPIO_OUT);
        mraa_gpio_write(cs_pin, 1);

        mraa_spi_context spi;
        spi = mraa_spi_init(0);
        if (spi == NULL){
           printf("init spi fail\n");
           exit(1);
        }

        mraa_spi_mode(spi, MRAA_SPI_MODE0);
//        mraa_spi_frequency(spi, spifreq);

        mraa_gpio_write(cs_pin, 0);

        mraa_spi_write_buf(spi, data, 1);
        recdata = mraa_spi_write_buf(spi, recdata, 5);
        usleep(2000);
        printf("read back: 0x%X, 0x%X, 0x%X, 0x%X, 0x%X\n", recdata[0], \
               recdata[1], recdata[2], recdata[3], recdata[4]);

        mraa_gpio_write(cs_pin, 1);
        usleep(2000);
        mraa_spi_stop(spi);

	return MRAA_SUCCESS;
}
