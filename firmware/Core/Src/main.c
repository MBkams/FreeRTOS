/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define STACK_SIZE 1000
#define Delay 1000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Semaphore */
SemaphoreHandle_t Semaphore;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void LedBlinker (void * pvParameters);

void taskGive(void *pvParameters);
void taskTake(void *unsued);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



/* Fonction Give*/
void taskGive(void *pvParameters) {

    int delay = (int) pvParameters;
    uint8_t uartTxBuffer[100];
    char* s = pcTaskGetName(xTaskGetCurrentTaskHandle());


    for(;;)
    {

      //Verification creation semaphore
      if ( Semaphore != NULL)
      {
        // Affichage sur console
        sprintf((char*)uartTxBuffer, "Je suis la tache %s avant de donner le semaphore\n", s);
        HAL_UART_Transmit(&huart1, uartTxBuffer, strlen((char*)uartTxBuffer), HAL_MAX_DELAY);

        //Prise du semaphore
        xSemaphoreGive(Semaphore);

        //Verification avec la LED
        HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_RESET);

        /* Vérification du status binaire du semaphore*/
        if(uxSemaphoreGetCount(Semaphore) > 0)
        {
          sprintf((char*)uartTxBuffer, "Je suis la tache %s apres avoir donner semaphore tous les %d ms\n", s, delay);
          HAL_UART_Transmit(&huart1, uartTxBuffer, strlen((char*)uartTxBuffer), HAL_MAX_DELAY);

          
        }
        
        //Delais
        vTaskDelay(delay/portTICK_PERIOD_MS);

      }
      else
      {
        char *strsem = "Erreur : Semaphore non cree\n" ;
        HAL_UART_Transmit(&huart1, strsem, strlen(strsem), HAL_MAX_DELAY);

        vTaskDelay(1000/portTICK_PERIOD_MS);
        HAL_NVIC_SystemReset();

      }
      
    }

}

void taskTake(void *unsued) {

    uint8_t uartTxBuffer[100];
    char* s = pcTaskGetName(xTaskGetCurrentTaskHandle());

    for(;;)
    {

      //Verification creation semaphore
      if ( Semaphore != NULL)
      {
        // Affichage sur console
        sprintf((char*)uartTxBuffer, "Je suis la tache %s avant de prendre le semaphore\n", s);
        HAL_UART_Transmit(&huart1, uartTxBuffer, strlen((char*)uartTxBuffer), HAL_MAX_DELAY);

        //Prise du semaphore
        xSemaphoreTake(Semaphore,HAL_MAX_DELAY);

        //Verification avec la LED
        HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,GPIO_PIN_SET);

        /* Vérification du status binaire du semaphore*/
        if(uxSemaphoreGetCount(Semaphore) > 0)
        {
          sprintf((char*)uartTxBuffer, "Je suis la tache %s apres avoir pris le semaphore \n", s);
          HAL_UART_Transmit(&huart1, uartTxBuffer, strlen((char*)uartTxBuffer), HAL_MAX_DELAY);

          
        }

      }
      else
      {
        char *strsem = "Erreur : Semaphore non cree\n" ;
        HAL_UART_Transmit(&huart1, strsem, strlen(strsem), HAL_MAX_DELAY);

        vTaskDelay(1000/portTICK_PERIOD_MS);
        HAL_NVIC_SystemReset();
    }
  }
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  BaseType_t xReturned;
  TaskHandle_t xHandle1 = NULL;

  TaskHandle_t xHandlegive = NULL;
  TaskHandle_t xHandletake = NULL;
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
  /* USER CODE BEGIN 2 */

  // Create binary semaphore before starting task
  Semaphore = xSemaphoreCreateBinary(); 

  /* Create a taskGive*/
  xTaskCreate(
          taskGive,       // Function to be called
          "Give",         // Name of task
          STACK_SIZE,     // Stack size
          (void*) Delay,  // Parameter to pass to function
          1,              // Task priority 0 to configMAX_PRIORITIES - 1 (FreeRTOSConfig.h)
          &xHandlegive       // Task handle (allows to find and manipulate the task)
          );

  /* Create a taskTake*/
  xTaskCreate(
          taskTake,       // Function to be called
          "Take",         // Name of task
          STACK_SIZE,     // Stack size
          NULL,           // Parameter to pass to function
          2,              // Task priority 0 to configMAX_PRIORITIES - 1 (FreeRTOSConfig.h)
          &xHandletake       // Task handle (allows to find and manipulate the task)
          );

  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* Redirect stdout to UART */

  while (1)
  {
    // HAL_Delay(1000);
    // HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
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
