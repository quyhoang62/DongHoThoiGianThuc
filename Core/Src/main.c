/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Button.h"
#include "DS3231.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
Button_Typedef button0;
Button_Typedef button1;
Button_Typedef button2;
uint32_t minute = 0;
uint8_t temp0, temp1, temp2, temp3, temp4, temp5;
DateTime date_time;
float temperature;

#define D0_LOW() HAL_GPIO_WritePin(DK0_GPIO_Port, DK0_Pin, GPIO_PIN_SET)
#define D0_HIGH() HAL_GPIO_WritePin(DK0_GPIO_Port, DK0_Pin, GPIO_PIN_RESET)
#define D1_LOW() HAL_GPIO_WritePin(DK1_GPIO_Port, DK1_Pin, GPIO_PIN_SET)
#define D1_HIGH() HAL_GPIO_WritePin(DK1_GPIO_Port, DK1_Pin, GPIO_PIN_RESET)
#define D2_LOW() HAL_GPIO_WritePin(DK2_GPIO_Port, DK2_Pin, GPIO_PIN_SET)
#define D2_HIGH() HAL_GPIO_WritePin(DK2_GPIO_Port, DK2_Pin, GPIO_PIN_RESET)
#define D3_LOW() HAL_GPIO_WritePin(DK3_GPIO_Port, DK3_Pin, GPIO_PIN_SET)
#define D3_HIGH() HAL_GPIO_WritePin(DK3_GPIO_Port, DK3_Pin, GPIO_PIN_RESET)
#define D4_LOW() HAL_GPIO_WritePin(DK4_GPIO_Port, DK4_Pin, GPIO_PIN_SET)
#define D4_HIGH() HAL_GPIO_WritePin(DK4_GPIO_Port, DK4_Pin, GPIO_PIN_RESET)
#define D5_LOW() HAL_GPIO_WritePin(DK5_GPIO_Port, DK5_Pin, GPIO_PIN_SET)
#define D5_HIGH() HAL_GPIO_WritePin(DK5_GPIO_Port, DK5_Pin, GPIO_PIN_RESET)



uint8_t segmentNumber[10] = {
	0xC0, // 0
	0xF9, // 1
	0xA4, // 2
	0xB0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xF8, // 7
	0x80, // 8
	0x90 	// 9
};

// anot chung: 1 tat 0 sang
void SevenSegment_Update(uint8_t number)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, ((number>>0)&0x01));
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, ((number>>1)&0x01));
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, ((number>>2)&0x01));
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_3, ((number>>3)&0x01));
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, ((number>>4)&0x01));
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, ((number>>5)&0x01));
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, ((number>>6)&0x01));
}

uint8_t blink_state = 1;       // Trạng thái nháy (1: hiển thị, 0: tắt)
uint32_t last_blink_time = 0;  // Thời điểm nháy cuối cùng

