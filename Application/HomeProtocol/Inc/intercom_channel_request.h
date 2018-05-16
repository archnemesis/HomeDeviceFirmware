
#ifndef _INTERCOM_CHANNEL_REQUEST_MESSAGE_H_
#define _INTERCOM_CHANNEL_REQUEST_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_INTERCOM_CHANNEL_REQUEST_ID 3
#define MESSAGE_INTERCOM_CHANNEL_REQUEST_LENGTH 6

/**
 * Sent when a client is requesting communications from the server.
 */
struct intercom_channel_request_message {
  char hwid_callee[6];
} __packed;

typedef struct intercom_channel_request_message intercom_channel_request_message_t;

message_t *intercom_channel_request_encode_alloc(intercom_channel_request_message_t *intercom_channel_request);

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

#ifdef __cplusplus
}
#endif

#endif
