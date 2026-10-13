/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "stm32_ssd1315.h"
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
I2C_HandleTypeDef hi2c1;
DMA_HandleTypeDef hdma_i2c1_tx;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
char uart_rx_buf[128];
char uart_tx_buf[128];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */
void uart_send(const char *fmt, ...){
  va_list args;
  va_start(args, fmt);
  vsnprintf(uart_tx_buf, sizeof(uart_tx_buf), fmt, args);
  va_end(args);
  HAL_UART_Transmit(&huart1, (uint8_t*)uart_tx_buf, strlen(uart_tx_buf), 1000);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  uart_send("System start\r\n");
  HAL_Delay(1000);

  SSD1315_t oled1 = SSD1315_Init(&hi2c1, SSD1315_ADDR0); // blocking
  if (oled1.err != HAL_OK){
	  uart_send("Oled init failed\r\n");
	  while(1);
  }

  uint32_t last_update = 0;
  uint8_t toggle_pattern = 0x00;
  uint8_t mode = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  if (HAL_GetTick() - last_update >= 2000) {
		  if (SSD1315_GetPrtStatus(&oled1) == HAL_OK) {
			  last_update = HAL_GetTick();

			  switch (mode) {
			  	case 0: // test FillAll
			  		toggle_pattern = ~toggle_pattern;
			  		SSD1315_fb_FillAll(&oled1, toggle_pattern);
			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

			  	case 1: // test SetPixel
			  		SSD1315_fb_FillAll(&oled1, 0x00);
			  		SSD1315_SetPixel(&oled1, 0, 0);
			  		SSD1315_SetPixel(&oled1, 0, 63);
			  		SSD1315_SetPixel(&oled1, 127, 0);
			  		SSD1315_SetPixel(&oled1, 127, 63);
			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

			  	case 2: // test ClrPixel
			  		SSD1315_fb_FillAll(&oled1, 0xFF);
			  		SSD1315_ClrPixel(&oled1, 0, 0);
			  		SSD1315_ClrPixel(&oled1, 0, 63);
			  		SSD1315_ClrPixel(&oled1, 127, 0);
			  		SSD1315_ClrPixel(&oled1, 127, 63);
			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

			    case 3: // test TogPixel
			  		SSD1315_TogPixel(&oled1, 63, 31);
			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

			  	case 4: // test DrwLine_H
			  		SSD1315_fb_FillAll(&oled1, 0x00);

			  		SSD1315_DrwLine_H(&oled1, 10, 5, 117, 5);
			  		SSD1315_DrwLine_H(&oled1, 10, 58, 117, 58);
			  		SSD1315_DrwLine_H(&oled1, 10, 5, 10, 58);
			  		SSD1315_DrwLine_H(&oled1, 117, 5, 117, 58);

			  		SSD1315_DrwLine_H(&oled1, 63, 5, 117, 31);
			  		SSD1315_DrwLine_H(&oled1, 117, 31, 63, 58);
			  		SSD1315_DrwLine_H(&oled1, 63, 58, 10, 31);
			  		SSD1315_DrwLine_H(&oled1, 10, 31, 63, 5);

			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

			    case 5: // test DrwLine_L
			  		SSD1315_fb_FillAll(&oled1, 0xFF);

			  		SSD1315_DrwLine_L(&oled1, 10, 5, 117, 5);
			  		SSD1315_DrwLine_L(&oled1, 10, 58, 117, 58);
			  		SSD1315_DrwLine_L(&oled1, 10, 5, 10, 58);
			  		SSD1315_DrwLine_L(&oled1, 117, 5, 117, 58);

			  		SSD1315_DrwLine_L(&oled1, 63, 5, 117, 31);
			  		SSD1315_DrwLine_L(&oled1, 117, 31, 63, 58);
			  		SSD1315_DrwLine_L(&oled1, 63, 58, 10, 31);
			  		SSD1315_DrwLine_L(&oled1, 10, 31, 63, 5);

			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

			  	case 6:
			  		SSD1315_fb_FillAll(&oled1, 0x00);

			  		SSD1315_FillRect_H(&oled1, 4, 2, 56, 24);
			  		SSD1315_FillRect_H(&oled1, 68, 2, 56, 24);
			  		SSD1315_FillRect_H(&oled1, 4, 34, 56, 24);
			  		SSD1315_FillRect_H(&oled1, 68, 34, 56, 24);

			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

			  	case 7:
				  	SSD1315_fb_FillAll(&oled1, 0xFF);

				  	SSD1315_FillRect_L(&oled1, 4, 2, 56, 24);
				  	SSD1315_FillRect_L(&oled1, 68, 2, 56, 24);
				  	SSD1315_FillRect_L(&oled1, 4, 34, 56, 24);
				  	SSD1315_FillRect_L(&oled1, 68, 34, 56, 24);

				  	SSD1315_WriteFullFrame(&oled1);

				  	mode++;
				  	break;

          case 8:
			  		SSD1315_fb_FillAll(&oled1, 0x00);

			  		SSD1315_DrawRect_H(&oled1, 4, 2, 56, 24);
			  		SSD1315_DrawRect_H(&oled1, 68, 2, 56, 24);
			  		SSD1315_DrawRect_H(&oled1, 4, 34, 56, 24);
			  		SSD1315_DrawRect_H(&oled1, 68, 34, 56, 24);

			  		SSD1315_WriteFullFrame(&oled1);

			  		mode++;
			  		break;

          case 9:
				  	SSD1315_fb_FillAll(&oled1, 0xFF);

				  	SSD1315_DrawRect_L(&oled1, 4, 2, 56, 24);
				  	SSD1315_DrawRect_L(&oled1, 68, 2, 56, 24);
				  	SSD1315_DrawRect_L(&oled1, 4, 34, 56, 24);
				  	SSD1315_DrawRect_L(&oled1, 68, 34, 56, 24);

				  	SSD1315_WriteFullFrame(&oled1);

				  	mode = 0;
				  	break;

			  	default:
			  	  mode = 0;
			  	  break;
			  }
		  }

		  else uart_send("I2C port busy\r\n");

	  }

	  	  HAL_Delay(20);
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
#ifdef USE_FULL_ASSERT
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
