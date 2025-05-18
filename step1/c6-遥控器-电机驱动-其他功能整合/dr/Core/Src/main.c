/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ppm.h"
#include "oled.h"
#include "gy86.h"
#include "MS5611.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PPM_Chn_Max 8                 // 假设8个PPM通道
#define BLUE_BUF_SIZE 64              // 蓝牙发送缓冲区大小
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t blueData[50];
uint16_t PPM_Databuf[8] = {0};
char tx_buffer[BLUE_BUF_SIZE];
volatile uint8_t ppm_data_ready = 0;  // PPM数据就绪标志
volatile uint8_t uart_tx_busy = 0;    // UART发送忙标志
int16_t mag_x, mag_y, mag_z;
int16_t acc_x, acc_y, acc_z, gyro_x, gyro_y, gyro_z;
uint8_t letsgo = 1;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
//{
//  if (huart->Instance == USART2)
//  {
//    // 使用DMA将接收到的数据发送回去
//    HAL_UART_Transmit_DMA(&huart2, (uint8_t*)tx_buffer, len);
//    // 重新启动接收，使用Ex函数，接收不定长数据
//    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, blueData, sizeof(blueData));
//    // 关闭DMA传输过半中断（HAL库默认开启，但我们只需要接收完成中断）
//    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
//  }
//}

//void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
//  if (huart->Instance == USART2) {
//    // 处理接收到的数据（如blueData[0...Size-1]）
//    // 重新启动接收前关闭DMA中断
//    __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT | DMA_IT_TC);
//    HAL_UARTEx_ReceiveToIdle_DMA(&huart2, blueData, sizeof(blueData));
//  }
//}
//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart->Instance == USART2) {
//        uart_tx_busy = 0;  // 标记发送完成
//    }
//}

void startMotor_only4(){
	// An highlighted block
		HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4,2000);//BB
		HAL_Delay(4000);
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4,1000);//BBB
		HAL_Delay(4000);

	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_4,1049);//起飞
		HAL_Delay(2000);

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
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start(&htim2);

  HAL_Delay(20);
//  // 使用Ex函数，接收不定长数据
//  HAL_UARTEx_ReceiveToIdle_DMA(&huart2, blueData, sizeof(blueData));
//  // 关闭DMA传输过半中断（HAL库默认开启，但我们只需要接收完成中断）
//  __HAL_DMA_DISABLE_IT(huart2.hdmarx, DMA_IT_HT);
//  // 现在等待数据接收完成，接收完成后会自动进入 回调函数...

  HAL_Delay(20);
  OLED_Init();                           //OLED初始
  OLED_Clear();                         //清屏
  OLED_DrawBMP(42,1, 85, 5,BMP1,0);//正相显示图片BMP1
  OLED_ShowString(14,6,"Summer Pocket",16, 0);


  HAL_Delay(20);
  HMC5883L_Init();
  MPU6050_Init();

  char defaultData[] = "Bluetooth";
  uint16_t len_default = sizeof(defaultData) - 1;  // 去掉末尾的\0
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(!uart_tx_busy){
		  HMC5883L_ReadData(&mag_x, &mag_y, &mag_z);
		  MPU6050_GetData(&acc_x, &acc_y, &acc_z, &gyro_x, &gyro_y, &gyro_z);
		  uart_tx_busy = 1;
		    int len = snprintf(tx_buffer, sizeof(tx_buffer),
		        "%d,%d,%d,%d,%d,%d,%d,%d,%d\r\n",
		        mag_x, mag_y, mag_z,
		        acc_x, acc_y, acc_z,
		        gyro_x, gyro_y, gyro_z);
		  HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, len, 500);
		  uart_tx_busy = 0;
		  HAL_Delay(200);
	  }
	  if(!uart_tx_busy){
	          HAL_UART_Transmit(&huart2, (uint8_t*)defaultData, len_default, 500);
	          HAL_Delay(500); // 保持适当间隔
	      }
	  if (ppm_data_ready && !uart_tx_busy && 0) {
	       __disable_irq();
	        uint16_t temp_buf[8];
	        memcpy(temp_buf, (uint16_t*)PPM_Databuf, sizeof(PPM_Databuf));
	        ppm_data_ready = 0;
	        __enable_irq();

	        uart_tx_busy = 1;
	        int len = snprintf(tx_buffer, sizeof(tx_buffer),
	            "%hu,%hu,%hu,%hu,%hu,%hu,%hu,%hu\r\n",
	            temp_buf[0], temp_buf[1],
	            temp_buf[2], temp_buf[3],
	            temp_buf[4], temp_buf[5],
	            temp_buf[6], temp_buf[7]);

	        HAL_UART_Transmit(&huart2, (uint8_t*)tx_buffer, len, 500);
	        uart_tx_busy = 0;
	        letsgo = (letsgo+1)%2;
	      }
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
