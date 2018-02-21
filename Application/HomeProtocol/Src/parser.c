/*
 * parser.c
 *
 *  Created on: Feb 19, 2018
 *      Author: Robin
 */

#include "parser.h"
#include "message.h"
#include "FreeRTOS.h"
#include <string.h>

uint32_t rx_count = 0;
uint32_t error_count = 0;

void HomeProtocol_ParserInit(void)
{

}

void HomeProtocol_ParserProcessNetbuf(struct netbuf *netbuf, ParserCallback callback)
{
	char *data;
	uint16_t len;
	uint16_t i;

	static struct {
		uint8_t in_header;
		uint8_t in_body;
		uint8_t got_header;
		uint16_t byte_count;
		uint16_t bytes_expected;
	} state = {
			.in_header = 0,
			.in_body = 0,
			.got_header = 0,
			.byte_count = 0,
			.bytes_expected = 0
	};

	static message_t *buffer = NULL;

	if (buffer == NULL) {
		buffer = pvPortMalloc(sizeof(message_t));
	}

	do {
		netbuf_data(netbuf, (void *)&data, &len);

		for (i = 0; i < len; i++) {
			if (state.in_body == 1) {
				*((char *)buffer + (state.byte_count++)) = data[i];

				//
				// once we know the size of the message, we can
				// adjust what we are expecting (save traffic)
				//
				if (state.got_header == 0 && state.byte_count == MESSAGE_HEADER_SIZE) {
					if (buffer->size > MESSAGE_MAX_TOTAL_SIZE) {
						state.in_header = 0;
						state.in_body = 0;
						state.byte_count = 0;
						state.bytes_expected = 0;
						buffer->id = 0;
						buffer->size = 0;
						error_count++;

						continue;
					}

					state.bytes_expected = buffer->size;
					state.got_header = 1;
				}

				if (state.byte_count == state.bytes_expected) {
					struct message *msg = (struct message *)pvPortMalloc(buffer->size);
					memcpy((void *)msg, (void *)buffer, buffer->size);
					callback(msg);

					rx_count++;
					state.in_header = 0;
					state.got_header = 0;
					state.in_body = 0;
					state.byte_count = 0;
					state.bytes_expected = 0;
					buffer->id = 0;
					buffer->size = 0;
				}
			}
			else if (state.in_header == 0) {
				if (data[i] == 'A') {
					state.in_header = 1;
				}
			}
			else if (state.in_header == 1) {
				if (data[i] == 'E') {
					state.in_body = 1;
					state.in_header = 0;
					state.byte_count = 0;
					state.bytes_expected = MESSAGE_MAX_TOTAL_SIZE;
				}
				else {
					state.in_header = 0;
					state.got_header = 0;
					state.in_body = 0;
					state.byte_count = 0;
					state.bytes_expected = 0;
					buffer->id = 0;
					buffer->size = 0;
					error_count++;
				}
			}
		}
	}
	while (netbuf_next(netbuf) >= 0);

	netbuf_delete(netbuf);
}
