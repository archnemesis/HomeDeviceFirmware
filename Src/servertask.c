/*
 * servertask.c
 *
 *  Created on: Feb 18, 2018
 *      Author: Robin
 */

#include "lwip/api.h"
#include "gpio.h"
#include "adc.h"
#include "tim.h"
#include <string.h>

QueueHandle_t xFrameQueue;

void ServerTaskInit(void)
{
	xFrameQueue = xQueueCreate(4, sizeof(uint16_t *));
}

void ServerTaskMain(void const * argument)
{
	struct netconn *conn;
	struct netconn *newconn;
	struct netbuf *buf;
	uint32_t seq = 0;
	uint16_t len;
	err_t err;
	void *data;

	BaseType_t result;
	uint16_t *chunk;
	const char message[] = "NucleoDemo2\r\n";

	if ((conn = netconn_new(NETCONN_TCP)) != NULL) {
		err = netconn_bind(conn, NULL, 2005);

		if (err == ERR_OK) {
			netconn_listen(conn);

			while (1) {
				err = netconn_accept(conn, &newconn);
				netconn_set_recvtimeout(newconn, 1);
				HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
				HAL_TIM_Base_Start_IT(&htim2);
				HAL_ADC_Start_IT(&hadc1);

				if (err == ERR_OK) {
					while (1) {
						result = xQueueReceive(xFrameQueue, (void *)&chunk, portTICK_PERIOD_MS * 10);

						if (result == pdTRUE) {
							netconn_write(newconn, (void *)chunk, 256, NETCONN_NOCOPY);
							vPortFree(chunk);
						}

						err = netconn_recv(newconn, &buf);

						if (err == ERR_OK) {
							do {
								netbuf_data(buf, &data, &len);
								// do something with incoming data
							}
							while (netbuf_next(buf) >= 0);

							netbuf_delete(buf);
						}
						else if (err == ERR_TIMEOUT) {

						}
						else {
							netconn_close(newconn);
							netconn_delete(newconn);
							HAL_ADC_Stop_IT(&hadc1);
							HAL_TIM_Base_Stop_IT(&htim2);
							HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);
							break;
						}
					}
				}
			}
		}
	}
}
