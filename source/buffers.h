/*
 * buffers.h
 *
 *  Created on: Apr 4, 2021
 *      Author: IanHellmer
 */

#ifndef BUFFERS_H_
#define BUFFERS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MK64F12.h"
#include "stdbool.h"
#include "peripherals.h"

#define buffer_a false
#define buffer_b true

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void PlaceInBuffer(uint16_t value, volatile unsigned int channel);


#endif /* BUFFERS_H_ */