void Segment_Display(uint16_t counter, uint8_t digit, uint8_t blink_option) 
{
    uint8_t temp0 = counter / 10;
    uint8_t temp1 = counter % 10;

    // Kiểm tra thời gian để thay đổi trạng thái nháy
    if (HAL_GetTick() - last_blink_time >= 1000) { // Chu kỳ 1 giây
        blink_state = !blink_state; // Đảo trạng thái nháy
        last_blink_time = HAL_GetTick();
    }

    switch (digit) {
        case 0:
            if (blink_option == 1 && !blink_state) { 
                // Nháy 2 LED đầu: Tắt LED
                SevenSegment_Update(0xFF);
                D0_HIGH();
                HAL_Delay(5);
                D0_LOW();
                D1_HIGH();
                HAL_Delay(5);
                D1_LOW();
            } else {
                // Hiển thị bình thường hoặc khi không nháy
                SevenSegment_Update(segmentNumber[temp0]);
                D0_HIGH();
                HAL_Delay(5);
                D0_LOW();
                SevenSegment_Update(segmentNumber[temp1]);
                D1_HIGH();
                HAL_Delay(5);
                D1_LOW();
            }
            break;

        case 1:
            if (blink_option == 2 && !blink_state) { 
                // Nháy 2 LED sau (D2, D3): Tắt LED
                SevenSegment_Update(0xFF);
                D2_HIGH();
                HAL_Delay(1);
                D2_LOW();
                D3_HIGH();
                HAL_Delay(1);
                D3_LOW();
            } else {
                // Hiển thị bình thường hoặc khi không nháy
                SevenSegment_Update(segmentNumber[temp0]);
                D2_HIGH();
                HAL_Delay(1);
                D2_LOW();
                SevenSegment_Update(segmentNumber[temp1]);
                D3_HIGH();
                HAL_Delay(1);
                D3_LOW();
            }
            break;

        case 2:
            if (blink_option == 3 && !blink_state) { 
                // Nháy 2 LED cuối (D4, D5): Tắt LED
                SevenSegment_Update(0xFF);
                D4_HIGH();
                HAL_Delay(1);
                D4_LOW();
                D5_HIGH();
                HAL_Delay(1);
                D5_LOW();
            } else {
                // Hiển thị bình thường hoặc khi không nháy
                SevenSegment_Update(segmentNumber[temp0]);
                D4_HIGH();
                HAL_Delay(1);
                D4_LOW();
                SevenSegment_Update(segmentNumber[temp1]);
                D5_HIGH();
                HAL_Delay(1);
                D5_LOW();
            }
            break;

        default:
            break;
    }
}

typedef enum 
{
	SEC_STA, // 0
	MIN_STA, // 1
	HOUR_STA, // 2
	ALL_STA, // 3
}SegmentStatus;

SegmentStatus segment_status = ALL_STA;

static uint8_t Times_Button = 0;
void btn_pressing_callback(Button_Typedef * ButtonX){
	
	if(ButtonX == &button0)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		if(Times_Button == 0)
		{
			// 
			segment_status = HOUR_STA;
			Times_Button++;
		}
		else if(Times_Button == 1)
		{
			// 
			segment_status = MIN_STA;
			Times_Button++;
		}
		else if(Times_Button == 2)
		{
			//
			segment_status = SEC_STA;
			Times_Button++;
		}
		else if(Times_Button == 3)
		{
			//
			segment_status = ALL_STA;
			Times_Button = 0;
		}
	}
	else if(ButtonX == &button1)
	{
		//
		switch(segment_status)
		{
			case SEC_STA:
				date_time.sec++;
				if(date_time.sec == 60)
				{
					date_time.sec = 0;
				}
				rtc_write_time(&date_time);
			break;
		case MIN_STA:
				date_time.min++;
				if(date_time.min == 60)
				{
					date_time.min = 0;
				}
				rtc_write_time(&date_time);
			break;
		case HOUR_STA:
				date_time.hour++;
				if(date_time.hour == 24)
				{
					date_time.hour = 0;
				}
				rtc_write_time(&date_time);
			break;
		case ALL_STA:
			break;
		}
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	}
	else if(ButtonX == &button2)
	{
		switch(segment_status)
		{
			case SEC_STA:
				date_time.sec--;
				if(date_time.sec == 0)
				{
					date_time.sec = 60;
				}
				rtc_write_time(&date_time);
			break;
		case MIN_STA:
				if(date_time.min == 0)
				{
					date_time.min = 60;
				}
				date_time.min--;
				
				rtc_write_time(&date_time);
			break;
		case HOUR_STA:
				if(date_time.hour == 0)
				{
					date_time.hour = 24;
				}
				date_time.hour--;
				rtc_write_time(&date_time);
			break;
		case ALL_STA:
			break;
		}
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
	}
}
void Display_Temp(float temperature, uint16_t display_time_ms)
{
		uint16_t temperature1 = temperature*100;
    uint8_t temp0 = temperature1/1000;
    uint8_t temp1 = (temperature1/100)%10;
		uint8_t temp2	= (temperature1/10)%10;
		uint8_t temp3 = temperature1%10;
    uint32_t start_time = HAL_GetTick(); // Lấy thời điểm bắt đầu
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
    while ((HAL_GetTick() - start_time) < display_time_ms) {
			
        SevenSegment_Update(segmentNumber[temp0]);
        D0_HIGH();
        HAL_Delay(1);
        D0_LOW();

        SevenSegment_Update(segmentNumber[temp1]);
        D1_HIGH();
        HAL_Delay(1);
        D1_LOW();
			
				SevenSegment_Update(segmentNumber[temp2]);
        D2_HIGH();
        HAL_Delay(1);
        D2_LOW();

        SevenSegment_Update(segmentNumber[temp3]);
        D3_HIGH();
        HAL_Delay(1);
        D3_LOW();
			
				SevenSegment_Update(0x9C);
        D4_HIGH();
        HAL_Delay(1);
        D4_LOW();
			SevenSegment_Update(0xC6);
        D5_HIGH();
        HAL_Delay(1);
        D5_LOW();
    }

    // Sau 5 giây, tắt LED
    SevenSegment_Update(0); // Giá trị 0 tương ứng với tắt LED
    D0_LOW();
    D1_LOW();
		D2_LOW();
		D3_LOW();
		D4_LOW();
		D5_LOW();
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
}


