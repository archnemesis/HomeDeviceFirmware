
#include <string.h>
#include "FreeRTOS.h"
#include "intercom_channel_accept.h"

message_t *intercom_channel_accept_encode_alloc(intercom_channel_accept_message_t *intercom_channel_accept) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_INTERCOM_CHANNEL_ACCEPT_LENGTH);
    intercom_channel_accept_encode(message, intercom_channel_accept);
    return message;
}

void intercom_channel_accept_encode(message_t *message, intercom_channel_accept_message_t *intercom_channel_accept) {
    message->id = 4;
    message->size = 6;
    memcpy((void *)message->data, intercom_channel_accept, sizeof(intercom_channel_accept_message_t));
}

void intercom_channel_accept_decode(message_t *message, intercom_channel_accept_message_t *intercom_channel_accept) {
    memcpy((void *)intercom_channel_accept, (void *)message->data, sizeof(intercom_channel_accept_message_t));
}

void intercom_channel_accept_send(intercom_channel_accept_message_t *intercom_channel_accept) {
    message_t *message = intercom_channel_accept_encode_alloc(intercom_channel_accept);
    message_send(message);
}
