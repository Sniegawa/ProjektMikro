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
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ringbuffer.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define USART_LINE_LEN 128
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
RxRingBuffer rxBuffer;
TxRingBuffer txBuffer;
static uint8_t txByte;
static uint8_t rxByte;
volatile uint32_t USART_Errors;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void USART_Send(const char* str);
int8_t USART_GetLine(char* buf);
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  RxRB_Init(&rxBuffer);
  TxRB_Init(&txBuffer);

  HAL_UART_Receive_IT(&huart2, &rxByte, 1);
  USART_Send("START\r\n");

  char bx[USART_LINE_LEN];
  int len;
  uint32_t cntr = 0;

  char txMsgBuf[USART_LINE_LEN];

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  cntr++;
	  if((len = USART_GetLine(bx)) > 0) // Wykonujemy tylko gdy jest pełna linia
	  {
		  snprintf(txMsgBuf,sizeof(txMsgBuf), "REC[%d]> %s\r\n", len, bx);
		  USART_Send(txMsgBuf);

		  switch(bx[0])
		  {
		  	  case 'l':
		  	  case 'L':
		  		  HAL_GPIO_TogglePin(LD2_GPIO_Port,LD2_Pin);
		  		  USART_Send("Zmiana stanu diody\r\n");
		  		  break;
		  	  case 's':
		  	  case 'S':
		  		  snprintf(txMsgBuf, sizeof(txMsgBuf), "STAN SWITCHA USER %d\r\n", HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin));
		  		  USART_Send(txMsgBuf);
		  		  break;
		  	  case '?':
		  		  snprintf(txMsgBuf, sizeof(txMsgBuf), "LOOP %lu RX_OVF %lu TX_OVF %lu ERR %lu\r\n",
		  				  cntr, (unsigned long)rxBuffer.overflowCount, (unsigned long)txBuffer.overflowCount, (unsigned long)USART_Errors);
		  		  USART_Send(txMsgBuf);
		  		  break;
		  	  default:
		  		  snprintf(txMsgBuf, sizeof(txMsgBuf), "nieznana komenda: %s\r\n");
		  		  USART_Send(txMsgBuf);
		  }
	  }
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
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

/* USER CODE BEGIN 4 */

void USART_Send(const char* str)
{
	uint16_t messageLen = (uint16_t)strlen(str);

	// Wpisujemy wiadomość (lub taką jej część jaka się zmieści)
	uint16_t i = 0;
	while(i < messageLen && TxRB_Push(&txBuffer, (uint8_t)str[i])) i++;

	// Jeżeli nie nadajemy i mamy coś do nadania
	// Zabezpieczenie przed pustym argumentem str
	__disable_irq();
	if(!txBuffer.active && TxRB_Pop(&txBuffer, &txByte))
	{
		HAL_StatusTypeDef ret = HAL_UART_Transmit_IT(&huart2, &txByte, 1);
		if(ret != HAL_OK)
		{
			// Jeżeli HAL nie przyjął polecenia nadania
			// Cofamy bufor aby nie stracić danych
			txBuffer.tail = (txBuffer.tail - 1) & TX_RINGBUFFER_MASK;
			txBuffer.active = false;
		}
		else
			txBuffer.active = true;
	}
	__enable_irq();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
	if(huart != &huart2) return;

	if(TxRB_Pop(&txBuffer, &txByte))
	{
		HAL_StatusTypeDef ret = HAL_UART_Transmit_IT(&huart2, &txByte, 1);
		if(ret != HAL_OK)
		{
			// Jeżeli HAL nie przyjął polecenia nadania
			// Cofamy bufor aby nie stracić danych
			txBuffer.tail = (txBuffer.tail - 1) & TX_RINGBUFFER_MASK;
			txBuffer.active = false;
		}
	}
	else
		txBuffer.active = false;

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
	if(huart != &huart2) return;

	// Ignorujemy informacje zwrotną, czy bajt został odrzucony
	// Zostaje ona jedynie zapisana w buforze w overflowCount
	RxRB_Push(&rxBuffer, rxByte);

	HAL_UART_Receive_IT(&huart2, &rxByte, 1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
	if(huart == &huart2)
	{
		USART_Errors++;
		HAL_UART_Receive_IT(&huart2, &rxByte, 1);
	}
}


int8_t USART_GetLine(char* buf)
{
	static char bf[USART_LINE_LEN];
	static uint8_t idx = 0;
	static bool too_long = false;
	uint8_t c;
	int8_t ret;

	while(RxRB_Pop(&rxBuffer, &c))
	{
		if(c == '\r' || c == '\n')
		{
			if(too_long)
			{
				too_long = 0;
				idx = 0;
				USART_Send("ERR: linia za dluga\r\n");
				continue;
			}
			if(idx == 0) continue; // Pusta linia
			bf[idx] = 0;
			memcpy(buf, bf, idx + 1); // Z znakiem konca
			ret = idx;
			idx = 0;
			return ret;
		}
		if(idx < USART_LINE_LEN - 1)
		{
			bf[idx++] = (char)c;
		}
		else
		{
			too_long = 1;
		}
	}
	return 0; // Brak pełnej linii
}

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