void Display()
{
		switch(segment_status)
	{
		case SEC_STA:
			Segment_Display(date_time.min, 1, 0);
			Segment_Display(date_time.hour, 0, 0);
			Segment_Display(date_time.sec, 2, 3);
			break;
		case MIN_STA:
			Segment_Display(date_time.sec, 2, 0);
			Segment_Display(date_time.hour, 0, 0);
			Segment_Display(date_time.min, 1, 2);
			break;
		case HOUR_STA:
			Segment_Display(date_time.sec, 2, 0);
			Segment_Display(date_time.min, 1, 0);
			Segment_Display(date_time.hour, 0, 1);
			break;
		case ALL_STA:
			Segment_Display(date_time.sec, 2, 0);
			Segment_Display(date_time.min, 1, 0);
			Segment_Display(date_time.hour, 0, 0);
			break;
	}
}


void btn_press_timeout_short_callback(Button_Typedef * ButtonX)
{
    if (ButtonX == &button2)
    {
        Display_Temp(temperature, 5000); // Hiển thị nhiệt độ trong 5 giây
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
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	Button_Init(&button0, BT0_GPIO_Port, BT0_Pin);
	Button_Init(&button1, BT1_GPIO_Port, BT1_Pin);
	Button_Init(&button2, BT2_GPIO_Port, BT2_Pin);
//	date_time.hour = 13;
//	date_time.min = 48;
//	date_time.sec = 0;
//	date_time.day = 7;
//	date_time.date = 18;
//	date_time.mon = 1;
//	date_time.year = 25;
//	rtc_write_time(&date_time);
	rtc_read_time(&date_time);
//	SethourMode(12);
//	SethourMode(12);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		static uint32_t time_read_led = 0;
		static uint32_t time_read = 0;
		if(HAL_GetTick()-time_read >=1000)
		{
			rtc_read_time(&date_time);
			temperature = rtc_read_temp(&date_time);
			time_read = HAL_GetTick();
		}
		button_handle(&button0);
		button_handle(&button1);
		button_handle(&button2);
		Display();
		if(HAL_GetTick() - time_read_led >=500)
		{
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_12);
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_13);
			time_read_led = HAL_GetTick();
		}
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
  hi2c1.Init.ClockSpeed = 100000;
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
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|DK0_Pin
                          |DK1_Pin|DK2_Pin|DK3_Pin|DK4_Pin
                          |DK5_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA3
                           PA4 PA5 PA6 DK0_Pin
                           DK1_Pin DK2_Pin DK3_Pin DK4_Pin
                           DK5_Pin */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3
                          |GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|DK0_Pin
                          |DK1_Pin|DK2_Pin|DK3_Pin|DK4_Pin
                          |DK5_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : BT0_Pin BT1_Pin BT2_Pin */
  GPIO_InitStruct.Pin = BT0_Pin|BT1_Pin|BT2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

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
