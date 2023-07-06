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
#include "dma.h"
#include "app_lorawan.h"
#include "rtc.h"
#include "subghz.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "LmHandler.h"
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

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
static LmHandlerParams_t LoRaWANParams = {
    .ActiveRegion = LORAMAC_REGION_CN470,
    .DefaultClass = CLASS_A, 
    .AdrEnable = LORAMAC_HANDLER_ADR_OFF,
    .TxDatarate  = DR_2,
    .PingPeriodicity = 4,
};
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
  MX_RTC_Init();
  MX_USART2_UART_Init();
  MX_SUBGHZ_Init();
  MX_LoRaWAN_Init();
  /* USER CODE BEGIN 2 */
   LmHandlerConfigure(&LoRaWANParams);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    MX_LoRaWAN_Process();

    /* USER CODE BEGIN 3 */
    LmHandlerProcess();
        static bool first = true;
    LmHandlerMsgTypes_t confirm = 0;
    static LmHandlerErrorStatus_t send_status = 0;
    static uint32_t time;
    if (first == true || HAL_GetTick() - time > 10000) {
        time = HAL_GetTick();
    } else {
        return;
    }

    if(first == false && LoRaMacIsBusy() == true) {
        printf("The Mac is Busy\r\n");
    } else if(LmHandlerJoinStatus() == LORAMAC_HANDLER_RESET) {
        first = false;
        MibRequestConfirm_t mibReq = {0};

        /*打印DevEui 与 AppEui参数*/
        mibReq.Type = MIB_DEV_EUI;
        LoRaMacMibGetRequestConfirm(&mibReq);
        printf( "Default DevEui      : %02X", mibReq.Param.DevEui[0] );
        for( int i = 1; i < 8; i++ ){
            printf( "-%02X", mibReq.Param.DevEui[i] );
        }
            printf( "\r\n" );
        
        /*打印 AppKey */
        mibReq.Type = MIB_APP_KEY; 
        LoRaMacMibGetRequestConfirm(&mibReq);
        printf( "AppKey      : %02X", mibReq.Param.AppKey[0] );
        for( int i = 1; i < 16; i++ ){
            printf( "-%02X", mibReq.Param.AppKey[i] );
        }
            printf( "\r\n" );


        mibReq.Type = MIB_JOIN_EUI; //其实就是appeui
        LoRaMacMibGetRequestConfirm(&mibReq);
        printf( "AppEui      : %02X", mibReq.Param.JoinEui[0] );
        for( int i = 1; i < 8; i++ ){
            printf( "-%02X", mibReq.Param.JoinEui[i] );
        }
            printf( "\r\n" );


        /*设置信道掩码*/
        uint16_t chan_mask[6] = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
        mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
        mibReq.Param.ChannelsMask = chan_mask;
        LoRaMacMibSetRequestConfirm(&mibReq);

        printf("The netting\r\n");
        LmHandlerJoin(ACTIVATION_TYPE_OTAA);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the CPU, AHB and APB busses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.LSIDiv = RCC_LSI_DIV1;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the SYSCLKSource, HCLK, PCLK1 and PCLK2 clocks dividers
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK3|RCC_CLOCKTYPE_HCLK
                              |RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1
                              |RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.AHBCLK3Divider = RCC_SYSCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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

