/*
 * buffers.c
 *
 *  Created on: Apr 4, 2021
 *      Author: IanHellmer
 */

#define buffer_a false
#define buffer_b true

#include "buffers.h"
#include "custom_peripherals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MK64F12.h"
#include "stdbool.h"
#include "peripherals.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
volatile uint16_t buffer_channel[8][2][2000] = {0};
volatile bool buffer_channel_using = buffer_a;
volatile uint16_t buffer_channel_position[8] = {0};
volatile bool buffer_channel_write = buffer_a;
volatile bool buffers_full = 0;
volatile unsigned int not_transferred = 0;


void PlaceInBuffer(uint16_t value, volatile unsigned int channel) {
	buffer_channel[channel][buffer_channel_using][buffer_channel_position[channel]] = value;

	buffer_channel_position[channel]++;
	if((buffer_channel_position[channel] >= 2000) && (channel == 7)) {		// Check to see if all the channels have been filled
		if(buffer_channel_using == buffer_a) {								// Switch the buffer being used to store ADC values
			buffer_channel_using = buffer_b;								// 		and the buffer used to write to the SD Card
			buffer_channel_write = buffer_a;
		}
		else if(buffer_channel_using == buffer_b) {							// Switch the buffer being used to store ADC values
			buffer_channel_using = buffer_a;								// 		and the buffer used to write to the SD Card
			buffer_channel_write = buffer_b;
		}
		for(int i = 0; i < 8; i++)				// Reset the positions of all the buffers
		{
			buffer_channel_position[i] = 0;
		}

		if(buffers_full == true) {				// Check to see if the buffer has been emptied since the last time buffers_full was made true
			#if ENABLE_PRINTF
			PRINTF("\r\nBuffer not emptied in time\r\n");
			#endif
			not_transferred++;
		}
		else {
			buffers_full = true;				// Make buffers_full true to signal to main the transfer the data in the buffers to the SD card
		}
	}
}




