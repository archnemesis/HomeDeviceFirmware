
#ifndef _INTERCOM_CHANNEL_REQUEST_MESSAGE_H_
#define _INTERCOM_CHANNEL_REQUEST_MESSAGE_H_

#include "message.h"
#include <stdint.h>

#define MESSAGE_INTERCOM_CHANNEL_REQUEST_ID 3
#define MESSAGE_INTERCOM_CHANNEL_REQUEST_LENGTH 12

/**
 * Sent when a client is requesting communications from the server.
 */
struct intercom_channel_request_message {
  char hwid_caller[6];
  uint32_t remote_channel_ip;
  uint16_t remote_channel_port;
} __attribute__((packed));

typedef struct intercom_channel_request_message intercom_channel_request_message_t;

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param intercom_channel_request_message
 */
void intercom_channel_request_encode(message_t *message, intercom_channel_request_message_t *intercom_channel_request);

/**
 * Decode a intercom_channel_request_message stored in a message wrapper.
 * @param message
 * @param intercom_channel_request_message
 */
void intercom_channel_request_decode(message_t *message, intercom_channel_request_message_t *intercom_channel_request);

/**
 * Shortcut to send a message.
 * @param message
 */
void intercom_channel_request_send(intercom_channel_request_message_t *intercom_channel_request);

#endif