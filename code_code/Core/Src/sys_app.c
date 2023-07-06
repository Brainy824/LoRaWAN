/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    sys_app.c
  * @author  MCD Application Team
  * @brief   Initializes HW and SW system entities (not related to the radio)
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
#include <stdio.h>
#include "platform.h"
#include "sys_app.h"
#include "stm32_seq.h"
#include "stm32_systime.h"
#include "stm32_lpm.h"
#include "timer_if.h"
#include "utilities_def.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
#define MAX_TS_SIZE (int) 16

/**
  * Defines the maximum battery level
  */
#define LORAWAN_MAX_BAT   254
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Exported functions ---------------------------------------------------------*/
void SystemApp_Init(void)
{
  /* USER CODE BEGIN SystemApp_Init_1 */

  /* USER CODE END SystemApp_Init_1 */

}

uint8_t GetBatteryLevel(void)
{
  uint8_t batteryLevel = 0;

  /* USER CODE BEGIN GetBatteryLevel_0 */

  /* USER CODE END GetBatteryLevel_0 */

  return batteryLevel;  /* 1 (very low) to 254 (fully charged) */
}

uint16_t GetTemperatureLevel(void)
{
  uint16_t temperatureLevel = 0;

  /* USER CODE BEGIN GetTemperatureLevel */

  /* USER CODE END GetTemperatureLevel */
  return temperatureLevel;
}

void GetUniqueId(uint8_t *id)
{
  /* USER CODE BEGIN GetUniqueId_1 */

  /* USER CODE END GetUniqueId_1 */
  uint32_t val = 0;
  val = LL_FLASH_GetUDN();
  if (val == 0xFFFFFFFF)  /* Normally this should not happen */
  {
    uint32_t ID_1_3_val = HAL_GetUIDw0() + HAL_GetUIDw2();
    uint32_t ID_2_val = HAL_GetUIDw1();

    id[7] = (ID_1_3_val) >> 24;
    id[6] = (ID_1_3_val) >> 16;
    id[5] = (ID_1_3_val) >> 8;
    id[4] = (ID_1_3_val);
    id[3] = (ID_2_val) >> 24;
    id[2] = (ID_2_val) >> 16;
    id[1] = (ID_2_val) >> 8;
    id[0] = (ID_2_val);
  }
  else  /* Typical use case */
  {
    id[7] = val & 0xFF;
    id[6] = (val >> 8) & 0xFF;
    id[5] = (val >> 16) & 0xFF;
    id[4] = (val >> 24) & 0xFF;
    val = LL_FLASH_GetDeviceID();
    id[3] = val & 0xFF;
    val = LL_FLASH_GetSTCompanyID();
    id[2] = val & 0xFF;
    id[1] = (val >> 8) & 0xFF;
    id[0] = (val >> 16) & 0xFF;
  }

  /* USER CODE BEGIN GetUniqueId_2 */

  /* USER CODE END GetUniqueId_2 */
}

uint32_t GetDevAddr(void)
{
  uint32_t val = 0;
  /* USER CODE BEGIN GetDevAddr_1 */

  /* USER CODE END GetDevAddr_1 */

  val = LL_FLASH_GetUDN();
  if (val == 0xFFFFFFFF)
  {
    val = ((HAL_GetUIDw0()) ^ (HAL_GetUIDw1()) ^ (HAL_GetUIDw2()));
  }

  /* USER CODE BEGIN GetDevAddr_2 */

  /* USER CODE END GetDevAddr_2 */
  return val;

}

/* USER CODE BEGIN EF */

/* USER CODE END EF */

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

/* HAL overload functions ---------------------------------------------------------*/

/* Use #if 0 if you want to keep the default HAL instead overcharge them*/
/* USER CODE BEGIN Overload_HAL_weaks_1 */
#if 1
/* USER CODE END Overload_HAL_weaks_1 */

/**
  * @note This function overwrites the __weak one from HAL
  */
uint32_t HAL_GetTick(void)
{
  /* TIMER_IF can be based on other counter the SysTick e.g. RTC */
  /* USER CODE BEGIN HAL_GetTick_1 */

  /* USER CODE END HAL_GetTick_1 */
  return TIMER_IF_GetTimerValue();
  /* USER CODE BEGIN HAL_GetTick_2 */

  /* USER CODE END HAL_GetTick_2 */
}

/**
  * @note This function overwrites the __weak one from HAL
  */
void HAL_Delay(__IO uint32_t Delay)
{
  /* TIMER_IF can be based on other counter the SysTick e.g. RTC */
  /* USER CODE BEGIN HAL_Delay_1 */

  /* USER CODE END HAL_Delay_1 */
  TIMER_IF_DelayMs(Delay);
  /* USER CODE BEGIN HAL_Delay_2 */

  /* USER CODE END HAL_Delay_2 */
}
/* USER CODE BEGIN Overload_HAL_weaks_2 */
#endif
/* Redefine here your own if needed */

/* USER CODE END Overload_HAL_weaks_2 */

