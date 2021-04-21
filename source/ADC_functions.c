/*
 * ADC_functions.c
 *
 *  Created on: Apr 5, 2021
 *      Author: ianpi
 */

#include "ADC_functions.h"
#include "SDcard_functions.h"
#include "buffers.h"
#include "custom_peripherals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MK64F12.h"
#include "stdbool.h"
#include "peripherals.h"

extern volatile unsigned int PITcounter;
extern volatile bool ADCvalueTaken;
extern volatile bool current_ch_enabled;
volatile bool in_cycle_mode = false;
volatile bool in_single_mode = true;
extern volatile bool ready_flag;
extern volatile bool g_blink;
extern bool need_to_make_new_directory;

extern volatile bool buffers_full;
extern volatile uint16_t buffer_channel[8][2][2000];
extern volatile bool buffer_channel_using;
extern volatile bool buffer_channel_write;
extern volatile uint16_t buffer_channel_position[8];

extern bool setup_complete;
extern volatile int counter;
volatile int multiple_cycles = 0;
bool restart_blink = false;

extern volatile bool Testing_Button_Pressed;
volatile int high_readings = 0;
volatile int disRegard_value = 0;
#define MAXDISREGARD 600000

/* ADC0_IRQn interrupt handler */
void ADC0_IRQHANDLER(void) {
  uint16_t result_value = 0;
  result_value = ADC16_GetChannelConversionValue(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP);
  ADCvalueTaken = true;

  if(setup_complete){
  		// initially only the current channel we need to monitor is set, meaning we are in single mode
  		// if we are in single mode, check to see if the result_value is >= 34000
  		if(disRegard_value++ > MAXDISREGARD){
  			if(in_single_mode) {
  				ready_flag = true;
  				if(result_value > 34000) { // start process
  					high_readings++;

  					if(high_readings > 100) {
  						Testing_Button_Pressed = true; // switch to cycle mode, cycle through  all the channels
  						in_cycle_mode = true; // enable cycle mode check, check for signal going below 34k
  						in_single_mode = false;
  						g_blink = false; // renable for second cycle somewhere...TO DO
  						counter = 0;
  						GPIO_PortClear(GPIOB, 1u << 10U); // off green LED
  						GPIO_PortSet(GPIOB, 1u << 9U); // on yellow LED
  					}
  				}
  				else{
  					high_readings = 0;
  				}
  			}

  			// if we are in cycle mode and the value is from channel 1, the current channel we monitor
  			// check to see if it has gone low
  			if(in_cycle_mode){
  				PlaceInBuffer(result_value, PITcounter);
  				if(current_ch_enabled) {
					if(result_value < 34000){
						Testing_Button_Pressed = false;	// if it has gone low, stop the cycle process
						in_cycle_mode = false;
						in_single_mode = true;
						GPIO_PortSet(GPIOB, 1u << 10U); // on green LED
						GPIO_PortClear(GPIOB, 1u << 9U); // off yellow LED
						reinitializeSystem();
						need_to_make_new_directory = true;
						g_blink = true;

					}
					current_ch_enabled = false; // after ch.1, another channel will be set
  				}
  			}
  		}
  	}


  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}

void reinitializeSystem(void)
{
	buffers_full = 0;
	PITcounter = -1;
	buffer_channel_using = buffer_a;
	for(int i = 0; i < 8; i++) {
		buffer_channel_position[i] = 0;
	}
	buffer_channel_write = buffer_a;
}

