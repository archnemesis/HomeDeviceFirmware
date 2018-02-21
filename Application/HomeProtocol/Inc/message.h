

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <stdint.h>

enum ErrorCode {
  RequestError = 0,
  RequestDeniedPermissions = 1,
  RequestDeniedUnRegistered = 2,
  RequestFailed = 3
} __attribute__ ((packed));

enum DeviceType {
  Computer = 0,
  WallPanelSmall = 1,
  WallPanelMedium = 2,
  WallPanelLarge = 3,
  Keypad = 4
} __attribute__ ((packed));

enum DeviceUITheme {
  Default = 0,
  Light = 1,
  Dark = 2
} __attribute__ ((packed));


#define MESSAGE_HEADER_SIZE 3
#define MESSAGE_MAX_DATA_SIZE 66
#define MESSAGE_MAX_TOTAL_SIZE (MESSAGE_HEADER_SIZE + MESSAGE_MAX_DATA_SIZE)

struct message {
    uint8_t id;
    uint16_t size;
    uint32_t data[17];
} __attribute__((packed));

typedef struct message message_t;

#include "request_configuration.h"
#include "request_error.h"
#include "intercom_channel_request.h"
#include "intercom_channel_accept.h"
#include "configuration_payload.h"
#include "ping.h"


#endif
