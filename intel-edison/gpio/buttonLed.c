/*
 * Author: kurtqiao
 * Copyright (c) 2015
 *
 * GPLv3, free as freedom.
 */

#include "mraa.h"

#include <stdio.h>
#include <unistd.h>

/*
 * LED on/off by button test
 * set Pin7 as button (input), make pin7 GND as push button
 * set pin8 as LED (output), when push button LED light on
 *
 */

int main()
{
	mraa_result_t t_status;
	int t_button=0;

	mraa_platform_t platform = mraa_get_platform_type();

	mraa_gpio_context d_pin = NULL;
	mraa_gpio_context d_pout = NULL;

	fprintf(stdout, "The Intel Edison(FAB C) %d\n", platform);
	fprintf(stdout, "Try short Pin7 and Gnd to light on Pin8 LED\n");

	//gpio init pin7&pin8
	d_pin = mraa_gpio_init(7);
	d_pout = mraa_gpio_init(8);

    //internal pull-up pin7, or you will have some problem when read from it.
	mraa_gpio_mode(d_pin, MRAA_GPIO_PULLUP);

	//set pin7 as gpio input
	t_status = mraa_gpio_dir(d_pin, MRAA_GPIO_IN);
	//set pin8 as gpio output
	t_status = mraa_gpio_dir(d_pout, MRAA_GPIO_OUT);
	//not strict code here, just test
	if (t_status != MRAA_SUCCESS) {
		fprintf(stderr, "Can't set pin success!\n");
		return MRAA_ERROR_UNSPECIFIED;
	}
	//fprintf(stdout, "read from pin 7: %d\n", mraa_gpio_read(d_pin));

	// loop forever 
	for (;;) {
		t_button = mraa_gpio_read(d_pin);
		//Gnd short pin7 to light on LED
		mraa_gpio_write(d_pout, !t_button);
		//sleep(0.2);
	}

	return MRAA_SUCCESS;
}
