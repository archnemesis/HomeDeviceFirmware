
#include <string.h>
#include "FreeRTOS.h"
#include "intercom_channel_create.h"

message_t *intercom_channel_create_encode_alloc(intercom_channel_create_message_t *intercom_channel_create) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_INTERCOM_CHANNEL_CREATE_LENGTH);
    intercom_channel_create_encode(message, intercom_channel_create);
    return message;
}

void intercom_channel_create_encode(message_t *message, intercom_channel_create_message_t *intercom_channel_create) {
    message->id = 7;
    message->size = 8;
    memcpy((void *)message->data, intercom_channel_create, sizeof(intercom_channel_create_message_t));
}

void intercom_channel_create_decode(message_t *message, intercom_channel_create_message_t *intercom_channel_create) {
    memcpy((void *)intercom_channel_create, (void *)message->data, sizeof(intercom_channel_create_message_t));
}

void intercom_channel_create_send(intercom_channel_create_message_t *intercom_channel_create) {
    message_t *message = intercom_channel_create_encode_alloc(intercom_channel_create);
    message_send(message);
}
