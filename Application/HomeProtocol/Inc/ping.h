
#ifndef _PING_MESSAGE_H_
#define _PING_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_PING_ID 6
#define MESSAGE_PING_LENGTH 4

/**
 * A short packet that is used to test connectivity.
 */
struct ping_message {
  uint32_t timestamp;
} __packed;

typedef struct ping_message ping_message_t;

message_t *ping_encode_alloc(ping_message_t *ping);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param ping_message
 */
void ping_encode(message_t *message, ping_message_t *ping);

/**
 * Decode a ping_message stored in a message wrapper.
 * @param message
 * @param ping_message
 */
void ping_decode(message_t *message, ping_message_t *ping);

/**
 * Shortcut to send a message.
 * @param message
 */
void ping_send(ping_message_t *ping);

#ifdef __cplusplus
}
#endif

#endif
