/*
 * SDcard_functions.c
 *
 *  Created on: Apr 4, 2021
 *      Author: IanHellmer
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
#include "SDcard_functions.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
extern FIL g_fileObject;
FATFS g_fileSystem; /* File system object */
char dir_name[] = "/CYCLE_1";
unsigned int dir_number = 1;
char file_name[] = "/CYCLE_1/CH_1.txt";
bool setup_complete = false;
unsigned int current_cycle = 0;
extern volatile bool buffers_full;
extern volatile uint16_t buffer_channel[8][2][2000];
extern volatile bool buffer_channel_using;
extern volatile bool buffer_channel_write;

status_t sdcardWaitCardInsert(void)
{
    BOARD_SD_Config(&g_sd, NULL, BOARD_SDMMC_SD_HOST_IRQ_PRIORITY, NULL);

    /* SD host init function */
    if (SD_HostInit(&g_sd) != kStatus_Success)
    {
        PRINTF("\r\nSD host init fail\r\n");
        return kStatus_Fail;
    }

    /* wait card insert */
    if (SD_PollingCardInsert(&g_sd, kSD_Inserted) == kStatus_Success)
    {
        PRINTF("\r\nCard detected.\r\n");
        /* power off card */
        SD_SetCardPower(&g_sd, false);
        /* power on the card */
        SD_SetCardPower(&g_sd, true);
    }
    else
    {
        PRINTF("\r\nCard detect fail.\r\n");
        return kStatus_Fail;
    }

    return kStatus_Success;
}

status_t initializeSDcard(void)
{
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
    BYTE work[FF_MAX_SS];
    FRESULT error;

    PRINTF("\r\nPlease insert a micro SD card.\r\n");

    if (sdcardWaitCardInsert() != kStatus_Success)
    {
        return kStatus_Fail;
    }

    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
    {
        PRINTF("Mount volume failed.\r\n");
        return kStatus_Fail;
    }

	#if (FF_FS_RPATH >= 2U)
    	error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    	if (error)
    	{
    		PRINTF("Change drive failed.\r\n");
    		return kStatus_Fail;
    	}
	#endif

	#if FF_USE_MKFS
		PRINTF("\r\nMaking file system......This can take a while, depending on the storage size of the card.\r\n");
		if (f_mkfs(driverNumberBuffer, 0, work, sizeof work))
		{
			PRINTF("Make file system failed.\r\n");
			return kStatus_Fail;
		}
	#endif /* FF_USE_MKFS */

	PRINTF("\r\nSetup Complete.\r\n");
	setup_complete = true;
	return kStatus_Success;
}

status_t initializeSDcardNoMKFS(void)
{
	const TCHAR driverNumberBuffer[3U] = {SDDISK + '0', ':', '/'};
    FRESULT error;

    PRINTF("\r\nPlease insert a micro SD card.\r\n");

    if (sdcardWaitCardInsert() != kStatus_Success)
    {
        return kStatus_Fail;
    }

    if (f_mount(&g_fileSystem, driverNumberBuffer, 0U))
    {
        PRINTF("Mount volume failed.\r\n");
        return kStatus_Fail;
    }

	#if (FF_FS_RPATH >= 2U)
    	error = f_chdrive((char const *)&driverNumberBuffer[0U]);
    	if (error)
    	{
    		PRINTF("Change drive failed.\r\n");
    		return kStatus_Fail;
    	}
	#endif

	PRINTF("\r\nSetup Complete.\r\n");
	setup_complete = true;
	return kStatus_Success;
}

status_t makeDirectory(void)
{
	FRESULT error;

	dir_name[7] = '0' + dir_number;
	file_name[7] = '0' + dir_number;
	dir_number++;
	error = f_mkdir(_T(dir_name));
	if (error)
	{
		if (error == FR_EXIST)
		{
			PRINTF("Directory exists.\r\n");
		}
		return kStatus_Fail;
	}
	for(int i = 0; i < 8; i++)
	{
		file_name[12] = '1' + i;
		error = f_open(&g_fileObject, _T(file_name), (FA_CREATE_NEW));
		if (error)
		{
			if (error == FR_EXIST)
			{
				PRINTF("File exists.\r\n");
			}
			else
			{
				PRINTF("Open file failed.\r\n");
				return kStatus_Fail;
			}
		}
		if (f_close(&g_fileObject))
		{
			PRINTF("\r\nClose file failed.\r\n");
			return kStatus_Fail;
		}
	}
	return kStatus_Success;
}

void transferBuffersToSD(void)
{
	PRINTF("\r\nTransferring Buffers to SD Card\r\n");
    for(int i = 0; i < 8; i++)
    {
    	transferIndividualChannel(i);
    }
    PRINTF("\r\nBuffers Transferred\r\n");
    buffers_full = 0;
}

void transferIndividualChannel(unsigned int channel_number)
{

	FRESULT error;
    UINT bytesWritten;

	file_name[12] = '1' + channel_number;

	error = f_open(&g_fileObject, _T(file_name), FA_WRITE);
	if (error)
	{
		if (error == FR_EXIST)
		{
			PRINTF("File exists.\r\n");
		}
		/* if file not exist, create a new file */
		else if (error == FR_NO_FILE)
		{
			if (f_open(&g_fileObject, _T(file_name), (FA_WRITE | FA_CREATE_NEW)) != FR_OK)
			{
				PRINTF("Create file failed.\r\n");
				return;
			}
		}
		else
		{
			PRINTF("Open file failed.\r\n");
			return;
		}
	}

	/* write append */
	if (f_lseek(&g_fileObject, g_fileObject.obj.objsize) != FR_OK)
	{
		PRINTF("lseek file failed.\r\n");
		return;
	}

	error = f_write(&g_fileObject, &buffer_channel[channel_number][buffer_channel_write], sizeof(buffer_channel[channel_number][buffer_channel_write]), &bytesWritten);
	if ((error) || (bytesWritten != sizeof(buffer_channel[channel_number][buffer_channel_write])))
	{
		PRINTF("Write file failed.\r\n");
		return;
	}

	if (f_close(&g_fileObject))
	{
		PRINTF("\r\nClose file failed.\r\n");
		return;
	}
}

//setup_complete = true;


