/*
 * thread_intercom.c
 *
 *  Created on: Mar 24, 2018
 *      Author: Robin
 */

#include "thread_intercom.h"
#include <limits.h>
#include <string.h>
#include <lwip/sockets.h>
#include "FreeRTOS.h"
#include "task.h"
#include "dac.h"
#include "tim.h"
#include "gpio.h"

QueueHandle_t xIncomingAudio;
QueueHandle_t xOutgoingAudio;
QueueHandle_t xBufferQueue;

static in_addr_t remote_addr;
static in_port_t remote_port;
static TaskHandle_t intercom_task;

char *dac_buffer;

#define FRAME_SIZE 320

void IntercomThread_Init(void)
{
	xIncomingAudio = xQueueCreate(10, sizeof(int16_t *));
	xOutgoingAudio = xQueueCreate(10, sizeof(int16_t *));
	xBufferQueue = xQueueCreate(10, sizeof(int16_t *));
}

void IntercomThread_ChannelOpen(in_addr_t addr, in_port_t port)
{
	remote_addr = addr;
	remote_port = port;
	xTaskNotify(intercom_task, INTERCOM_NOTIFY_OPEN_CHANNEL, eSetBits);
}

void IntercomThread_ChannelClose(void)
{
	xTaskNotify(intercom_task, INTERCOM_NOTIFY_CLOSE_CHANNEL, eSetBits);
}

