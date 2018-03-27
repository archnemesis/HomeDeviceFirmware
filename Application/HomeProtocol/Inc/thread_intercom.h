/*
 * thread_intercom.h
 *
 *  Created on: Mar 24, 2018
 *      Author: Robin
 */

#ifndef HOMEPROTOCOL_INC_THREAD_INTERCOM_H_
#define HOMEPROTOCOL_INC_THREAD_INTERCOM_H_

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>

#define INTERCOM_NOTIFY_OPEN_CHANNEL 0x01
#define INTERCOM_NOTIFY_CLOSE_CHANNEL 0x02

extern QueueHandle_t xIncomingAudio;
extern QueueHandle_t xOutgoingAudio;

void IntercomThread_Init(void);
void IntercomThread_ChannelOpen(in_addr_t addr, in_port_t port);
void IntercomThread_ChannelClose(void);
void IntercomThread_Main(const void * argument);
void IntercomThread_VolumeUp(void);
void IntercomThread_VolumeDn(void);

#endif /* HOMEPROTOCOL_INC_THREAD_INTERCOM_H_ */
