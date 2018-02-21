/*
 * servertask.h
 *
 *  Created on: Feb 11, 2018
 *      Author: Robin
 */

#ifndef SERVERTASK_H_
#define SERVERTASK_H_

#include "FreeRTOS.h"

extern QueueHandle_t xFrameQueue;

void ServerTaskInit(void);
void ServerTaskMain(void const * argument);

#endif /* SERVERTASK_H_ */
