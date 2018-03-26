

#ifndef _MESSAGES_H_
#define _MESSAGES_H_

#include <stdint.h>
#include "FreeRTOS.h"

#if defined ( __GNUC__ ) && !defined (__CC_ARM) /* GNU Compiler */
  #ifndef __weak
    #define __weak   __attribute__((weak))
  #endif /* __weak */
#endif /* __GNUC__ */

enum CommandCode {
  RequestConfigurationCommand = 1,
  PingCommand = 2
} __attribute__ ((packed));

enum ErrorCode {
  RequestError = 1,
  RequestDeniedPermissions = 2,
  RequestDeniedUnRegistered = 3,
  RequestFailed = 4
} __attribute__ ((packed));

enum DeviceType {
  Computer = 1,
  WallPanelSmall = 2,
  WallPanelMedium = 3,
  WallPanelLarge = 4,
  Keypad = 5
} __attribute__ ((packed));

enum DeviceUITheme {
  Default = 1,
  Light = 2,
  Dark = 3
} __attribute__ ((packed));

enum ControlType {
  OnOff = 1,
  Slider = 2,
  Momentary = 3
} __attribute__ ((packed));


#define MESSAGE_HEADER_SIZE 13
#define MESSAGE_MAX_DATA_SIZE 234
#define MESSAGE_MAX_TOTAL_SIZE (MESSAGE_HEADER_SIZE + MESSAGE_MAX_DATA_SIZE)

struct message {
    uint8_t id;
    uint16_t size;
    char hwid[6];
    uint32_t timestamp;
    uint32_t data[59];
} __packed;

typedef struct message message_t;

void message_send(message_t *message);

#include "command.h"
#include "request_error.h"
#include "intercom_channel_request.h"
#include "intercom_channel_accept.h"
#include "configuration_payload.h"
#include "ping.h"
#include "intercom_channel_create.h"

#endif
