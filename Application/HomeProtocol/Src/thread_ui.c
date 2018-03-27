/*
 * thread_ui.c
 *
 *  Created on: Feb 21, 2018
 *      Author: Robin
 */

#include "thread_ui.h"
#include "thread_client.h"
#include "thread_intercom.h"
#include "ethernetif.h"
#include "lwip/api.h"
#include "lwip/ip_addr.h"
#include "gfx.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>

static font_t font;
static GHandle ghConsoleWindow;
static GHandle ghMainTabset;
static GHandle ghMainTabsetHomePage;
static GHandle ghMainTabsetIntercomPage;
static GHandle ghMainTabsetNetworkPage;
static GHandle ghMainTabsetConsolePage;
static GHandle ghTestButton;

static GHandle ghDisplayNameLabel;
static GHandle ghDescriptionLabel;

static GHandle ghNetworkAddressLabel;
static GHandle ghNetworkNetmaskLabel;
static GHandle ghNetworkGatewayLabel;

static GHandle ghNetworkConnectedServerLabel;
static GHandle ghNetworkMessagesTxLabel;
static GHandle ghNetworkMessagesRxLabel;

static GHandle ghIntercomContactList;
static GHandle ghIntercomCallButton;
static GHandle ghIntercomTalkButton;
static GHandle ghIntercomVolumeUpButton;
static GHandle ghIntercomVolumeDnButton;

static GHandle ghNotificationWindow;
static GHandle ghNotificationLabel;

QueueHandle_t xConsoleMessageQueue;
TimerHandle_t xUpdateTimer;
TimerHandle_t xNotificationTimer;

void UserInterfaceThread_HideNotification(void);
void UserInterfaceThread_HideNotificationFromTimer(TimerHandle_t xTimer);

const float calib[] = {
		0.00133f,
		0.25124f,
		-15.85558f,
		-0.14418f,
		0.00725f,
		274.49918f
};

bool_t LoadMouseCalibration(unsigned instance, void *data, size_t sz) {
	(void)instance;

	if (sz != sizeof(calib) || instance != 0) {
		return FALSE;
	}

	memcpy(data, (void*)calib, sz);
	return TRUE;
}

bool_t SaveMouseCalibration(unsigned instance, const void *data, size_t sz)
{
	(void)instance;

	if (sz != sizeof(calib) || instance != 0) {
		return FALSE;
	}

	memcpy((void*)calib, data, sz);
	return TRUE;
}

void UserInterfaceThread_Init(void)
{
	xConsoleMessageQueue = xQueueCreate(10, sizeof(char *));
	xNotificationTimer = xTimerCreate(
			"NotificationTimer",
			pdMS_TO_TICKS( 500 ),
			pdFALSE,
			NULL,
			UserInterfaceThread_HideNotificationFromTimer
			);
}

void UserInterfaceThread_WriteConsoleMessage(const char *message)
{

}

void UserInterfaceThread_PrintConsoleString(const char * string, uint8_t copy)
{
	char *str = string;

	if (xConsoleMessageQueue != NULL) {
		if (copy == 1) {
			str = pvPortMalloc(strlen(string) + 1);
			memcpy(str, string, strlen(string) + 1);
		}

		if (xQueueSendToBack(xConsoleMessageQueue, (void *)&str, 0) != pdTRUE) {
			// release the memory if we alloc'd it
			if (copy == 1) {
				vPortFree(str);
			}
		}
	}
}

