
/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "custom_peripherals.h"
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * NVIC initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'NVIC'
- type: 'nvic'
- mode: 'general'
- custom_name_enabled: 'false'
- type_id: 'nvic_57b5eef3774cc60acaede6f5b8bddc67'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'NVIC'
- config_sets:
  - nvic:
    - interrupt_table:
      - 0: []
      - 1: []
      - 2: []
      - 3: []
      - 4: []
      - 5: []
    - interrupts: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/* Empty initialization function (commented out)
static void NVIC_init(void) {
} */

/***********************************************************************************************************************
 * PIT initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'PIT'
- type: 'pit'
- mode: 'LPTMR_GENERAL'
- custom_name_enabled: 'false'
- type_id: 'pit_a4782ba5223c8a2527ba91aeb2bc4159'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'PIT'
- config_sets:
  - fsl_pit:
    - enableRunInDebug: 'false'
    - timingConfig:
      - clockSource: 'BusInterfaceClock'
      - clockSourceFreq: 'BOARD_BootClockRUN'
    - channels:
      - 0:
        - channel_id: 'CHANNEL_0'
        - channelNumber: '0'
        - enableChain: 'false'
        - timerPeriod: '96khz'
        - startTimer: 'true'
        - enableInterrupt: 'true'
        - interrupt:
          - IRQn: 'PIT0_IRQn'
          - enable_interrrupt: 'enabled'
          - enable_priority: 'false'
          - priority: '0'
          - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const pit_config_t PIT_config = {
  .enableRunInDebug = false
};

static void PIT_init(void) {
  /* Initialize the PIT. */
  PIT_Init(PIT_PERIPHERAL, &PIT_config);
  /* Set channel 0 period to 10.417 µs (625 ticks). */
  PIT_SetTimerPeriod(PIT_PERIPHERAL, PIT_CHANNEL_0, PIT_CHANNEL_0_TICKS);
  /* Enable interrupts from channel 0. */
  PIT_EnableInterrupts(PIT_PERIPHERAL, PIT_CHANNEL_0, kPIT_TimerInterruptEnable);
  /* Enable interrupt PIT_CHANNEL_0_IRQN request in the NVIC */
  EnableIRQ(PIT_CHANNEL_0_IRQN);
  /* Start channel 0. */
  PIT_StartTimer(PIT_PERIPHERAL, PIT_CHANNEL_0);
}
/***********************************************************************************************************************
 * LPTMR0 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'LPTMR0'
- type: 'lptmr'
- mode: 'LPTMR_GENERAL'
- custom_name_enabled: 'false'
- type_id: 'lptmr_48552e76e8733b28a9c768b6d8d4fefa'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'LPTMR0'
- config_sets:
  - fsl_lptmr:
    - lptmr_config:
      - timerMode: 'kLPTMR_TimerModeTimeCounter'
      - pinSelect: 'ALT.0'
      - pinPolarity: 'kLPTMR_PinPolarityActiveHigh'
      - enableFreeRunning: 'false'
      - bypassPrescaler: 'true'
      - prescalerClockSource: 'kLPTMR_PrescalerClock_0'
      - clockSource: 'BOARD_BootClockVLPR'
      - value: 'kLPTMR_Prescale_Glitch_0'
      - timerPeriod: '12khz'
    - enableInterrupt: 'true'
    - interrupt:
      - IRQn: 'LPTMR0_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'true'
      - priority: '0'
      - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */


const lptmr_config_t LPTMR0_config = {
  .timerMode = kLPTMR_TimerModeTimeCounter,
  .pinSelect = kLPTMR_PinSelectInput_0,
  .pinPolarity = kLPTMR_PinPolarityActiveHigh,
  .enableFreeRunning = false,
  .bypassPrescaler = true,
  .prescalerClockSource = kLPTMR_PrescalerClock_1,
  .value = kLPTMR_Prescale_Glitch_0
};

static void LPTMR0_init(void) {
  /* Initialize the LPTMR */
  LPTMR_Init(LPTMR0_PERIPHERAL, &LPTMR0_config);
  /* Set LPTMR period */
  LPTMR_SetTimerPeriod(LPTMR0, USEC_TO_COUNT(1000000U, CLOCK_GetFreq(kCLOCK_LpoClk)));
  /* Configure timer interrupt */
  LPTMR_EnableInterrupts(LPTMR0_PERIPHERAL, kLPTMR_TimerInterruptEnable);
  /* Interrupt vector LPTMR0_IRQn priority settings in the NVIC. */
  NVIC_SetPriority(LPTMR0_IRQN, LPTMR0_IRQ_PRIORITY);
  /* Enable interrupt LPTMR0_IRQn request in the NVIC. */
  EnableIRQ(LPTMR0_IRQN);
}

/***********************************************************************************************************************
 * GPIOA initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'GPIOA'
- type: 'gpio'
- mode: 'GPIO'
- custom_name_enabled: 'false'
- type_id: 'gpio_5920c5e026e8e974e6dc54fbd5e22ad7'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'GPIOA'
- config_sets:
  - fsl_gpio:
    - enable_irq: 'true'
    - port_interrupt:
      - IRQn: 'PORTA_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'true'
      - priority: '0'
      - enable_custom_name: 'true'
      - handler_custom_name: 'SW3_IRQ'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

static void GPIOA_init(void) {
  /* Make sure, the clock gate for port A is enabled (e. g. in pin_mux.c) */
  /* Interrupt vector PORTA_IRQn priority settings in the NVIC. */
  NVIC_SetPriority(GPIOA_IRQN, GPIOA_IRQ_PRIORITY);
  /* Enable interrupt PORTA_IRQn request in the NVIC. */
  EnableIRQ(GPIOA_IRQN);
}

/***********************************************************************************************************************
 * GPIOD initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'GPIOD'
- type: 'gpio'
- mode: 'GPIO'
- custom_name_enabled: 'false'
- type_id: 'gpio_5920c5e026e8e974e6dc54fbd5e22ad7'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'GPIOD'
- config_sets:
  - fsl_gpio:
    - enable_irq: 'true'
    - port_interrupt:
      - IRQn: 'PORTD_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'true'
      - handler_custom_name: 'Testing_Button_IRQ'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

static void GPIOD_init(void) {
  /* Make sure, the clock gate for port D is enabled (e. g. in pin_mux.c) */
  /* Enable interrupt PORTD_IRQn request in the NVIC. */
  EnableIRQ(GPIOD_IRQN);
}

/***********************************************************************************************************************
 * GPIOC initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'GPIOC'
- type: 'gpio'
- mode: 'GPIO'
- custom_name_enabled: 'false'
- type_id: 'gpio_5920c5e026e8e974e6dc54fbd5e22ad7'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'GPIOC'
- config_sets:
  - fsl_gpio:
    - enable_irq: 'true'
    - port_interrupt:
      - IRQn: 'PORTC_IRQn'
      - enable_interrrupt: 'enabled'
      - enable_priority: 'false'
      - priority: '0'
      - enable_custom_name: 'true'
      - handler_custom_name: 'LED_Button_IRQ'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

static void GPIOC_init(void) {
  /* Make sure, the clock gate for port C is enabled (e. g. in pin_mux.c) */
  /* Enable interrupt PORTC_IRQn request in the NVIC. */
  EnableIRQ(GPIOC_IRQN);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void Init_Custom_Peripherals(void)
{
  /* Initialize components */
	BOARD_InitPeripherals();
	PIT_init();
	LPTMR0_init();
	GPIOA_init();
	GPIOD_init();
	GPIOC_init();
}
