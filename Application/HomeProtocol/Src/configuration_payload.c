
#include <string.h>
#include "FreeRTOS.h"
#include "configuration_payload.h"

message_t *configuration_payload_encode_alloc(configuration_payload_message_t *configuration_payload) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_CONFIGURATION_PAYLOAD_LENGTH);
    configuration_payload_encode(message, configuration_payload);
    return message;
}

void configuration_payload_encode(message_t *message, configuration_payload_message_t *configuration_payload) {
    message->id = 5;
    message->size = 234;
    memcpy((void *)message->data, configuration_payload, sizeof(configuration_payload_message_t));
}

void configuration_payload_decode(message_t *message, configuration_payload_message_t *configuration_payload) {
    memcpy((void *)configuration_payload, (void *)message->data, sizeof(configuration_payload_message_t));
}

void configuration_payload_send(configuration_payload_message_t *configuration_payload) {
    message_t *message = configuration_payload_encode_alloc(configuration_payload);
    message_send(message);
}
