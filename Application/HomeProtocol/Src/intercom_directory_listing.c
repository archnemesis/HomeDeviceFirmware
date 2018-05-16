
#include <string.h>
#include "FreeRTOS.h"
#include "intercom_directory_listing.h"

message_t *intercom_directory_listing_encode_alloc(intercom_directory_listing_message_t *intercom_directory_listing) {
    message_t *message = pvPortMalloc(MESSAGE_HEADER_SIZE + MESSAGE_INTERCOM_DIRECTORY_LISTING_LENGTH);
    intercom_directory_listing_encode(message, intercom_directory_listing);
    return message;
}

void intercom_directory_listing_encode(message_t *message, intercom_directory_listing_message_t *intercom_directory_listing) {
    message->id = 8;
    message->size = 226;
    memcpy((void *)message->data, intercom_directory_listing, sizeof(intercom_directory_listing_message_t));
}

void intercom_directory_listing_decode(message_t *message, intercom_directory_listing_message_t *intercom_directory_listing) {
    memcpy((void *)intercom_directory_listing, (void *)message->data, sizeof(intercom_directory_listing_message_t));
}

void intercom_directory_listing_send(intercom_directory_listing_message_t *intercom_directory_listing) {
    message_t *message = intercom_directory_listing_encode_alloc(intercom_directory_listing);
    message_send(message);
}
