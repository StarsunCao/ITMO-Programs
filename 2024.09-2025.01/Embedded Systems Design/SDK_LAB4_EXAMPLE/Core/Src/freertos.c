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
osThreadId task3Handle;
osSemaphoreId semTask1Handle;
osSemaphoreId semTask2Handle;
osSemaphoreId semTask3Handle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartTask1(void const * argument);
void StartTask2(void const * argument);
void StartTask3(void const * argument);

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
	  osSemaphoreDef(semTask1);
	  semTask1Handle = osSemaphoreCreate(osSemaphore(semTask1), 1);
	  osSemaphoreWait(semTask1Handle, 0);  // 初始化为0

	  osSemaphoreDef(semTask2);
	  semTask2Handle = osSemaphoreCreate(osSemaphore(semTask2), 1);
	  osSemaphoreWait(semTask2Handle, 0);  // 初始化为0

	  osSemaphoreDef(semTask3);
	  semTask3Handle = osSemaphoreCreate(osSemaphore(semTask3), 1);
	  osSemaphoreWait(semTask3Handle, 0);  // 初始化为0

	  osThreadDef(task1, StartTask1, osPriorityNormal, 0, 128);
	  task1Handle = osThreadCreate(osThread(task1), NULL);

	  osThreadDef(task2, StartTask2, osPriorityNormal, 0, 128);
	  task2Handle = osThreadCreate(osThread(task2), NULL);

	  osThreadDef(task3, StartTask3, osPriorityNormal, 0, 128);
	  task3Handle = osThreadCreate(osThread(task3), NULL);

	  osSemaphoreRelease(semTask1Handle);  // 启动任务1

	  osKernelStart();
}

void BlinkLED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, int times, int delayMs) {
    for (int i = 0; i < times; i++) {
        HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
        HAL_Delay(delayMs);
        HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
        HAL_Delay(delayMs);
    }
}

/* USER CODE BEGIN Header_StartTask1 */
/**
  * @brief  Function implementing the tast1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask1 */
void StartTask1(void const * argument)
{
  /* USER CODE BEGIN StartTask1 */
  /* Infinite loop */
  for(;;)
  {
	    osSemaphoreWait(semTask1Handle, osWaitForever);
	    BlinkLED(RED_LED_GPIO_Port, RED_LED_Pin, 3, 500);
	    osSemaphoreRelease(semTask2Handle);
  }
  /* USER CODE END StartTask1 */
}

/* USER CODE BEGIN Header_StartTask2 */
/**
* @brief Function implementing the task2 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask2 */
void StartTask2(void const * argument)
{
  /* USER CODE BEGIN StartTask2 */
  /* Infinite loop */
  for(;;)
  {
	    osSemaphoreWait(semTask2Handle, osWaitForever);
	    BlinkLED(YELLOW_LED_GPIO_Port, YELLOW_LED_Pin, 3, 500);
	    osSemaphoreRelease(semTask3Handle);
  }
  /* USER CODE END StartTask2 */
}

/* USER CODE BEGIN Header_StartTask3 */
/**
* @brief Function implementing the task3 thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask3 */
void StartTask3(void const * argument)
{
  /* USER CODE BEGIN StartTask3 */
  /* Infinite loop */
  for(;;)
  {
	    osSemaphoreWait(semTask3Handle, osWaitForever);
	    BlinkLED(GREEN_LED_GPIO_Port, GREEN_LED_Pin, 4, 500);
	    osSemaphoreRelease(semTask1Handle);
  }
  /* USER CODE END StartTask3 */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
