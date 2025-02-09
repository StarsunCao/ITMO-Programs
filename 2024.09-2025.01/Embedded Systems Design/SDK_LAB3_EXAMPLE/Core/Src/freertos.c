/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
osThreadId task1Handle;
osThreadId task2Handle;
osThreadId mainTaskHandle;
osMessageQId queue1Handle;
osMessageQId queue2Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void task1_func(void const * argument);
void task2_func(void const * argument);
void mainTask_func(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
	  /* Create the queue(s) */
	  osMessageQDef(queue1, 16, uint8_t);
	  queue1Handle = osMessageCreate(osMessageQ(queue1), NULL);

	  osMessageQDef(queue2, 16, uint8_t);
	  queue2Handle = osMessageCreate(osMessageQ(queue2), NULL);

	  /* Create the thread(s) */
	  osThreadDef(task1, task1_func, osPriorityNormal, 0, 128);
	  task1Handle = osThreadCreate(osThread(task1), NULL);

	  osThreadDef(task2, task2_func, osPriorityNormal, 0, 128);
	  task2Handle = osThreadCreate(osThread(task2), NULL);

	  osThreadDef(mainTask, mainTask_func, osPriorityHigh, 0, 128);
	  mainTaskHandle = osThreadCreate(osThread(mainTask), NULL);

	  osKernelStart();
}

/* Main Task */
void mainTask_func(void const * argument) {
  uint8_t blinkCount;
  osEvent event;
  osDelay(400);

  for (;;) {
    /* Check Task 1 Queue */
    event = osMessageGet(queue1Handle, 0);
    if (event.status == osEventMessage) {
      blinkCount = (uint8_t) event.value.v;
      for (uint8_t i = 0; i < blinkCount; i++) {
        HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
        osDelay(400);
        HAL_GPIO_TogglePin(RED_LED_GPIO_Port, RED_LED_Pin);
        osDelay(400);
      }
    }

    /* Check Task 2 Queue */
    event = osMessageGet(queue2Handle, 0);
    if (event.status == osEventMessage) {
      blinkCount = (uint8_t) event.value.v;
      for (uint8_t i = 0; i < blinkCount; i++) {
        HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
        osDelay(400);
        HAL_GPIO_TogglePin(GREEN_LED_GPIO_Port, GREEN_LED_Pin);
        osDelay(400);
      }
    }
  }
}

/* Task 1 */
void task1_func(void const * argument) {
  uint8_t redSequence[] = {1, 2, 3}; // Red LED blink sequence
  uint8_t index = 0;

  for (;;) {
    osMessagePut(queue1Handle, redSequence[index], osWaitForever);
    index = (index + 1) % (sizeof(redSequence) / sizeof(redSequence[0]));
    osDelay(400);
  }
}

/* Task 2 */
void task2_func(void const * argument) {
  uint8_t greenSequence[] = {3, 2, 1, 4}; // Green LED blink sequence
  uint8_t index = 0;

  for (;;) {
    osMessagePut(queue2Handle, greenSequence[index], osWaitForever);
    index = (index + 1) % (sizeof(greenSequence) / sizeof(greenSequence[0]));
    osDelay(400);
  }
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
