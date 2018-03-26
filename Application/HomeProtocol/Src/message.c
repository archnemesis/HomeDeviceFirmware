#include "message.h"


__weak void message_send(message_t *message)
{
    vPortFree(message);
}
