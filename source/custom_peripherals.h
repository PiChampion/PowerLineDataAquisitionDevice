

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "fsl_common.h"
#include "fsl_lptmr.h"
#include "fsl_adc16.h"
#include "fsl_gpio.h"
#include "fsl_port.h"
#include "fsl_cmp.h"
#include "fsl_pit.h"

#include "fsl_debug_console.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"


/***********************************************************************************************************************
 * Definitions
 **********************************************************************************************************************/

// Enable Print statements for debugging functionality
#define ENABLE_PRINTF 0 // value > 0 will allow print statements to print

/* Definition of peripheral ID */
#define LPTMR0_PERIPHERAL LPTMR0
/* Definition of the clock source frequency */
#define LPTMR0_CLK_FREQ 4000000UL
/* Definition of the prescaled clock source frequency */
#define LPTMR0_INPUT_FREQ 4000000UL
/* Definition of the timer period in us */
#define LPTMR0_USEC_COUNT 83UL
/* Definition of the timer period in number of ticks */
#define LPTMR0_TICKS 333UL
/* LPTMR0 interrupt vector ID (number). */
#define LPTMR0_IRQN LPTMR0_IRQn
/* LPTMR0 interrupt vector priority. */
#define LPTMR0_IRQ_PRIORITY 0
/* LPTMR0 interrupt handler identifier. */
#define LPTMR0_IRQHANDLER LPTMR0_IRQHandler

#define PIT_PERIPHERAL PIT
/* Definition of clock source. */
#define PIT_CLOCK_SOURCE kCLOCK_BusClk
/* Definition of clock source frequency. */
#define PIT_CLK_FREQ 60000000UL
/* Definition of ticks count for channel 0 - deprecated. */
#define PIT_0_TICKS 624U
/* PIT interrupt vector ID (number) - deprecated. */
#define PIT_0_IRQN PIT0_IRQn
/* PIT interrupt handler identifier - deprecated. */
#define PIT_0_IRQHANDLER PIT0_IRQHandler
/* Definition of channel number for channel 0. */
#define PIT_CHANNEL_0 kPIT_Chnl_0
/* Definition of ticks count for channel 0. */
#define PIT_CHANNEL_0_TICKS 624U
/* PIT interrupt vector ID (number). */
#define PIT_CHANNEL_0_IRQN PIT0_IRQn
/* PIT interrupt handler identifier. */
#define PIT_CHANNEL_0_IRQHANDLER PIT0_IRQHandler

/* Alias for GPIOA peripheral */
#define GPIOA_GPIO GPIOA
/* Alias for PORTA */
#define GPIOA_PORT PORTA
/* GPIOA interrupt vector ID (number). */
#define GPIOA_IRQN PORTA_IRQn
/* GPIOA interrupt vector priority. */
#define GPIOA_IRQ_PRIORITY 0
/* GPIOA interrupt handler identifier. */

#define SW3_IRQ PORTA_IRQHandler
/* Alias for GPIOD peripheral */

#define GPIOD_GPIO GPIOD
/* Alias for PORTD */
#define GPIOD_PORT PORTD
/* GPIOD interrupt vector ID (number). */
#define GPIOD_IRQN PORTD_IRQn
/* GPIOD interrupt handler identifier. */
#define Testing_Button_IRQ PORTD_IRQHandler
/* Alias for GPIOC peripheral */

#define GPIOC_GPIO GPIOC
/* Alias for PORTC */
#define GPIOC_PORT PORTC
/* GPIOC interrupt vector ID (number). */
#define GPIOC_IRQN PORTC_IRQn
/* GPIOC interrupt handler identifier. */
#define LED_Button_IRQ PORTC_IRQHandler

/***********************************************************************************************************************
 * Global variables
 **********************************************************************************************************************/
extern const lptmr_config_t LPTMR0_config;

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/

void Init_Custom_Peripherals(void);

