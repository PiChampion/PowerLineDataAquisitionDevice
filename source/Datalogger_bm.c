/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <string.h>
#include "fsl_sd.h"
#include "fsl_debug_console.h"
#include "ff.h"
#include "diskio.h"
#include "fsl_sd_disk.h"
#include "pin_mux.h"
#include "clock_config.h"
#include "board.h"
#include "sdmmc_config.h"
#include "fsl_sysmpu.h"
#include "fsl_cmp.h"
#include "fsl_lptmr.h"

#include "fsl_common.h"
#include "fsl_adc16.h"
#include "fsl_gpio.h"
#include "fsl_port.h"

#include <stdlib.h>
/*${standard_header_anchor}*/
#include "usb_device_config.h"
#include "usb.h"
#include "usb_device.h"

#include "usb_device_class.h"
#include "usb_device_msc.h"
#include "usb_device_ch9.h"
#include "usb_device_descriptor.h"
#include "disk.h"
#include "buffers.h"
#include "usb_functions.h"
#include "SDcard_functions.h"

#include "usb_disk_adapter.h"
#include "fsl_device_registers.h"
#include "custom_peripherals.h"

#if (defined(FSL_FEATURE_SOC_SYSMPU_COUNT) && (FSL_FEATURE_SOC_SYSMPU_COUNT > 0U))
#include "fsl_sysmpu.h"
#endif /* FSL_FEATURE_SOC_SYSMPU_COUNT */

#if ((defined FSL_FEATURE_SOC_USBPHY_COUNT) && (FSL_FEATURE_SOC_USBPHY_COUNT > 0U))
#include "usb_phy.h"
#endif
/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* buffer size (in byte) for read/write operations */
#define BUFFER_SIZE (100U)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/


/*******************************************************************************
 * Variables
 ******************************************************************************/
FIL g_fileObject;   /* File object */
bool usb_initialize = true;
bool SD_initialize = true;
bool need_to_make_new_directory = true;
bool first_time_initialize = true;
char directory_name[] = "/CYCLE_1";
extern volatile bool usb_control;
extern volatile bool buffers_full;
extern char file_name;

/* @brief decription about the read/write buffer
 * The size of the read/write buffer should be a multiple of 512, since SDHC/SDXC card uses 512-byte fixed
 * block length and this driver example is enabled with a SDHC/SDXC card.If you are using a SDSC card, you
 * can define the block length by yourself if the card supports partial access.
 * The address of the read/write buffer should align to the specific DMA data buffer address align value if
 * DMA transfer is used, otherwise the buffer address is not important.
 * At the same time buffer address/size should be aligned to the cache line size if cache is supported.
 */
/*! @brief Data written to the card */
SDK_ALIGN(uint8_t g_bufferWrite[BUFFER_SIZE], BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);
/*! @brief Data read from the card */
SDK_ALIGN(uint8_t g_bufferRead[BUFFER_SIZE], BOARD_SDMMC_DATA_BUFFER_ALIGN_SIZE);

//CMP & DAC
#define CMP_USER_CHANNEL 1U /* PTC7, CMP0_IN1, J1-13 on FRDM-K64F. */
#define CMP_DAC_CHANNEL  7U

volatile uint32_t lptmrCounter = 0U;
uint32_t currentCounter = 0U;

/*******************************************************************************
 * Code
 ******************************************************************************/

/*!
 * @brief Main function
 */
int main(void)
{
    BOARD_InitPins();
    BOARD_BootClockRUN();
    BOARD_InitDebugConsole();
    Init_Custom_Peripherals();
    SYSMPU_Enable(SYSMPU, false);

    //-------------DAC & CMP setup----------------------------------------------------------------
     cmp_config_t mCmpConfigStruct;
     cmp_dac_config_t mCmpDacConfigStruct;

     CMP_GetDefaultConfig(&mCmpConfigStruct);
     /* Init the CMP comparator. */
     CMP_Init(CMP0, &mCmpConfigStruct);

     /* Configure the DAC channel. */
     mCmpDacConfigStruct.referenceVoltageSource = kCMP_VrefSourceVin2; /* VCC. */
     mCmpDacConfigStruct.DACValue = 32U; /* Half voltage of logic high level. */
     CMP_SetDACConfig(CMP0, &mCmpDacConfigStruct);
     CMP_SetInputChannels(CMP0, CMP_USER_CHANNEL, CMP_DAC_CHANNEL);
     //--------------------------------------------------------------------------------------------

    PRINTF("\r\nGroup 14: Design Day\r\n");
    FRESULT error;

    while (true)
    {
    	if(usb_control)
    	{
    		if(usb_initialize)
			{
    			BOARD_USB_Disk_Config(USB_DEVICE_INTERRUPT_PRIORITY);
    			USB_DeviceApplicationInit();
    			usb_initialize = 0;
    			SD_initialize = 1;
			}
    	USB_DeviceMscAppTask();
    	}
    	else {
    		if(SD_initialize)
    			//USB_DeviceDeinit
				//USB_DeviceStop CONTROLLER_ID
    			//USB_DeviceMscDeinit
    			//USB_DeviceClassDeinit(CONTROLLER_ID);
    		{
    			if(first_time_initialize)
    			{
					if (initializeSDcard() != kStatus_Success)
					{
						return -1;
					}
					first_time_initialize = false;
    			}
    			else
    			{
    				USB_DeviceClassDeinit(CONTROLLER_ID);
    				if (initializeSDcardNoMKFS() != kStatus_Success)
    				{
    					return -1;
    				}
    				error = f_open(&g_fileObject, _T("/CYCLE_1/CH_1.txt"), (FA_WRITE));
    				if (error)
    				{
    					PRINTF("Open file failed.\r\n");
    					return kStatus_Fail;
    				}
    				if (f_close(&g_fileObject))
    				{
    					PRINTF("\r\nClose file failed.\r\n");
    					return kStatus_Fail;
    				}
    				PRINTF("We Back.\r\n");

    			}
    			SD_initialize = 0;
    			usb_initialize = 1;
    		}
    		if(need_to_make_new_directory)
    		{
    			if (makeDirectory() != kStatus_Success)
    			{
    				return -1;
    			}
    			need_to_make_new_directory = false;
    		}
    		if(buffers_full)
    		{
    			transferBuffersToSD();
    		}
    	}
    }
}


