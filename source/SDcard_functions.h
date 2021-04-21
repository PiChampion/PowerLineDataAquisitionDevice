/*
 * SDcard_functions.h
 *
 *  Created on: Apr 4, 2021
 *      Author: IanHellmer
 */

#ifndef SDCARD_FUNCTIONS_H_
#define SDCARD_FUNCTIONS_H_

#include "fsl_common.h"

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief wait card insert function.
 */
status_t sdcardWaitCardInsert(void);

status_t initializeSDcard(void);

status_t initializeSDcardNoMKFS(void);

status_t makeDirectory(void);

void transferBuffersToSD(void);

void transferIndividualChannel(unsigned int channel_number);

#endif /* SDCARD_FUNCTIONS_H_ */
