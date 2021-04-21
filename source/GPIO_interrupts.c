/*
 * GPIO_interrupts.c
 *
 *  Created on: Apr 3, 2021
 *      Author: IanHellmer
 */
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

/* Whether the SW button is pressed */
volatile bool g_ButtonPress = false;
volatile bool LED_Button_Pressed = false;
volatile bool Testing_Button_Pressed = false;
volatile bool PIT_flag = false;
volatile bool get_ADC_values = false;
volatile bool usb_control = false;

// debounce code
volatile bool LED_Button_Debounce = false;
volatile int LED_Button_Debounce_counter = 0;

volatile bool Testing_Button_Debounce = false;
volatile int Testing_Button_Debounce_counter = 0;
volatile bool ready_flag = false;
volatile int new_flag = 0;
int counter = 0;
extern bool setup_complete;
extern unsigned int current_cycle;

int timer_for_battery_check = 0;
int batt_check = 0;

/*******************************************************************************
 * Button IRQs
 ******************************************************************************/
/* PORTC_IRQn interrupt handler */
void LED_Button_IRQ(void) {
  /* Get pin flags */
  uint32_t pin_flags = GPIO_PortGetInterruptFlags(GPIOC);

  /* Place your interrupt code here */
  LED_Button_Pressed = true;
  LED_Button_Debounce = true;


  if (kCMP_OutputAssertEventFlag == (kCMP_OutputAssertEventFlag & CMP_GetStatusFlags(CMP0))) {
	  GPIO_PortClear(GPIOB, 1u << 23U); // off red LED
  }
  else {
      GPIO_PortSet(GPIOB, 1u << 23U); // on red LED
      batt_check = 1;
  }

  /* Clear pin flags */
  GPIO_PortClearInterruptFlags(GPIOC, pin_flags);

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}
//====================================================================================

/* PORTD_IRQn interrupt handler */
void Testing_Button_IRQ(void) {
  /* Get pin flags */
  //uint32_t pin_flags= GPIO_PortGetInterruptFlags(GPIOD);

  /* Place your interrupt code here */
  Testing_Button_Debounce = true;

  /* Clear pin flags */
  GPIO_PortClearInterruptFlags(GPIOD, 3);
  //GPIO_PortClearInterruptFlags(GPIOD, pin_flags);

  /* Add for ARM errata 838869, affects Cortex-M4, Cortex-M4F
     Store immediate overlapping exception return operation might vector to incorrect interrupt. */
  #if defined __CORTEX_M && (__CORTEX_M == 4U)
    __DSB();
  #endif
}
