/*
 * thread_ui.h
 *
 *  Created on: Feb 21, 2018
 *      Author: Robin
 */

#ifndef HOMEPROTOCOL_INC_THREAD_UI_H_
#define HOMEPROTOCOL_INC_THREAD_UI_H_

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>

extern QueueHandle_t xConsoleMessageQueue;

void UserInterfaceThread_Init(void);
void UserInterfaceThread_Main(const void * argument);
void UserInterfaceThread_PrintConsoleString(const char * string, uint8_t copy);
void UserInterfaceThread_SetDisplayName(const char *name);
void UserInterfaceThread_SetDescription(const char *desc);
void UserInterfaceThread_SetNetworkAddress(const char *addr);

#endif /* HOMEPROTOCOL_INC_THREAD_UI_H_ */
