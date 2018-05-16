/*
 * thread_client.c
 *
 *  Created on: Feb 20, 2018
 *      Author: Robin
 */

#include "thread_client.h"
#include "ethernetif.h"
#include "lwip/api.h"
#include "lwip/ip_addr.h"

#include "mbedtls/debug.h"
#include "mbedtls/ssl.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"

#include <string.h>

#include "parser.h"
#include "thread_ui.h"

void ClientThread_ParserCallback(message_t * message);

const size_t MESSAGE_BUFFER_SIZE = sizeof(message_t) + 2;

QueueHandle_t xQueueOutMessages;
ParserContext_t parser;

const char CACERT[] = "-----BEGIN CERTIFICATE-----\r\n"
    "MIICQDCCAcWgAwIBAgIJAJvz0HNKEO7bMAoGCCqGSM49BAMCMFwxCzAJBgNVBAYT\r\n"
    "AlVTMQ8wDQYDVQQIDAZPcmVnb24xEjAQBgNVBAcMCUhpbGxzYm9ybzEWMBQGA1UE\r\n"
    "CgwNUm9iaW4gR2luZ3JhczEQMA4GA1UECwwHQ2VudHJhbDAeFw0xODAzMTcyMjUy\r\n"
    "MTVaFw0xOTAzMTcyMjUyMTVaMFwxCzAJBgNVBAYTAlVTMQ8wDQYDVQQIDAZPcmVn\r\n"
    "b24xEjAQBgNVBAcMCUhpbGxzYm9ybzEWMBQGA1UECgwNUm9iaW4gR2luZ3JhczEQ\r\n"
    "MA4GA1UECwwHQ2VudHJhbDB2MBAGByqGSM49AgEGBSuBBAAiA2IABCf3KeOEkT1i\r\n"
    "yKPzrDAckZUrQc409KYkE5IgetNz061DZQU75Gn6pldGnud0S1iim4HBf2IhQzbi\r\n"
    "badzZJEVcIDoolOPo5uKYHzLGhlW2JP5BGNI3uBT0X8VpCF2+YhLpKNTMFEwHQYD\r\n"
    "VR0OBBYEFOON2/n1+a89uuN5q0M7/oe1TWn1MB8GA1UdIwQYMBaAFOON2/n1+a89\r\n"
    "uuN5q0M7/oe1TWn1MA8GA1UdEwEB/wQFMAMBAf8wCgYIKoZIzj0EAwIDaQAwZgIx\r\n"
    "AIw2jqNqaJ9In98LO6ez7AvCFadk0ly3+s7XLP8soimig8TIZxn4NIhYB9RsHCex\r\n"
    "nwIxAJGoej1KdWPjJZFrkaNFClQqznImSNe3QGkKinUAhcI4TlAiUz2xmpDkf75m\r\n"
    "un0opg==\r\n"
    "-----END CERTIFICATE-----\r\n";

void ClientThread_Init(void)
{
  xQueueOutMessages = xQueueCreate(5, sizeof(message_t *));
  HomeProtocol_ParserInit(&parser);
  HomeProtocol_ParserSetCallback(&parser, ClientThread_ParserCallback);
}

void ClientThread_SendMessage(message_t *message)
{
  if (xQueueOutMessages != NULL) {
    if (pdFALSE
        == xQueueSendToBack(xQueueOutMessages, (void * )&message, portMAX_DELAY)) {
      vPortFree(message);
    }
  }
}

void message_send(message_t *message)
{
  static struct netif *netif = NULL;

  if (netif == NULL)
    netif = netif_find("st0");

  message->hwid[0] = netif->hwaddr[0];
  message->hwid[1] = netif->hwaddr[1];
  message->hwid[2] = netif->hwaddr[2];
  message->hwid[3] = netif->hwaddr[3];
  message->hwid[4] = netif->hwaddr[4];
  message->hwid[5] = netif->hwaddr[5];
  message->timestamp = 0;

  ClientThread_SendMessage(message);
}

void ClientThread_TimerUpdateCallback(TimerHandle_t xTimer)
{

}

void ClientThread_ParserCallback(message_t * message)
{
  switch (message->id) {
  case MESSAGE_REQUEST_ERROR_ID: {
    request_error_message_t err;
    request_error_decode(message, (request_error_message_t *) &err);
    switch (err.code) {
    case RequestError:
      // TODO: handle request error
      UserInterfaceThread_PrintConsoleString("Got an error!\n", 1);
      break;
    default:
      // TODO: handle unknown error
      break;
    }

    break;
  }
  case MESSAGE_CONFIGURATION_PAYLOAD_ID: {
    configuration_payload_message_t payload;
    configuration_payload_decode(message, &payload);
    UserInterfaceThread_PrintConsoleString("Got configuration payload\n", 1);
    UserInterfaceThread_SetDisplayName(payload.display_name);
    UserInterfaceThread_SetDescription(payload.description);

    break;
  }
  case MESSAGE_INTERCOM_DIRECTORY_LISTING_ID: {
    intercom_directory_listing_message_t listing;
    intercom_directory_listing_decode(message, &listing);
    UserInterfaceThread_UpdateContactList(&listing);

    break;
  }
  case MESSAGE_INTERCOM_INCOMING_CHANNEL_REQUEST_ID: {
    intercom_incoming_channel_request_message_t request;
    intercom_incoming_channel_request_decode(message, &request);
    UserInterfaceThread_RequestIntercomChannel(&request);
    break;
  }
  default:
    break;
  }

  vPortFree(message);
}

void ethernetif_notify_conn_changed(struct netif *netif)
{
  if (netif_is_link_up(netif)) {
    __BKPT();
  }
  else {
    __BKPT();
  }
}

