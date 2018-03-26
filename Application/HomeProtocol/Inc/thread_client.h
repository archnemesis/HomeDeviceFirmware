/*
 * thread_client.h
 *
 *  Created on: Feb 20, 2018
 *      Author: Robin
 */

#ifndef HOMEPROTOCOL_INC_THREAD_CLIENT_H_
#define HOMEPROTOCOL_INC_THREAD_CLIENT_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "message.h"

extern QueueHandle_t xQueueOutMessages;

void ClientThread_Init(void);
void ClientThread_Main(const void * argument);
void ClientThread_SendMessage(message_t *message);

#endif /* HOMEPROTOCOL_INC_THREAD_CLIENT_H_ */
