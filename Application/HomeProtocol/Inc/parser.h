/*
 * parser.h
 *
 *  Created on: Feb 19, 2018
 *      Author: Robin
 */

#ifndef HOMEPROTOCOL_INC_PARSER_H_
#define HOMEPROTOCOL_INC_PARSER_H_

#include "message.h"
#include "lwip/netbuf.h"

typedef void (*ParserCallback_t)(struct message * message);

typedef struct ParserContext {
	uint8_t in_header;
	uint8_t in_body;
	uint8_t got_header;
	int byte_count;
	int bytes_expected;
	int error_count;
	int rx_count;
	message_t *buffer;
	ParserCallback_t callback;
} ParserContext_t;

void HomeProtocol_ParserInit(ParserContext_t *ctx);
void HomeProtocol_ParserSetCallback(ParserContext_t *ctx, ParserCallback_t callback);
void HomeProtocol_ParserProcessBytes(ParserContext_t *ctx, unsigned char *data, size_t len);

#endif /* HOMEPROTOCOL_INC_PARSER_H_ */
