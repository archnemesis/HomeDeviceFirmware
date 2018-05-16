
#include <string.h>
#include "FreeRTOS.h"
#include "intercom_incoming_channel_request.h"

message_t *intercom_incoming_channel_request_encode_alloc(intercom_incoming_channel_request_message_t *intercom_incoming_channel_request) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_INTERCOM_INCOMING_CHANNEL_REQUEST_LENGTH);
    intercom_incoming_channel_request_encode(message, intercom_incoming_channel_request);
    return message;
}

void intercom_incoming_channel_request_encode(message_t *message, intercom_incoming_channel_request_message_t *intercom_incoming_channel_request) {
    message->id = 9;
    message->size = 42;
    memcpy((void *)message->data, intercom_incoming_channel_request, sizeof(intercom_incoming_channel_request_message_t));
}

void intercom_incoming_channel_request_decode(message_t *message, intercom_incoming_channel_request_message_t *intercom_incoming_channel_request) {
    memcpy((void *)intercom_incoming_channel_request, (void *)message->data, sizeof(intercom_incoming_channel_request_message_t));
}

void intercom_incoming_channel_request_send(intercom_incoming_channel_request_message_t *intercom_incoming_channel_request) {
    message_t *message = intercom_incoming_channel_request_encode_alloc(intercom_incoming_channel_request);
    message_send(message);
}
