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

typedef void (*ParserCallback)(struct message * message);

void HomeProtocol_ParserInit(void);
void HomeProtocol_ParserProcessNetbuf(struct netbuf *netbuf, ParserCallback callback);

#endif /* HOMEPROTOCOL_INC_PARSER_H_ */