void UserInterfaceThread_SetupUI(void)
{
	font = gdispOpenFont("DejaVuSans12_aa");
	gwinSetDefaultFont(font);

	GWidgetInit wi;
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;
	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight();
	wi.g.x = 0;
	wi.g.y = 0;

	ghMainTabset = gwinTabsetCreate(0, &wi, 0);
	ghMainTabsetHomePage = gwinTabsetAddTab(ghMainTabset, "Home", FALSE);
	ghMainTabsetIntercomPage = gwinTabsetAddTab(ghMainTabset, "Intercom", FALSE);
	ghMainTabsetNetworkPage = gwinTabsetAddTab(ghMainTabset, "Network", FALSE);
	ghMainTabsetConsolePage = gwinTabsetAddTab(ghMainTabset, "Debug", FALSE);

	/* Debug Console */
	wi.g.height = gwinGetInnerHeight(ghMainTabset);
	wi.g.parent = ghMainTabsetConsolePage;
	ghConsoleWindow = gwinConsoleCreate(0, &wi);
	//gwinConsoleSetBuffer(ghConsoleWindow, TRUE);

	wi.g.x = 5;
	wi.g.y = 5;
	wi.g.parent = ghMainTabsetHomePage;
	wi.g.height = 16;
	wi.g.width = 120;
	wi.text = "Display Name:";
	gwinLabelCreate(0, &wi);

	wi.g.x = 125;
	wi.g.width = gdispGetWidth() - 125;
	wi.text = "[NOT CONFIGURED]";
	ghDisplayNameLabel = gwinLabelCreate(0, &wi);

	wi.g.x = 5;
	wi.g.y = 26;
	wi.g.width = 120;
	wi.text = "Description:";
	gwinLabelCreate(0, &wi);

	wi.g.x = 125;
	wi.g.width = gdispGetWidth() - 125;
	wi.text = "[NOT CONFIGURED]";
	ghDescriptionLabel = gwinLabelCreate(0, &wi);

	wi.g.height = gwinGetInnerHeight(ghMainTabset) / 2;
	wi.g.width = gwinGetInnerWidth(ghMainTabset) / 2;
	wi.g.y = gwinGetInnerHeight(ghMainTabset) / 4;
	wi.g.x = gwinGetInnerWidth(ghMainTabset) / 4;
	wi.g.parent = ghMainTabsetHomePage;
	wi.text = "Configure";
	ghTestButton = gwinButtonCreate(0, &wi);

	wi.g.x = 5;
	wi.g.y = 0;
	wi.g.parent = ghMainTabsetNetworkPage;
	wi.g.width = gwinGetInnerWidth(ghMainTabset) / 4;
	wi.g.height = 15;
	wi.text = "Address:";
	gwinLabelCreate(0, &wi);

	wi.g.x = (gwinGetInnerWidth(ghMainTabset) / 4) + 5;
	wi.text = "0.0.0.0";
	ghNetworkAddressLabel = gwinLabelCreate(0, &wi);

	wi.g.y = 20;
	wi.g.x = 5;
	wi.text = "Netmask:";
	gwinLabelCreate(0, &wi);

	wi.g.x = (gwinGetInnerWidth(ghMainTabset) / 4) + 5;
	wi.text = "0.0.0.0";
	ghNetworkNetmaskLabel = gwinLabelCreate(0, &wi);

	wi.g.y = 40;
	wi.g.x = 5;
	wi.text = "Gateway:";
	gwinLabelCreate(0, &wi);

	wi.g.x = (gwinGetInnerWidth(ghMainTabset) / 4) + 5;
	wi.text = "0.0.0.0";
	ghNetworkGatewayLabel = gwinLabelCreate(0, &wi);

	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.show = FALSE;
	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight();
	wi.g.parent = NULL;
	ghNotificationWindow = gwinContainerCreate(0, &wi, GWIN_CONTAINER_BORDER);

	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.show = TRUE;
	wi.g.parent = ghNotificationWindow;
	wi.customDraw = gwinLabelDrawJustifiedCenter;
	wi.text = "Requesting configuration...";
	ghNotificationLabel = gwinLabelCreate(0, &wi);

	wi.g.x = 0;
	wi.g.y = 0;
	wi.g.width = gwinGetInnerWidth(ghMainTabset) / 2;
	wi.g.height = gwinGetInnerHeight(ghMainTabset);
	wi.g.parent = ghMainTabsetIntercomPage;
	wi.g.show = TRUE;
	wi.customDraw = NULL;
	ghIntercomContactList = gwinListCreate(0, &wi, FALSE);
	gwinListAddItem(ghIntercomContactList, "Robin", TRUE);
	gwinListAddItem(ghIntercomContactList, "Erin", TRUE);

	int half_width = gwinGetInnerWidth(ghMainTabset) / 2;
	int half_btn_width = (half_width - 15) / 2;

	wi.g.x = half_width + 5;
	wi.g.y = 5;
	wi.g.width = half_btn_width;
	wi.g.height = 60;
	wi.g.parent = ghMainTabsetIntercomPage;
	wi.text = "Talk";
	ghIntercomCallButton = gwinButtonCreate(0, &wi);

	wi.g.x = half_width + 5 + half_btn_width + 5;
	wi.g.width = half_btn_width;
	wi.text = "End";
	ghIntercomTalkButton = gwinButtonCreate(0, &wi);
	gwinSetEnabled(ghIntercomTalkButton, FALSE);

	wi.g.x = half_width + 5;
	wi.g.y = 70;
	wi.g.width = half_btn_width;
	wi.g.height = 60;
	wi.g.parent = ghMainTabsetIntercomPage;
	wi.text = "-";
	ghIntercomVolumeDnButton = gwinButtonCreate(0, &wi);

	wi.g.x = half_width + 5 + half_btn_width + 5;
	wi.g.width = half_btn_width;
	wi.text = "+";
	ghIntercomVolumeUpButton = gwinButtonCreate(0, &wi);
}

void UserInterfaceThread_SetDisplayName(const char *name)
{
	gwinSetText(ghDisplayNameLabel, name, TRUE);
}

void UserInterfaceThread_SetDescription(const char *desc)
{
	gwinSetText(ghDescriptionLabel, desc, TRUE);
}

