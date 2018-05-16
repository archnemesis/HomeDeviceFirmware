
#ifndef _REQUEST_ERROR_MESSAGE_H_
#define _REQUEST_ERROR_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_REQUEST_ERROR_ID 2
#define MESSAGE_REQUEST_ERROR_LENGTH 18

/**
 * Returned when a request is denied or failed due to error.
 */
struct request_error_message {
  uint16_t code;
  char message[16];
} __packed;

typedef struct request_error_message request_error_message_t;

message_t *request_error_encode_alloc(request_error_message_t *request_error);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param request_error_message
 */
void request_error_encode(message_t *message, request_error_message_t *request_error);

/**
 * Decode a request_error_message stored in a message wrapper.
 * @param message
 * @param request_error_message
 */
void request_error_decode(message_t *message, request_error_message_t *request_error);

/**
 * Shortcut to send a message.
 * @param message
 */
void request_error_send(request_error_message_t *request_error);

#ifdef __cplusplus
}
#endif

#endif
