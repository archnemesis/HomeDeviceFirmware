/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     

#include "adc.h"
#include "gpio.h"
#include "ltdc.h"
#include "tim.h"
#include "thread_client.h"
#include "thread_ui.h"
#include "thread_intercom.h"
#include "gfx.h"

#include <string.h>

/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId adcSampleTaskHandle;

/* USER CODE BEGIN Variables */

osThreadId clientTaskHandle;
osThreadId uiTaskHandle;
osThreadId intercomTaskHandle;

QueueHandle_t xSampleQueue;
QueueHandle_t xBufferQueue;
const int iBufferCount = 4;
const int iBufferSize = 128;

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void StartAdcSampleTask(void const * argument);

extern void MX_LWIP_Init(void);
extern void MX_USB_DEVICE_Init(void);
extern void MX_MBEDTLS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{
  /* MX_LWIP_Init() is generated within mbedtls_net_init() function in net_cockets.c file */
  /* Up to user to call mbedtls_net_init() function in MBEDTLS initialization step */

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();

  /* Up to user define the empty MX_MBEDTLS_Init() function located in mbedtls.c file */
  MX_MBEDTLS_Init();

  /* USER CODE BEGIN StartDefaultTask */

  memset((void *)0xD0000000, 0xFFFFFFFF, 0x800000);

  HAL_GPIO_WritePin(LCD_BLEN_GPIO_Port, LCD_BLEN_Pin, GPIO_PIN_SET);

  gfxInit();
  ClientThread_Init();
  UserInterfaceThread_Init();
  IntercomThread_Init();

  /* definition and creation of adcSampleTask */
  osThreadDef(clientTask, ClientThread_Main, osPriorityNormal, 0, 2048);
  clientTaskHandle = osThreadCreate(osThread(clientTask), NULL);

  /* definition and creation of adcSampleTask */
  osThreadDef(uiTask, UserInterfaceThread_Main, osPriorityNormal, 0, 512);
  uiTaskHandle = osThreadCreate(osThread(uiTask), NULL);

  /* intercom task */
  osThreadDef(intercomTask, IntercomThread_Main, osPriorityNormal, 0, 1024);
  intercomTaskHandle = osThreadCreate(osThread(intercomTask), NULL);

  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* StartAdcSampleTask function */
void StartAdcSampleTask(void const * argument)
{
  /* USER CODE BEGIN StartAdcSampleTask */

	xSampleQueue = xQueueCreate(iBufferSize, sizeof(uint16_t));

	int bufferIndex = 0;
	uint16_t *activeBuffer = 0;
	uint16_t sample;

	for(;;)
	{
		if (xSampleQueue != 0) {
			if (xQueueReceive(xSampleQueue, (uint16_t *)&sample, portMAX_DELAY)) {
				if (bufferIndex == 0) {
					activeBuffer = pvPortMalloc(iBufferSize * sizeof(uint16_t));
				}

				// we got a sample!
				HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);
				activeBuffer[bufferIndex++] = sample;

				if (bufferIndex == iBufferSize) {
					// put the buffer on the network frame queue
					vPortFree(activeBuffer);
					bufferIndex = 0;
				}
			}
		}
	}
  /* USER CODE END StartAdcSampleTask */
}

/* USER CODE BEGIN Application */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	int16_t val = 0;
	BaseType_t xHigherPriorityTaskWoken;

	HAL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin);
	val = (int16_t)((int32_t)HAL_ADC_GetValue(&hadc1) - 32767);

	xQueueSendToBackFromISR(xSampleQueue, (uint16_t *)&val, &xHigherPriorityTaskWoken);

	if (xHigherPriorityTaskWoken) {
		portYIELD_FROM_ISR(pdTRUE);
	}
}
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
