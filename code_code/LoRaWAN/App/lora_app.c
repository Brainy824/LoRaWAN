/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    lora_app.c
  * @author  MCD Application Team
  * @brief   Application of the LRWAN Middleware
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
#include "platform.h"
#include "Region.h" /* Needed for LORAWAN_DEFAULT_DATA_RATE */
#include "sys_app.h"
#include "lora_app.h"
#include "stm32_seq.h"
#include "LmHandler.h"

/* USER CODE BEGIN Includes */
#include "lora_app_version.h"
#include "lorawan_version.h"
#include "subghz_phy_version.h"
/* USER CODE END Includes */

/* External variables ---------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private function prototypes -----------------------------------------------*/
/**
  * @brief  join event callback function
  * @param  joinParams status of join
  */
static void OnJoinRequest(LmHandlerJoinParams_t *joinParams);

/**
  * @brief  tx event callback function
  * @param  params status of last Tx
  */
static void OnTxData(LmHandlerTxParams_t *params);

/**
  * @brief callback when LoRa application has received a frame
  * @param appData data received in the last Rx
  * @param params status of last Rx
  */
static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params);

/*!
 * Will be called each time a Radio IRQ is handled by the MAC layer
 *
 */
static void OnMacProcessNotify(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private variables ---------------------------------------------------------*/
/**
  * @brief LoRaWAN handler Callbacks
  */
static LmHandlerCallbacks_t LmHandlerCallbacks =
{
  .GetBatteryLevel =           GetBatteryLevel,
  .GetTemperature =            GetTemperatureLevel,
  .GetUniqueId =               GetUniqueId,
  .GetDevAddr =                GetDevAddr,
  .OnMacProcess =              OnMacProcessNotify,
  .OnJoinRequest =             OnJoinRequest,
  .OnTxData =                  OnTxData,
  .OnRxData =                  OnRxData
};

/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Exported functions ---------------------------------------------------------*/
/* USER CODE BEGIN EF */

/* USER CODE END EF */

void LoRaWAN_Init(void)
{
  /* USER CODE BEGIN LoRaWAN_Init_1 */
   printf("APP_VERSION:        V%X.%X.%X\r\n",
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__LORA_APP_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW LoraWAN info */
  printf("MW_LORAWAN_VERSION: V%X.%X.%X\r\n",
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__LORAWAN_VERSION >> __APP_VERSION_SUB2_SHIFT));

  /* Get MW SubGhz_Phy info */
  printf("MW_RADIO_VERSION:   V%X.%X.%X\r\n",
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_MAIN_SHIFT),
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB1_SHIFT),
          (uint8_t)(__SUBGHZ_PHY_VERSION >> __APP_VERSION_SUB2_SHIFT));

  LoraInfo_Init();
  /* USER CODE END LoRaWAN_Init_1 */

  /* Init the Lora Stack*/
  LmHandlerInit(&LmHandlerCallbacks);

  /* USER CODE BEGIN LoRaWAN_Init_Last */

  /* USER CODE END LoRaWAN_Init_Last */
}

/* USER CODE BEGIN PB_Callbacks */

/* USER CODE END PB_Callbacks */

/* Private functions ---------------------------------------------------------*/
/* USER CODE BEGIN PrFD */

/* USER CODE END PrFD */

static void OnRxData(LmHandlerAppData_t *appData, LmHandlerRxParams_t *params)
{
  /* USER CODE BEGIN OnRxData_1 */
    if ((appData != NULL) && (params != NULL))
  {
    static const char *slotStrings[] = { "1", "2", "C", "C Multicast", "B Ping-Slot", "B Multicast Ping-Slot" };

    printf("\r\n###### ========== MCPS-Indication ==========\r\n");
    printf("###### D/L FRAME:%04lu | SLOT:%s | PORT:%d | DR:%d | RSSI:%d | SNR:%d\r\n", params->DownlinkCounter,
                slotStrings[params->RxSlot], appData->Port, params->Datarate, params->Rssi, params->Snr);
    printf("###### DATA:  ");
    for (uint8_t i = 0; i < appData->BufferSize; i++){
          printf("%02x", appData->Buffer[i]);
    }
    printf("\r\n\r\n");
    if(appData->Buffer[0] == 0xa1 && appData->Buffer[1] == 0xb2)
    {
      if(LmHandlerSetTxDatarate(4) == LORAMAC_HANDLER_SUCCESS){
        printf("Set TxDr Success\r\n");
      }
      else{
        printf("Set TxDr Failed\r\n");
      }
    }
  }
  /* USER CODE END OnRxData_1 */
}

