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

void HomeProtocol_ParserInit(ParserContext_t *ctx)
{
	ctx->in_header = 0;
	ctx->in_body = 0;
	ctx->got_header = 0;
	ctx->byte_count = 0;
	ctx->bytes_expected = 0;
	ctx->error_count = 0;
	ctx->buffer = pvPortMalloc(sizeof(message_t));
	ctx->callback = NULL;

	if (ctx->buffer == NULL) {
		Error_Handler();
	}
}

void HomeProtocol_ParserSetCallback(ParserContext_t *ctx, ParserCallback_t callback)
{
	ctx->callback = callback;
}

void HomeProtocol_ParserProcessBytes(ParserContext_t *ctx, unsigned char *data, size_t len)
{
	uint16_t i;

	for (i = 0; i < len; i++) {
		if (ctx->in_body == 1) {
			*((unsigned char *)ctx->buffer + (ctx->byte_count++)) = data[i];

			//
			// once we know the size of the message, we can
			// adjust what we are expecting (save traffic)
			//
			if (ctx->got_header == 0 && ctx->byte_count == MESSAGE_HEADER_SIZE) {
				if (ctx->buffer->size > MESSAGE_MAX_TOTAL_SIZE) {
					ctx->in_header = 0;
					ctx->in_body = 0;
					ctx->got_header = 0;
					ctx->byte_count = 0;
					ctx->bytes_expected = 0;
					ctx->buffer->id = 0;
					ctx->buffer->size = 0;
					ctx->error_count++;

					continue;
				}

				ctx->bytes_expected = ctx->buffer->size;
				ctx->got_header = 1;
			}

			if (ctx->byte_count == ctx->bytes_expected) {
				struct message *msg = (struct message *)pvPortMalloc(ctx->buffer->size);
				memcpy((void *)msg, (void *)ctx->buffer, ctx->buffer->size);
				ctx->callback(msg);

				ctx->rx_count++;
				ctx->in_header = 0;
				ctx->got_header = 0;
				ctx->in_body = 0;
				ctx->byte_count = 0;
				ctx->bytes_expected = 0;
				ctx->buffer->id = 0;
				ctx->buffer->size = 0;
			}
		}
		else if (ctx->in_header == 0) {
			if (data[i] == 'A') {
				ctx->in_header = 1;
			}
		}
		else if (ctx->in_header == 1) {
			if (data[i] == 'E') {
				ctx->in_body = 1;
				ctx->in_header = 0;
				ctx->byte_count = 0;
				ctx->bytes_expected = MESSAGE_MAX_TOTAL_SIZE;
			}
			else {
				ctx->in_header = 0;
				ctx->got_header = 0;
				ctx->in_body = 0;
				ctx->byte_count = 0;
				ctx->bytes_expected = 0;
				ctx->buffer->id = 0;
				ctx->buffer->size = 0;
				ctx->error_count++;
			}
		}
	}
}