void UserInterfaceThread_SetNetworkAddress(const char *addr)
{
	gwinSetText(ghNetworkAddressLabel, addr, TRUE);
}

void UserInterfaceThread_TimerUpdateCallback(TimerHandle_t xTimer)
{
	static struct netif *netif = NULL;
	char ipbuf[16];

	if (netif == NULL) {
		netif = netif_find("st0");
	}

	if (netif_is_link_up(netif)) {
		ip4addr_ntoa_r(&netif->ip_addr, ipbuf, 16);
		gwinSetText(ghNetworkAddressLabel, ipbuf, TRUE);
		ip4addr_ntoa_r(&netif->netmask, ipbuf, 16);
		gwinSetText(ghNetworkNetmaskLabel, ipbuf, TRUE);
		ip4addr_ntoa_r(&netif->gw, ipbuf, 16);
		gwinSetText(ghNetworkGatewayLabel, ipbuf, TRUE);
	}
	else {
		gwinSetText(ghNetworkAddressLabel, "0.0.0.0", TRUE);
		gwinSetText(ghNetworkNetmaskLabel, "0.0.0.0", TRUE);
		gwinSetText(ghNetworkGatewayLabel, "0.0.0.0", TRUE);
	}
}

void UserInterfaceThread_ShowNotification(const char *message, int timeout)
{
	gwinSetText(ghNotificationLabel, message, TRUE);
	gwinSetEnabled(ghMainTabset, FALSE);
	gwinSetVisible(ghMainTabset, FALSE);
	gwinSetVisible(ghNotificationWindow, TRUE);
	gwinSetEnabled(ghNotificationWindow, TRUE);

	if (timeout > 0) {
		xTimerChangePeriod(xNotificationTimer, pdMS_TO_TICKS(timeout), 0);
	}
}

void UserInterfaceThread_HideNotification(void)
{
	gwinSetEnabled(ghNotificationWindow, FALSE);
	gwinSetVisible(ghNotificationWindow, FALSE);
	gwinSetEnabled(ghMainTabset, TRUE);
	gwinSetVisible(ghMainTabset, TRUE);
}

void UserInterfaceThread_HideNotificationFromTimer(TimerHandle_t xTimer)
{
	UserInterfaceThread_HideNotification();
}

void UserInterfaceThread_Main(const void * argument)
{
	(void)argument;

	GListener listener;
	GEvent *event;

	struct netif *netif = netif_find("st0");
	ip4_addr_t remote;

	ipaddr_aton("10.1.1.143", &remote);

	UserInterfaceThread_SetupUI();

	xUpdateTimer = xTimerCreate("UpdateTimer", pdMS_TO_TICKS(500), pdTRUE, NULL, UserInterfaceThread_TimerUpdateCallback);
	if (xUpdateTimer != NULL) {
		xTimerStart(xUpdateTimer, 0);
	}

	geventListenerInit(&listener);
	gwinAttachListener(&listener);

	char *buf;

	while (1) {
		if (pdTRUE == xQueueReceive(xConsoleMessageQueue, &buf, 0)) {
			gwinPutString(ghConsoleWindow, buf);
			vPortFree(buf);
		}

		event = geventEventWait(&listener, 10);
		switch (event->type) {
		case GEVENT_GWIN_BUTTON:
		{
			GEventGWinButton *btnevent = (GEventGWinButton *)event;

			if (btnevent->gwin == ghTestButton) {
				UserInterfaceThread_ShowNotification("Requesting configuration...", 2000);
				gwinPutString(ghConsoleWindow, "Requesting configuration\n");
				command_message_t cmd;
				cmd.command_id = RequestConfigurationCommand;
				command_send(&cmd);
			}
			else if (btnevent->gwin == ghIntercomCallButton) {
				UserInterfaceThread_ShowNotification("Opening intercom channel...", 2000);
				IntercomThread_ChannelOpen((in_addr_t)remote.addr, 2050);
				gwinSetEnabled(ghIntercomCallButton, FALSE);
				gwinSetEnabled(ghIntercomTalkButton, TRUE);
			}
			else if (btnevent->gwin == ghIntercomTalkButton) {
				UserInterfaceThread_ShowNotification("Closing intercom channel...", 2000);
				IntercomThread_ChannelClose();
				gwinSetEnabled(ghIntercomCallButton, TRUE);
				gwinSetEnabled(ghIntercomTalkButton, FALSE);
			}
			else if (btnevent->gwin == ghIntercomVolumeUpButton) {
				IntercomThread_VolumeUp();
			}
			else if (btnevent->gwin == ghIntercomVolumeDnButton) {
				IntercomThread_VolumeDn();
			}
			break;
		}
		default:
			break;
		}
		geventEventComplete(&listener);
	}
}
