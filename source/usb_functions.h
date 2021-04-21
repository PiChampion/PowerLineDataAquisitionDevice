/*
 * usb_functions.h
 *
 *  Created on: Apr 3, 2021
 *      Author: IanHellmer
 */

#ifndef USB_FUNCTIONS_H_
#define USB_FUNCTIONS_H_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/*! @brief The maximum timeout time for the transfer complete event */
#define EVENT_TIMEOUT_TRANSFER_COMPLETE (1000U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void BOARD_InitHardware(void);
void USB_DeviceClockInit(void);
void USB_DeviceIsrEnable(void);
#if USB_DEVICE_CONFIG_USE_TASK
void USB_DeviceTaskFn(void *deviceHandle);
#endif
void USB_DeviceApplicationInit(void);
void USB_DeviceMscAppTask(void);

#endif /* USB_FUNCTIONS_H_ */