/* USER CODE BEGIN PrFD_LedEvents */

/* USER CODE END PrFD_LedEvents */

static void OnTxData(LmHandlerTxParams_t *params)
{
  /* USER CODE BEGIN OnTxData_1 */
      if ((params != NULL) && (params->IsMcpsConfirm != 0))
  {
    MibRequestConfirm_t mibReq = {0};

    printf("\r\n###### ========== MCPS-Confirm =============\r\n");
    printf("###### U/L FRAME:%04lu | PORT:%d | DR:%d | PWR:%d | CHANNEL:%d", params->UplinkCounter,
                params->AppData.Port, params->Datarate, params->TxPower, params->Channel);

    printf(" | MSG TYPE:");
    if (params->MsgType == LORAMAC_HANDLER_CONFIRMED_MSG)
      printf("CONFIRMED [%s]\r\n", (params->AckReceived != 0) ? "ACK" : "NACK");
    else
      printf("UNCONFIRMED\r\n");

    mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;
    LoRaMacMibGetRequestConfirm(&mibReq);
    printf("###### CHANNEL MASK: %04x", mibReq.Param.ChannelsMask[0]);
    for (uint8_t i = 6; i > 0; i--)
    printf(" %04x", mibReq.Param.ChannelsMask[i]);
    printf("\r\n");

    printf("###### DATA:  ");
    for (uint8_t i = 0; i < params->AppData.BufferSize; i++)
        printf("%02x", params->AppData.Buffer[i]);
    printf("\r\n\r\n");

  }
  /* USER CODE END OnTxData_1 */
}

static void OnJoinRequest(LmHandlerJoinParams_t *joinParams)
{
  /* USER CODE BEGIN OnJoinRequest_1 */
      if (joinParams != NULL)
  {
    if (joinParams->Status == LORAMAC_HANDLER_SUCCESS)
    {
        MibRequestConfirm_t mibGet = {0};
        printf("\r\n###### ===== JOINED ==== ######\r\n");
        if (joinParams->Mode == ACTIVATION_TYPE_OTAA)
            printf("###### OTAA\r\n");
        else
            printf("###### ABP\r\n");
        mibGet.Type = MIB_DEV_ADDR;
        LoRaMacMibGetRequestConfirm(&mibGet);
        printf("###### DevAddr: %08lX", mibGet.Param.DevAddr);
        mibGet.Type = MIB_CHANNELS_DATARATE;
        LoRaMacMibGetRequestConfirm(&mibGet);
        printf(" | DATA RATE: DR_%d\r\n", mibGet.Param.ChannelsDatarate);
        printf("###### ===== JOIN END ==== ######\r\n\r\n");

        //unxx_lora_joined_callback(joinParams->Mode);
    }
    else
    {
        printf("\r\n###### ===== JOIN FAILED ==== ######\r\n");
        printf("###### ===== JOIN END ==== ######\r\n\r\n");
    }

#if (MODULE_USER_CHANGE == 1)
    uint8_t led_mode = 0;
    read_data_from_flash((uint32_t *)&led_mode, sizeof(pApplication->led_mode), (uint32_t)&pApplication->led_mode, 0);

    if (LmHandlerJoinStatus() == LORAMAC_HANDLER_RESET)
    {
        g_lora_status_info.joined = false;
        // if (led_mode != 0)
        //     module_led_set_toggle(&state_led, 300, 0, 1);
    }
    else
    {
        g_lora_status_info.joined = true;
        if (led_mode != 0)
            module_led_set_toggle(&work_led, 150, 150, 2);
        report_poweron_enqueue(0);
    }
#endif
  }
  /* USER CODE END OnJoinRequest_1 */
}

static void OnMacProcessNotify(void)
{
  /* USER CODE BEGIN OnMacProcessNotify_1 */

  /* USER CODE END OnMacProcessNotify_1 */
}
