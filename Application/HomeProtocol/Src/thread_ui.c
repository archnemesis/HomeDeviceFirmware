/*
 * thread_ui.c
 *
 *  Created on: Feb 21, 2018
 *      Author: Robin
 */

#include "thread_ui.h"
#include "gfx.h"

static font_t font;
static GHandle ghMainWindow;
static GHandle ghTestLabel;
static GHandle ghTestButton;

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

void UserInterfaceThread_Main(const void * argument)
{
	(void)argument;

	GListener listener;
	GEvent *event;

	font = gdispOpenFont("fixed_7x14");
	gwinSetDefaultFont(font);

	GWidgetInit wi;
	wi.g.show = TRUE;
	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight();
	wi.g.x = 0;
	wi.g.y = 0;
	wi.text = "Main Window";

	ghMainWindow = gwinContainerCreate(0, &wi, 0);

	wi.g.parent = ghMainWindow;
	wi.g.height = 20;
	wi.g.width = gwinGetInnerWidth(ghMainWindow);
	wi.g.x = 0;
	wi.g.y = 0;
	wi.text = "Test Label";

	ghTestLabel = gwinLabelCreate(0, &wi);

	wi.g.height = 30;
	wi.g.x = 0;
	wi.g.y = 30;
	wi.text = "Test Button";

	ghTestButton = gwinButtonCreate(0, &wi);

	geventListenerInit(&listener);
	gwinAttachListener(&listener);

	char buf[16];

	while (1) {
		vTaskDelay(configTICK_RATE_HZ);
		sprintf(buf, "%06d", xTaskGetTickCount() / configTICK_RATE_HZ);
		gwinSetText(ghTestLabel, buf, TRUE);
	}
}
