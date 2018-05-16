
#ifndef _COMMAND_MESSAGE_H_
#define _COMMAND_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_COMMAND_ID 1
#define MESSAGE_COMMAND_LENGTH 2

/**
 * Commands that don't require arguments.
 */
struct command_message {
  uint16_t command_id;
} __packed;

typedef struct command_message command_message_t;

message_t *command_encode_alloc(command_message_t *command);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param command_message
 */
void command_encode(message_t *message, command_message_t *command);

/**
 * Decode a command_message stored in a message wrapper.
 * @param message
 * @param command_message
 */
void command_decode(message_t *message, command_message_t *command);

/**
 * Shortcut to send a message.
 * @param message
 */
void command_send(command_message_t *command);

#ifdef __cplusplus
}
#endif

#endif
