
#include <string.h>
#include "FreeRTOS.h"
#include "command.h"

message_t *command_encode_alloc(command_message_t *command) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_COMMAND_LENGTH);
    command_encode(message, command);
    return message;
}

void command_encode(message_t *message, command_message_t *command) {
    message->id = 1;
    message->size = 2;
    memcpy((void *)message->data, command, sizeof(command_message_t));
}

void command_decode(message_t *message, command_message_t *command) {
    memcpy((void *)command, (void *)message->data, sizeof(command_message_t));
}

void command_send(command_message_t *command) {
    message_t *message = command_encode_alloc(command);
    message_send(message);
}