void ClientThread_Main(const void * argument)
{
  (void) argument;

  message_t *message;
  message_t *in;
  uint32_t bytes_remaining = 0;

  int ret;
  const char *pers = "ssl_client1";
  uint32_t flags;
  unsigned char *buffer_out;	// temp outgoing message buffer
  unsigned char *buffer_in;   // temp incoming message buffer
  unsigned char *ptr;			// temp message buffer pointer

  buffer_out = pvPortMalloc(MESSAGE_BUFFER_SIZE);
  if (buffer_out == NULL) {
    Error_Handler();
  }

  buffer_in = pvPortMalloc(MESSAGE_BUFFER_SIZE);
  if (buffer_in == NULL) {
    Error_Handler();
  }

  message = pvPortMalloc(sizeof(message_t));
  if (message == NULL) {
    Error_Handler();
  }

  mbedtls_net_context ssl_fd;
  mbedtls_entropy_context entropy;
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_ssl_context ssl;
  mbedtls_ssl_config conf;
  mbedtls_x509_crt cacert;

  struct netif *netif = netif_find("st0");
  struct netconn *conn = NULL;
  ip4_addr_t remote;

  TickType_t timeout_mark = 0;

  /* Initialize and configure SSL connection */
  mbedtls_ssl_init(&ssl);
  mbedtls_ssl_config_init(&conf);
  mbedtls_x509_crt_init(&cacert);
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_entropy_init(&entropy);

  /* Random Bit Generator */
  ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
      (const unsigned char *) pers, strlen(pers));
  if (ret != 0) {
    // TODO: Handle Error!
    Error_Handler();
  }

  /* Load CA Certificate */
  ret = mbedtls_x509_crt_parse(&cacert, (const unsigned char *) CACERT,
      sizeof(CACERT));

  if (ret < 0) {
    (void) ret;
    Error_Handler();
  }

  ret = mbedtls_ssl_config_defaults(&conf,
  MBEDTLS_SSL_IS_CLIENT,
  MBEDTLS_SSL_TRANSPORT_STREAM,
  MBEDTLS_SSL_PRESET_DEFAULT);

  if (ret != 0) {
    Error_Handler();
  }

  mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
  mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
  mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);
  mbedtls_ssl_conf_read_timeout(&conf, 100);

  ipaddr_aton(HOMESERVER_HOST, &remote);

  while (1) {
    ret = mbedtls_net_connect(&ssl_fd, HOMESERVER_HOST, HOMESERVER_PORT,
        MBEDTLS_NET_PROTO_TCP);

    if (ret == 0) {
      mbedtls_net_set_nonblock(&ssl_fd);
      ret = mbedtls_ssl_setup(&ssl, &conf);

      if (ret != 0) {
        Error_Handler();
      }

      ret = mbedtls_ssl_set_hostname(&ssl, "homeserver.robingingras.com");

      if (ret != 0) {
        Error_Handler();
      }

      mbedtls_ssl_set_bio(&ssl, &ssl_fd, mbedtls_net_send, NULL,
          mbedtls_net_recv_timeout);

      while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ
            && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
          mbedtls_ssl_close_notify(&ssl);
          goto finished_connection;
        }
      }

      flags = mbedtls_ssl_get_verify_result(&ssl);

      if (flags != 0) {
        Error_Handler();
      }

      HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_SET);

      command_message_t cmd;
      cmd.command_id = RequestConfigurationCommand;
      command_send(&cmd);

      while (1) {
        if (bytes_remaining == 0) {
          if (pdTRUE == xQueueReceive(xQueueOutMessages, (void * )&in, 0)) {
            memcpy((void *) buffer_out, "AE", 2);
            memcpy((void *) (buffer_out + 2), (void *) in,
                MESSAGE_HEADER_SIZE + in->size);
            bytes_remaining = MESSAGE_HEADER_SIZE + in->size + 2;

            vPortFree(in);
          }
        }

        ptr = buffer_out;
        while (bytes_remaining > 0) {
          ret = mbedtls_ssl_write(&ssl, ptr, bytes_remaining);

          if (ret > 0) {
            bytes_remaining -= ret;
            ptr += ret;
            timeout_mark = xTaskGetTickCount();
          }
          else if (ret != MBEDTLS_ERR_SSL_WANT_WRITE
              && ret != MBEDTLS_ERR_SSL_WANT_READ) {
            // some failure
            mbedtls_ssl_close_notify(&ssl);
            goto finished_connection;
          }
          else if (ret == MBEDTLS_ERR_SSL_WANT_READ
              || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            break;
          }
        }

        ret = mbedtls_ssl_read(&ssl, buffer_in, MESSAGE_BUFFER_SIZE);

        if (ret > 0) {
          HomeProtocol_ParserProcessBytes(&parser, buffer_in, ret);
          timeout_mark = xTaskGetTickCount();
        }
        else if (ret == MBEDTLS_ERR_SSL_WANT_READ
            || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
          continue;
        }
        else if (ret == MBEDTLS_ERR_SSL_TIMEOUT) {
          if ((xTaskGetTickCount() - timeout_mark)
              > (10 * configTICK_RATE_HZ)) {
            ping_message_t ping;
            ping.timestamp = 1;
            ping_send(&ping);
          }
          continue;
        }
        else if (ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) {
          mbedtls_ssl_close_notify(&ssl);
          goto finished_connection;
          break;
        }
      }
    }

    finished_connection: ;

    /*
     * Connection failed or was dropped, so we wait a second
     * and try to reconnect.
     */
    mbedtls_net_free(&ssl_fd);
    mbedtls_ssl_session_reset(&ssl);
    vTaskDelay(configTICK_RATE_HZ);
  }
}
