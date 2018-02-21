
#include <string.h>
#include "request_error.h"

void request_error_encode(message_t *message, request_error_message_t *request_error) {
    message->id = 2;
    message->size = 18;
    memcpy((void *)message->data, request_error, sizeof(request_error_message_t));
}

void request_error_decode(message_t *message, request_error_message_t *request_error) {
    memcpy((void *)request_error, (void *)message, sizeof(request_error_message_t));
}

void request_error_send(request_error_message_t *request_error) {
    (void)request_error;
}
