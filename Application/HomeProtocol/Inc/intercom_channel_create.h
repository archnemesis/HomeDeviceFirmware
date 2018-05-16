
#ifndef _INTERCOM_CHANNEL_CREATE_MESSAGE_H_
#define _INTERCOM_CHANNEL_CREATE_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_INTERCOM_CHANNEL_CREATE_ID 7
#define MESSAGE_INTERCOM_CHANNEL_CREATE_LENGTH 8

/**
 * Instruct an endpoint to open an intercom channel.
 */
struct intercom_channel_create_message {
  uint16_t port;
  char caller[6];
} __packed;

typedef struct intercom_channel_create_message intercom_channel_create_message_t;

message_t *intercom_channel_create_encode_alloc(intercom_channel_create_message_t *intercom_channel_create);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param intercom_channel_create_message
 */
void intercom_channel_create_encode(message_t *message, intercom_channel_create_message_t *intercom_channel_create);

/**
 * Decode a intercom_channel_create_message stored in a message wrapper.
 * @param message
 * @param intercom_channel_create_message
 */
void intercom_channel_create_decode(message_t *message, intercom_channel_create_message_t *intercom_channel_create);

/**
 * Shortcut to send a message.
 * @param message
 */
void intercom_channel_create_send(intercom_channel_create_message_t *intercom_channel_create);

#ifdef __cplusplus
}
#endif

#endif
