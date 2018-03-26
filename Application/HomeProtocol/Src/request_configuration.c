
#include <string.h>
#include "FreeRTOS.h"
#include "request_configuration.h"

message_t *request_configuration_encode_alloc(request_configuration_message_t *request_configuration) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_REQUEST_CONFIGURATION_LENGTH);
    request_configuration_encode(message, request_configuration);
    return message;
}

void request_configuration_encode(message_t *message, request_configuration_message_t *request_configuration) {
    message->id = 1;
    message->size = 26;
    memcpy((void *)message->data, request_configuration, sizeof(request_configuration_message_t));
}

void request_configuration_decode(message_t *message, request_configuration_message_t *request_configuration) {
    memcpy((void *)request_configuration, (void *)message->data, sizeof(request_configuration_message_t));
}

void request_configuration_send(request_configuration_message_t *request_configuration) {
    message_t *message = request_configuration_encode_alloc(request_configuration);
    message_send(message);
}
