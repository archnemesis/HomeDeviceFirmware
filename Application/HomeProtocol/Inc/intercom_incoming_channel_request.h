
#ifndef _INTERCOM_INCOMING_CHANNEL_REQUEST_MESSAGE_H_
#define _INTERCOM_INCOMING_CHANNEL_REQUEST_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_INTERCOM_INCOMING_CHANNEL_REQUEST_ID 9
#define MESSAGE_INTERCOM_INCOMING_CHANNEL_REQUEST_LENGTH 42

/**
 * Ask a client to accept an intercom channel request.
 */
struct intercom_incoming_channel_request_message {
  char caller_hwid[6];
  uint32_t addr;
  char display_name[16];
  char description[16];
} __packed;

typedef struct intercom_incoming_channel_request_message intercom_incoming_channel_request_message_t;

message_t *intercom_incoming_channel_request_encode_alloc(intercom_incoming_channel_request_message_t *intercom_incoming_channel_request);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param intercom_incoming_channel_request_message
 */
void intercom_incoming_channel_request_encode(message_t *message, intercom_incoming_channel_request_message_t *intercom_incoming_channel_request);

/**
 * Decode a intercom_incoming_channel_request_message stored in a message wrapper.
 * @param message
 * @param intercom_incoming_channel_request_message
 */
void intercom_incoming_channel_request_decode(message_t *message, intercom_incoming_channel_request_message_t *intercom_incoming_channel_request);

/**
 * Shortcut to send a message.
 * @param message
 */
void intercom_incoming_channel_request_send(intercom_incoming_channel_request_message_t *intercom_incoming_channel_request);

#ifdef __cplusplus
}
#endif

#endif
