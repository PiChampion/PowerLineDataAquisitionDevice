#include "custom_peripherals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MK64F12.h"
#include "stdbool.h"
#include "peripherals.h"



extern volatile uint32_t lptmrCounter;

/* Whether the SW button is pressed */
extern volatile bool g_ButtonPress;
extern volatile bool LED_Button_Pressed;
extern volatile bool Testing_Button_Pressed;
extern volatile bool PIT_flag;
extern volatile bool get_ADC_values;

// debounce code
extern volatile bool LED_Button_Debounce;
extern volatile int LED_Button_Debounce_counter;

extern volatile bool Testing_Button_Debounce;
extern volatile int Testing_Button_Debounce_counter;
extern volatile bool ready_flag;
extern volatile int new_flag;
extern volatile int counter;

volatile unsigned int PITcounter = -1;
volatile bool ADCvalueTaken = true;
volatile unsigned int bad = 0;
extern int timer_for_battery_check;
extern int batt_check;
int USB_Switching_Counter = 0;
extern volatile bool usb_control;
volatile bool g_blink = true;
extern bool restart_blink;

// once in cycle, will check in ADC interrupt to see if it has gone below threshold
// set to true in PIT ch. 1 enabling
volatile bool current_ch_enabled = false;
extern volatile bool in_single_mode;

/*******************************************************************************
 * PIT IRQ
 ******************************************************************************/
/* PIT0_IRQn interrupt handler */
void PIT_CHANNEL_0_IRQHANDLER(void) {
  uint32_t intStatus;
  /* Reading all interrupt flags of status register */
  intStatus = PIT_GetStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0);
  PIT_ClearStatusFlags(PIT_PERIPHERAL, PIT_CHANNEL_0, intStatus);

//-------------Debounce---------------------------------------------------------------------------


  if(LED_Button_Debounce) {
	  LED_Button_Debounce_counter++;
  }

  if (LED_Button_Debounce_counter >= 18000) {
	  //enable irq here
	  EnableIRQ(GPIOC_IRQN);
	  LED_Button_Debounce_counter = 0;
	  LED_Button_Debounce = false;
  }

  if(Testing_Button_Debounce) {
	  Testing_Button_Debounce_counter++;
  }

  if (Testing_Button_Debounce_counter >= 20000) {
	  //enable irq here
	  EnableIRQ(GPIOD_IRQN);
	  Testing_Button_Debounce_counter = 0;
	  Testing_Button_Debounce = false;
  }

  if(LED_Button_Pressed && !LED_Button_Debounce){
	  USB_Switching_Counter++;
	  LED_Button_Pressed = false;
  }

  if(ready_flag && g_blink) {
	  if( counter++ >= 55000) {
		  GPIO_PortToggle(GPIOB, 1u << 10U); 	// green LED;
		  counter = 0;
	  }
  }




//-------------------------------------------------------------------------------------

  if(batt_check) {
  	  timer_for_battery_check++;
  }

  if(timer_for_battery_check >= 144000 ) { // 1.5 seconds
  	  GPIO_PortClear(GPIOB, 1u << 23U); // off red LED
  	  batt_check = 0;
  	  USB_Switching_Counter = 0;
  	  timer_for_battery_check = 0;
  }

  if(USB_Switching_Counter >= 2) {
	  usb_control = !usb_control;
	  USB_Switching_Counter = 0;
	  if(usb_control) { // usb mode y/g on
		  GPIO_PortSet(GPIOB, 1u << 9U); // on yellow LED
		  GPIO_PortSet(GPIOB, 1u << 10U); // on green LED
		  GPIO_PortClear(GPIOB, 1u << 23U); // off red LED
		  g_blink = false;
	  } else { // sd card mode y/g off
		  GPIO_PortClear(GPIOB, 1u << 23U); // off red LED
		  GPIO_PortClear(GPIOB, 1u << 9U); // off yellow LED
		  GPIO_PortClear(GPIOB, 1u << 10U); // off green LED
		  g_blink = true;
	  }
  }

//-------------------------------------------------------------------------------------

  if(in_single_mode){
	  if(ADCvalueTaken) {
  		 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[0]);
  		ADCvalueTaken = false;
	  }
  }

  if(Testing_Button_Pressed) {
	  if(ADCvalueTaken == false){

		  bad++;
		#if ENABLE_PRINTF
	  	  PRINTF("\r\n ADC value was not taken in time \r\n");
		#endif

	  }

	  else {

		  PITcounter++;			// Move to next channel
		  if(PITcounter > 7) PITcounter = 0;	// When done with channel 8, loop around

		  switch(PITcounter) {
			 case 0:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[0]);
				 current_ch_enabled = true; // set high, check this value in ADC IRQ for below threshold, to end process
				break;
			 case 1:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[1]);
				break;
			 case 2:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[2]);
				break;
			 case 3:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[3]);
				break;
			 case 4:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[4]);
				break;
			 case 5:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[5]);
				break;
			 case 6:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[6]);
				break;
			 case 7:
				 ADC16_SetChannelConfig(ADC0_PERIPHERAL, ADC0_CH0_CONTROL_GROUP, &ADC0_channelsConfig[7]);
				break;
			 default :

				 #if ENABLE_PRINTF
				 PRINTF("\r\nPIT channel selection error\r\n");
				 #endif
				 ; // end of default
		  }
		  ADCvalueTaken = false;
	  }
  }
}

/*******************************************************************************
 * LPTMR0 IRQ
 ******************************************************************************/
/* LPTMR0_IRQn interrupt handler */
void LPTMR0_IRQHANDLER(void) {
	LPTMR_ClearStatusFlags(LPTMR0, kLPTMR_TimerCompareFlag);
	lptmrCounter++;


  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}
