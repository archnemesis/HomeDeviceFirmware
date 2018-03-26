
#include <string.h>
#include "FreeRTOS.h"
#include "ping.h"

message_t *ping_encode_alloc(ping_message_t *ping) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_PING_LENGTH);
    ping_encode(message, ping);
    return message;
}

void ping_encode(message_t *message, ping_message_t *ping) {
    message->id = 6;
    message->size = 4;
    memcpy((void *)message->data, ping, sizeof(ping_message_t));
}

void ping_decode(message_t *message, ping_message_t *ping) {
    memcpy((void *)ping, (void *)message->data, sizeof(ping_message_t));
}

void ping_send(ping_message_t *ping) {
    message_t *message = ping_encode_alloc(ping);
    message_send(message);
}
