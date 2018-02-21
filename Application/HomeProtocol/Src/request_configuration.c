
#include <string.h>
#include "request_configuration.h"

void request_configuration_encode(message_t *message, request_configuration_message_t *request_configuration) {
    message->id = 1;
    message->size = 26;
    memcpy((void *)message->data, request_configuration, sizeof(request_configuration_message_t));
}

void request_configuration_decode(message_t *message, request_configuration_message_t *request_configuration) {
    memcpy((void *)request_configuration, (void *)message, sizeof(request_configuration_message_t));
}

void request_configuration_send(request_configuration_message_t *request_configuration) {
    (void)request_configuration;
}
