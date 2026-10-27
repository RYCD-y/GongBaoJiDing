/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
osThreadId defaultTaskHandle;
osThreadId Name_Gimbal_TaskHandle;
osThreadId Name_Chassis_TaskHandle;
osThreadId Name_Shoot_TaskHandle;
osThreadId Name_Communicate_TaskHandle;
osThreadId Name_INS_TaskHandle;
osThreadId Name_RoboControl_TaskHandle;
osThreadId Name_UI_TaskHandle;
osThreadId Name_Buzzer_TaskHandle;
osThreadId Name_Key_TaskHandle;
osThreadId Name_LED_TaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void Gimbal_Task(void const * argument);
void Chassis_Task(void const * argument);
void Shoot_Task(void const * argument);
void Communicate_Task(void const * argument);
void INS_Task(void const * argument);
void RoboControl_Task(void const * argument);
void UI_Task(void const * argument);
void Buzzer_Task(void const * argument);
void Key_Task(void const * argument);
void LED_Task(void const * argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

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

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
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

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of Name_Gimbal_Task */
  osThreadDef(Name_Gimbal_Task, Gimbal_Task, osPriorityLow, 0, 512);
  Name_Gimbal_TaskHandle = osThreadCreate(osThread(Name_Gimbal_Task), NULL);

  /* definition and creation of Name_Chassis_Task */
  osThreadDef(Name_Chassis_Task, Chassis_Task, osPriorityLow, 0, 512);
  Name_Chassis_TaskHandle = osThreadCreate(osThread(Name_Chassis_Task), NULL);

  /* definition and creation of Name_Shoot_Task */
  osThreadDef(Name_Shoot_Task, Shoot_Task, osPriorityLow, 0, 512);
  Name_Shoot_TaskHandle = osThreadCreate(osThread(Name_Shoot_Task), NULL);

  /* definition and creation of Name_Communicate_Task */
  osThreadDef(Name_Communicate_Task, Communicate_Task, osPriorityLow, 0, 512);
  Name_Communicate_TaskHandle = osThreadCreate(osThread(Name_Communicate_Task), NULL);

  /* definition and creation of Name_INS_Task */
  osThreadDef(Name_INS_Task, INS_Task, osPriorityNormal, 0, 1024);
  Name_INS_TaskHandle = osThreadCreate(osThread(Name_INS_Task), NULL);

  /* definition and creation of Name_RoboControl_Task */
  osThreadDef(Name_RoboControl_Task, RoboControl_Task, osPriorityLow, 0, 512);
  Name_RoboControl_TaskHandle = osThreadCreate(osThread(Name_RoboControl_Task), NULL);

  /* definition and creation of Name_UI_Task */
  osThreadDef(Name_UI_Task, UI_Task, osPriorityIdle, 0, 512);
  Name_UI_TaskHandle = osThreadCreate(osThread(Name_UI_Task), NULL);

  /* definition and creation of Name_Buzzer_Task */
  osThreadDef(Name_Buzzer_Task, Buzzer_Task, osPriorityIdle, 0, 256);
  Name_Buzzer_TaskHandle = osThreadCreate(osThread(Name_Buzzer_Task), NULL);

  /* definition and creation of Name_Key_Task */
  osThreadDef(Name_Key_Task, Key_Task, osPriorityIdle, 0, 256);
  Name_Key_TaskHandle = osThreadCreate(osThread(Name_Key_Task), NULL);

  /* definition and creation of Name_LED_Task */
  osThreadDef(Name_LED_Task, LED_Task, osPriorityIdle, 0, 256);
  Name_LED_TaskHandle = osThreadCreate(osThread(Name_LED_Task), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_Gimbal_Task */
/**
* @brief Function implementing the Name_Gimbal_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Gimbal_Task */
void Gimbal_Task(void const * argument)
{
  /* USER CODE BEGIN Gimbal_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Gimbal_Task */
}

/* USER CODE BEGIN Header_Chassis_Task */
/**
* @brief Function implementing the Name_Chassis_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Chassis_Task */
void Chassis_Task(void const * argument)
{
  /* USER CODE BEGIN Chassis_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Chassis_Task */
}

/* USER CODE BEGIN Header_Shoot_Task */
/**
* @brief Function implementing the Name_Shoot_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Shoot_Task */
void Shoot_Task(void const * argument)
{
  /* USER CODE BEGIN Shoot_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Shoot_Task */
}

/* USER CODE BEGIN Header_Communicate_Task */
/**
* @brief Function implementing the Name_Communicate_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Communicate_Task */
void Communicate_Task(void const * argument)
{
  /* USER CODE BEGIN Communicate_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Communicate_Task */
}

/* USER CODE BEGIN Header_INS_Task */
/**
* @brief Function implementing the Name_INS_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_INS_Task */
void INS_Task(void const * argument)
{
  /* USER CODE BEGIN INS_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END INS_Task */
}

/* USER CODE BEGIN Header_RoboControl_Task */
/**
* @brief Function implementing the Name_RoboControl_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_RoboControl_Task */
void RoboControl_Task(void const * argument)
{
  /* USER CODE BEGIN RoboControl_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END RoboControl_Task */
}

/* USER CODE BEGIN Header_UI_Task */
/**
* @brief Function implementing the Name_UI_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_UI_Task */
void UI_Task(void const * argument)
{
  /* USER CODE BEGIN UI_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END UI_Task */
}

/* USER CODE BEGIN Header_Buzzer_Task */
/**
* @brief Function implementing the Name_Buzzer_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Buzzer_Task */
void Buzzer_Task(void const * argument)
{
  /* USER CODE BEGIN Buzzer_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Buzzer_Task */
}

/* USER CODE BEGIN Header_Key_Task */
/**
* @brief Function implementing the Name_Key_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Key_Task */
void Key_Task(void const * argument)
{
  /* USER CODE BEGIN Key_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Key_Task */
}

/* USER CODE BEGIN Header_LED_Task */
/**
* @brief Function implementing the Name_LED_Task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_LED_Task */
void LED_Task(void const * argument)
{
  /* USER CODE BEGIN LED_Task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END LED_Task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