void IntercomThread_Main(const void * argument)
{
	char *buf_in;
	char *buf_out;
	const int timeout = 8;
    fd_set read_fds;
    int buf_count = 0;
    int bytes_to_send = 0;
	int ret;
	int socket_rx;
	int frame_count = 0;
	int dac_started = 0;
	struct sockaddr_in ra;
	struct sockaddr_in ta;
	struct sockaddr_in sa;
    struct timeval tv;
	uint32_t ulNotifiedValue;

	buf_in = pvPortMalloc(FRAME_SIZE);
	if (buf_in == NULL) {
		Error_Handler();
	}

	dac_buffer = pvPortMalloc(FRAME_SIZE * 2);
	if (dac_buffer == NULL) {
		Error_Handler();
	}
	memset((void *)dac_buffer, 0, FRAME_SIZE * 2);

	tv.tv_sec  = timeout / 1000;
	tv.tv_usec = ( timeout % 1000 ) * 1000;

	intercom_task = xTaskGetCurrentTaskHandle();

	for (;;) {
		xTaskNotifyWait(0x00, ULONG_MAX, &ulNotifiedValue, portMAX_DELAY);

		if ((ulNotifiedValue & INTERCOM_NOTIFY_OPEN_CHANNEL) != 0) {
			socket_rx = socket(AF_INET, SOCK_DGRAM, 0);

			if (socket_rx < 0) {
				Error_Handler();
			}

			ret = fcntl(socket_rx, F_SETFL, fcntl(socket_rx, F_GETFL, 0) | O_NONBLOCK);

			memset(&ra, 0, sizeof(struct sockaddr_in));
			ra.sin_family = AF_INET;
			ra.sin_addr.s_addr = inet_addr("10.1.1.141");
			ra.sin_port = htons(2050);

			memset(&ta, 0, sizeof(struct sockaddr_in));
			ta.sin_family = AF_INET;
			ta.sin_addr.s_addr = remote_addr;
			ta.sin_port = remote_port;

			setsockopt(socket_rx, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(struct timeval));

			if (bind(socket_rx, (struct sockaddr *)&ra, sizeof(struct sockaddr_in)) < 0) {
				Error_Handler();
			}

			buf_count = 0;
			bytes_to_send = 0;
			dac_started = 0;
			frame_count = 0;

			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);

			for (;;) {
				if (xTaskNotifyWait(0x00, ULONG_MAX, &ulNotifiedValue, pdMS_TO_TICKS(1)) == pdTRUE) {
					if ((ulNotifiedValue & INTERCOM_NOTIFY_CLOSE_CHANNEL) == INTERCOM_NOTIFY_CLOSE_CHANNEL) {
						close(socket_rx);
						HAL_TIM_Base_Stop(&htim6);
						HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
						HAL_DAC_Stop(&hdac, DAC_CHANNEL_1);

						HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

						if (bytes_to_send > 0) {
							vPortFree(buf_out);
						}
						break;
					}
				}

				void *ptr;
				while (xQueueReceive(xBufferQueue, (void *)&ptr, 0)) {
					vPortFree(ptr);
				}

				if (bytes_to_send == 0 && xQueueReceive(xOutgoingAudio, (void *)&buf_out, pdMS_TO_TICKS(1)) == pdTRUE) {
					bytes_to_send = FRAME_SIZE;
				}

				if (bytes_to_send > 0) {
					void *ptr = (void *)(buf_out + (FRAME_SIZE - bytes_to_send));
					ret = sendto(socket_rx, ptr, bytes_to_send, 0, (struct sockaddr *)&ta, sizeof(struct sockaddr));
					if (ret < 0) {
						if (errno == EINTR || errno == EAGAIN) {
							continue;
						}
					}

					bytes_to_send -= ret;

					/* frame sent so free buffer and get next frame */
					if (bytes_to_send == 0) {
						vPortFree(buf_out);
					}
				}

				socklen_t sz;
				memset(&sa, 0, sizeof(struct sockaddr_in));

				/* try to get enough bytes to fill buffer completely */
				ret = (int)recvfrom(
						socket_rx,
						(void *)(buf_in + buf_count),
						FRAME_SIZE - buf_count,
						0,
						(struct sockaddr *)&sa,
						&sz);

				if (ret < 0) {
					if (errno == EINTR || errno == EAGAIN) {
						continue;
					}

					close(socket_rx);
					break;
				}

				if (ret > 0) {
					buf_count += ret;
					if (buf_count == FRAME_SIZE) {
						/* Send frame to audio queue and alloc new buffer */
						if (xQueueSendToBack(xIncomingAudio, (void *)&buf_in, 0) == pdTRUE) {
							frame_count++;

							HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);

							if (frame_count > 3 && dac_started == 0) {
								HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t *)dac_buffer, FRAME_SIZE * 2, DAC_ALIGN_12B_R);
								HAL_TIM_Base_Start(&htim6);
								dac_started = 1;
							}

							buf_in = pvPortMalloc(FRAME_SIZE);
						}
						buf_count = 0;
					}
				}
			}
		}
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	BaseType_t xTaskWokenByReceive0 = pdFALSE;
	BaseType_t xTaskWokenByReceive1 = pdFALSE;
	char *buf = NULL;

	/* fill second half of buffer with new frame */
	if (xQueueReceiveFromISR(xIncomingAudio, (void *)&buf, &xTaskWokenByReceive1) == pdTRUE) {
		memcpy((void *)(dac_buffer + FRAME_SIZE), (void *)buf, FRAME_SIZE);
		xQueueSendFromISR(xBufferQueue, (void *)&buf, &xTaskWokenByReceive0);
	}
	else {
		__BKPT();
	}

	if (xTaskWokenByReceive0 != pdFALSE || xTaskWokenByReceive1 != pdFALSE) {
        taskYIELD ();
    }
}

void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac)
{
	BaseType_t xTaskWokenByReceive0 = pdFALSE;
	BaseType_t xTaskWokenByReceive1 = pdFALSE;
	char *buf = NULL;

	/* fill first half of buffer with new frame */
	if (xQueueReceiveFromISR(xIncomingAudio, (void *)&buf, &xTaskWokenByReceive1) == pdTRUE) {
		memcpy((void *)dac_buffer, (void *)buf, FRAME_SIZE);
		xQueueSendFromISR(xBufferQueue, (void *)&buf, &xTaskWokenByReceive0);
	}
	else {
		__BKPT();
	}

	if (xTaskWokenByReceive0 != pdFALSE || xTaskWokenByReceive1 != pdFALSE) {
        taskYIELD ();
    }
}

void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac)
{
	__BKPT();
}

void HAL_DAC_DMAUnderrunCallbackCh1(DAC_HandleTypeDef *hdac)
{
	__BKPT();
}
