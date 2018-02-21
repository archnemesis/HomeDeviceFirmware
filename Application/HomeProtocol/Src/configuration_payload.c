
#include <string.h>
#include "configuration_payload.h"

void configuration_payload_encode(message_t *message, configuration_payload_message_t *configuration_payload) {
    message->id = 5;
    message->size = 66;
    memcpy((void *)message->data, configuration_payload, sizeof(configuration_payload_message_t));
}

void configuration_payload_decode(message_t *message, configuration_payload_message_t *configuration_payload) {
    memcpy((void *)configuration_payload, (void *)message, sizeof(configuration_payload_message_t));
}

void configuration_payload_send(configuration_payload_message_t *configuration_payload) {
    (void)configuration_payload;
}
