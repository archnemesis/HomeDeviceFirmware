
#ifndef _CONFIGURATION_PAYLOAD_MESSAGE_H_
#define _CONFIGURATION_PAYLOAD_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_CONFIGURATION_PAYLOAD_ID 5
#define MESSAGE_CONFIGURATION_PAYLOAD_LENGTH 234

/**
 * Delivers most settings needed by standard clients.
 */
struct configuration_payload_message {
  char display_name[32];
  char description[32];
  uint16_t theme;
  struct {
    uint16_t controltype;
    uint32_t min;
    uint32_t max;
    char name[16];
    char description[16];
  } __packed controls[4];
} __packed;

typedef struct configuration_payload_message configuration_payload_message_t;

message_t *configuration_payload_encode_alloc(configuration_payload_message_t *configuration_payload);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param configuration_payload_message
 */
void configuration_payload_encode(message_t *message, configuration_payload_message_t *configuration_payload);

/**
 * Decode a configuration_payload_message stored in a message wrapper.
 * @param message
 * @param configuration_payload_message
 */
void configuration_payload_decode(message_t *message, configuration_payload_message_t *configuration_payload);

/**
 * Shortcut to send a message.
 * @param message
 */
void configuration_payload_send(configuration_payload_message_t *configuration_payload);

#ifdef __cplusplus
}
#endif

#endif
