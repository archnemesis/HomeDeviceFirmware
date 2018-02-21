/*
 * thread_client.c
 *
 *  Created on: Feb 20, 2018
 *      Author: Robin
 */

#include "thread_client.h"
#include "ethernetif.h"
#include "lwip/api.h"
#include "lwip/ip_addr.h"
#include <string.h>

#include "parser.h"

void ClientThread_Init(void)
{

}

void ClientThread_ParserCallback(struct message * message)
{
	switch (message->id) {
		case MESSAGE_REQUEST_ERROR_ID:
		{
			request_error_message_t err;
			request_error_decode(message, (request_error_message_t *)&err);
			switch (err.code) {
			case RequestError:
				// TODO: handle request error
				break;
			default:
				// TODO: handle unknown error
				break;
			}

			break;
		}
		case MESSAGE_CONFIGURATION_PAYLOAD_ID:
		{
			configuration_payload_message_t payload;
			configuration_payload_decode(message, &payload);
			// TODO: handle configuration payload
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_SET);
			break;
		}
		default:
			break;
	}

	vPortFree(message);
}

void ClientThread_Main(const void * argument)
{
	(void)argument;

	message_t *message;
	struct netconn *conn = NULL;
	struct netbuf *buf;
	ip4_addr_t remote;
	err_t error;
	TickType_t timeout_mark = 0;

	ipaddr_aton(HOMESERVER_HOST, &remote);
	message = pvPortMalloc(sizeof(message_t));
	conn = netconn_new(NETCONN_TCP);

	while (1) {
		if (netconn_connect(conn, &remote, HOMESERVER_PORT) == ERR_OK) {
			netconn_set_recvtimeout(conn, 10);

			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

			request_configuration_message_t req;
			req.hwid[0] = 0x00;
			req.hwid[1] = 0x01;
			req.hwid[2] = 0x02;
			req.hwid[3] = 0x03;
			req.hwid[4] = 0x04;
			req.hwid[5] = 0x05;
			req.ipaddr = (uint32_t)remote.addr;
			strncpy((char *)req.name, "test", 16);

			request_configuration_encode(message, &req);
			netconn_write(conn, "AE", 2, NETCONN_NOCOPY);
			netconn_write(conn, (void *)message, MESSAGE_HEADER_SIZE + MESSAGE_REQUEST_CONFIGURATION_LENGTH, NETCONN_NOCOPY);

			timeout_mark = xTaskGetTickCount();

			while (1) {
				error = netconn_recv(conn, &buf);

				// TODO: check for messages on queue, send here

				switch (error) {
				case ERR_OK:
					HomeProtocol_ParserProcessNetbuf(buf, ClientThread_ParserCallback);
					timeout_mark = xTaskGetTickCount();
					break;
				case ERR_TIMEOUT:
				{
					if ((xTaskGetTickCount() - timeout_mark) > (10 * configTICK_RATE_HZ)) {
						ping_message_t ping;
						ping.timestamp = 1;
						ping_encode(message, &ping);
						netconn_write(conn, "AE", 2, NETCONN_NOCOPY);
						netconn_write(conn, (void *)message, MESSAGE_HEADER_SIZE + MESSAGE_PING_LENGTH, NETCONN_NOCOPY);
						timeout_mark = xTaskGetTickCount();
					}
					break;
				}
				default:
					netconn_close(conn);
					goto finished_connection;
					break;
				}
			}
		}

		finished_connection: ;

		/*
		 * Connection failed or was dropped, so we wait a second
		 * and try to reconnect.
		 */
		vTaskDelay(configTICK_RATE_HZ);
	}

	netconn_delete(conn);
}
