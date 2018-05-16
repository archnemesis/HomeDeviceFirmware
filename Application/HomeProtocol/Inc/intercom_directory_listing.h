
#ifndef _INTERCOM_DIRECTORY_LISTING_MESSAGE_H_
#define _INTERCOM_DIRECTORY_LISTING_MESSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "message.h"
#include <stdint.h>

#define MESSAGE_INTERCOM_DIRECTORY_LISTING_ID 8
#define MESSAGE_INTERCOM_DIRECTORY_LISTING_LENGTH 226

/**
 * Listing of intercom contact endpoints.
 */
struct intercom_directory_listing_message {
  uint16_t num_entries;
  uint16_t sequence;
  uint16_t total;
  struct {
    char hwid[6];
    char display_name[16];
  } __packed entries[10];
} __packed;

typedef struct intercom_directory_listing_message intercom_directory_listing_message_t;

message_t *intercom_directory_listing_encode_alloc(intercom_directory_listing_message_t *intercom_directory_listing);

/**
 * Encode a message to a buffer, making it ready to send.
 * @param message
 * @param intercom_directory_listing_message
 */
void intercom_directory_listing_encode(message_t *message, intercom_directory_listing_message_t *intercom_directory_listing);

/**
 * Decode a intercom_directory_listing_message stored in a message wrapper.
 * @param message
 * @param intercom_directory_listing_message
 */
void intercom_directory_listing_decode(message_t *message, intercom_directory_listing_message_t *intercom_directory_listing);

/**
 * Shortcut to send a message.
 * @param message
 */
void intercom_directory_listing_send(intercom_directory_listing_message_t *intercom_directory_listing);

#ifdef __cplusplus
}
#endif

#endif
