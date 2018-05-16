
#ifndef _INTERCOM_CHANNEL_ACCEPT_MESSAGE_H_
#define _INTERCOM_CHANNEL_ACCEPT_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_INTERCOM_CHANNEL_ACCEPT_ID 4
#define MESSAGE_INTERCOM_CHANNEL_ACCEPT_LENGTH 6

/**
 * Sent when a client accepts communications from the server.
 */
struct intercom_channel_accept_message {
  uint32_t remote_addr;
  uint16_t remote_port;
} __packed;

typedef struct intercom_channel_accept_message intercom_channel_accept_message_t;

message_t *intercom_channel_accept_encode_alloc(intercom_channel_accept_message_t *intercom_channel_accept);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param intercom_channel_accept_message
 */
void intercom_channel_accept_encode(message_t *message, intercom_channel_accept_message_t *intercom_channel_accept);

/**
 * Decode a intercom_channel_accept_message stored in a message wrapper.
 * @param message
 * @param intercom_channel_accept_message
 */
void intercom_channel_accept_decode(message_t *message, intercom_channel_accept_message_t *intercom_channel_accept);

/**
 * Shortcut to send a message.
 * @param message
 */
void intercom_channel_accept_send(intercom_channel_accept_message_t *intercom_channel_accept);

#ifdef __cplusplus
}
#endif

#endif
