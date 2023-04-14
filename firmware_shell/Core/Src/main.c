/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
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
#include "main.h"
#include "cmsis_os.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "shell.h"
#include "semphr.h"
#include "task.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TASK_SHELL_STACK_DEPTH 512
#define TASK_SHELL_PRIORITY 1
#define STACK_SIZE 1000
#define DELAY 1000
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	TaskHandle_t h_task_shell = NULL;
  TaskHandle_t h_task_led = NULL;

  TaskHandle_t h_of = NULL;
  TaskHandle_t h_stat = NULL;

  SemaphoreHandle_t xSemaphore;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void blink_led(void *unsued);
void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void taskoverlfow(void *pvParameters){
  
  int delay = (int) pvParameters;
  char buffer[STACK_SIZE];
  int i;
  
  while (1) {

    for (i = 0; i <= STACK_SIZE+1; i++) {
      buffer[i] = i;
      
      xSemaphoreTake(xSemaphore,portMAX_DELAY);
      printf("Task Overflow running %d\r\n", buffer[i]);

      printf("Je suis la tache 1 et je m'endors pour %d ticks\r\n", delay);
      
      

      vTaskDelay(delay);
  }

}
}

void taskstats(void *pvParameters){

int delay = (int) pvParameters;
static char buffer[1024];

  while(1){ 
    xSemaphoreGive(xSemaphore);

    vTaskGetRunTimeStats(buffer);
    printf("Runtime stats:\n%s", buffer);

    printf("Je suis la tache 2 et je m'endors pour %d ticks\r\n", delay);
    
    vTaskDelay(delay);

  }   

}
int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart1, (uint8_t*)&ch, 1, HAL_MAX_DELAY);

	return ch;
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, signed char *pcTaskName ){

  printf("ERROR: Stack overflow in task %s\n", pcTaskName);

  for(;;){
    
  }
}

void configureTimerForRunTimeStats(void)
{
  HAL_TIM_Base_Start(&htim2);
}

unsigned long getRunTimeCounterValue(void)
{
  return __HAL_TIM_GET_COUNTER(&htim2);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == USART1)
	{
		shell_uart_receive_irq_cb();	// C'est la fonction qui donne le sémaphore!
	}
}

int stat(int argc, char ** argv)
{
  static char buffer[1024];
  static char buffer_i[1024];

	if (argc == 1)
	{
		vTaskGetRunTimeStats(buffer);
    printf("Runtime stats:\n%s", buffer);

	  vTaskList(buffer_i);
    printf("Runtime stats:\n%s", buffer_i);
    

		return 0;
	}
	else
	{
		printf("Erreur, pas le bon nombre d'arguments\r\n");
		return -1;
	}
}

void task_shell(void * unused)
{
	shell_init();
  shell_add('s',stat, "Stats");
	shell_run();	// boucle infinie
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */

  /* Create a taskGive*/
  // xTaskCreate(
  //         taskoverlfow,       // Function to be called
  //         "Overflow",         // Name of task
  //         STACK_SIZE,     // Stack size
  //         (void *)DELAY,  // Parameter to pass to function
  //         1,              // Task priority 0 to configMAX_PRIORITIES - 1 (FreeRTOSConfig.h)
  //         &h_of       // Task handle (allows to find and manipulate the task)
  //         );

  // xTaskCreate(
  //         taskstats,       // Function to be called
  //         "Stat",         // Name of task
  //         STACK_SIZE,     // Stack size
  //         (void *)DELAY,  // Parameter to pass to function
  //         2,              // Task priority 0 to configMAX_PRIORITIES - 1 (FreeRTOSConfig.h)
  //         &h_stat       // Task handle (allows to find and manipulate the task)
  // );

  if (xTaskCreate(task_shell, "Shell", TASK_SHELL_STACK_DEPTH, NULL, 3, &h_task_shell) != pdPASS)
	{
		printf("Error creating task shell\r\n");
		Error_Handler();
	}

  // Create a semaphore
  xSemaphore = xSemaphoreCreateBinary();

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	while (1)
	{
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	}
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
