
#include <string.h>
#include "FreeRTOS.h"
#include "request_error.h"

message_t *request_error_encode_alloc(request_error_message_t *request_error) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_REQUEST_ERROR_LENGTH);
    request_error_encode(message, request_error);
    return message;
}

void request_error_encode(message_t *message, request_error_message_t *request_error) {
    message->id = 2;
    message->size = 18;
    memcpy((void *)message->data, request_error, sizeof(request_error_message_t));
}

void request_error_decode(message_t *message, request_error_message_t *request_error) {
    memcpy((void *)request_error, (void *)message->data, sizeof(request_error_message_t));
}

void request_error_send(request_error_message_t *request_error) {
    message_t *message = request_error_encode_alloc(request_error);
    message_send(message);
}
