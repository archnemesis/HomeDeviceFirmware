
#include <string.h>
#include "intercom_channel_request.h"

void intercom_channel_request_encode(message_t *message, intercom_channel_request_message_t *intercom_channel_request) {
    message->id = 3;
    message->size = 12;
    memcpy((void *)message->data, intercom_channel_request, sizeof(intercom_channel_request_message_t));
}

void intercom_channel_request_decode(message_t *message, intercom_channel_request_message_t *intercom_channel_request) {
    memcpy((void *)intercom_channel_request, (void *)message, sizeof(intercom_channel_request_message_t));
}

void intercom_channel_request_send(intercom_channel_request_message_t *intercom_channel_request) {
    (void)intercom_channel_request;
}
