
#include <string.h>
#include "intercom_channel_accept.h"

void intercom_channel_accept_encode(message_t *message, intercom_channel_accept_message_t *intercom_channel_accept) {
    message->id = 4;
    message->size = 12;
    memcpy((void *)message->data, intercom_channel_accept, sizeof(intercom_channel_accept_message_t));
}

void intercom_channel_accept_decode(message_t *message, intercom_channel_accept_message_t *intercom_channel_accept) {
    memcpy((void *)intercom_channel_accept, (void *)message, sizeof(intercom_channel_accept_message_t));
}

void intercom_channel_accept_send(intercom_channel_accept_message_t *intercom_channel_accept) {
    (void)intercom_channel_accept;
}
