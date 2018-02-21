
#include <string.h>
#include "ping.h"

void ping_encode(message_t *message, ping_message_t *ping) {
    message->id = 6;
    message->size = 4;
    memcpy((void *)message->data, ping, sizeof(ping_message_t));
}

void ping_decode(message_t *message, ping_message_t *ping) {
    memcpy((void *)ping, (void *)message, sizeof(ping_message_t));
}

void ping_send(ping_message_t *ping) {
    (void)ping;
}
