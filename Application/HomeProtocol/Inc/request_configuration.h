
#ifndef _REQUEST_CONFIGURATION_MESSAGE_H_
#define _REQUEST_CONFIGURATION_MESSAGE_H_

#include "message.h"
#include <stdint.h>

#define MESSAGE_REQUEST_CONFIGURATION_ID 1
#define MESSAGE_REQUEST_CONFIGURATION_LENGTH 26

/**
 * Sent to request configuration from server.
 */
struct request_configuration_message {
  char hwid[6];
  uint32_t ipaddr;
  char name[16];
} __packed;

typedef struct request_configuration_message request_configuration_message_t;

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param request_configuration_message
 */
void request_configuration_encode(message_t *message, request_configuration_message_t *request_configuration);

/**
 * Decode a request_configuration_message stored in a message wrapper.
 * @param message
 * @param request_configuration_message
 */
void request_configuration_decode(message_t *message, request_configuration_message_t *request_configuration);

/**
 * Shortcut to send a message.
 * @param message
 */
void request_configuration_send(request_configuration_message_t *request_configuration);

#endif
