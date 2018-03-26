
#include <string.h>
#include "FreeRTOS.h"
#include "intercom_channel_request.h"

message_t *intercom_channel_request_encode_alloc(intercom_channel_request_message_t *intercom_channel_request) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_INTERCOM_CHANNEL_REQUEST_LENGTH);
    intercom_channel_request_encode(message, intercom_channel_request);
    return message;
}

void intercom_channel_request_encode(message_t *message, intercom_channel_request_message_t *intercom_channel_request) {
    message->id = 3;
    message->size = 6;
    memcpy((void *)message->data, intercom_channel_request, sizeof(intercom_channel_request_message_t));
}

void intercom_channel_request_decode(message_t *message, intercom_channel_request_message_t *intercom_channel_request) {
    memcpy((void *)intercom_channel_request, (void *)message->data, sizeof(intercom_channel_request_message_t));
}

void intercom_channel_request_send(intercom_channel_request_message_t *intercom_channel_request) {
    message_t *message = intercom_channel_request_encode_alloc(intercom_channel_request);
    message_send(message);
}
