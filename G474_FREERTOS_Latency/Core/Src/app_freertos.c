/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : app_freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "tim.h"
/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

extern TIM_HandleTypeDef htim2;

#define PERIOD_MS   500U
#define N_SAMPLES   1000U
#define BUFF_SIZE   20U

static char buff[BUFF_SIZE];

/* USER CODE END PD */

/* Definitions for OledTask */
osThreadId_t OledTaskHandle;
const osThreadAttr_t OledTask_attributes = {
  .name = "OledTask",
  .priority = osPriorityNormal,
  .stack_size = 256 * 4
};

/* Definitions for CpuLoadTask */
osThreadId_t CpuLoadTaskHandle;
const osThreadAttr_t CpuLoadTask_attributes = {
  .name = "CpuLoadTask",
  .priority = osPriorityBelowNormal,   // ⬅️ PIÙ BASSA DELL'OLED, CARICO PIÙ "GENTILE"
  .stack_size = 256 * 4
};



/* USER CODE BEGIN FunctionPrototypes */
void StartOledTask(void *argument);
void StartCpuLoadTask(void *argument);
/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void) {

  OledTaskHandle = osThreadNew(StartOledTask, NULL, &OledTask_attributes);
  CpuLoadTaskHandle = osThreadNew(StartCpuLoadTask, NULL, &CpuLoadTask_attributes);
}

/* USER CODE BEGIN Header_StartOledTask */
void StartOledTask(void *argument)
{
  ssd1306_Init();
  ssd1306_Fill(0);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Latency test", Font_7x10, 1);
  ssd1306_UpdateScreen();

  uint32_t min_us = 0xFFFFFFFFU;
  uint32_t max_us = 0U;
  uint64_t sum_us = 0U;

  const TickType_t periodTicks = pdMS_TO_TICKS(PERIOD_MS);
  TickType_t xLastWakeTime = xTaskGetTickCount();

  uint32_t prev_ts = __HAL_TIM_GET_COUNTER(&htim2);
  const uint32_t nominal_us = PERIOD_MS * 1000U;

  for (uint32_t i = 0; i < N_SAMPLES; i++)
  {
    vTaskDelayUntil(&xLastWakeTime, periodTicks);

    uint32_t now_ts = __HAL_TIM_GET_COUNTER(&htim2);

    uint32_t elapsed_us;
    if (now_ts >= prev_ts)
      elapsed_us = now_ts - prev_ts;
    else
      elapsed_us = (0xFFFFFFFFu - prev_ts) + 1u + now_ts;

    prev_ts = now_ts;

    int32_t lat_signed = (int32_t)elapsed_us - (int32_t)nominal_us;
    uint32_t lat_us    = (lat_signed > 0) ? (uint32_t)lat_signed : 0u;

    if (lat_us < min_us) min_us = lat_us;
    if (lat_us > max_us) max_us = lat_us;
    sum_us += lat_us;

    printf("i=%4lu, lat=%lu us (elapsed=%lu us)\r\n",
           (unsigned long)i,
           (unsigned long)lat_us,
           (unsigned long)elapsed_us);

    ssd1306_Fill(0);
    ssd1306_SetCursor(0, 0);
    snprintf(buff, BUFF_SIZE, "Sample %lu", (unsigned long)(i + 1));
    ssd1306_WriteString(buff, Font_7x10, 1);

    ssd1306_SetCursor(0, 16);
    snprintf(buff, BUFF_SIZE, "%lu us", (unsigned long)lat_us);
    ssd1306_WriteString(buff, Font_7x10, 1);

    ssd1306_UpdateScreen();
  }

  uint32_t mean_us   = (uint32_t)(sum_us / N_SAMPLES);
  uint32_t jitter_us = max_us - min_us;

  printf("\r\n=== FreeRTOS Latency Summary ===\r\n");
  printf("Period: %u ms, Samples: %u\r\n", PERIOD_MS, N_SAMPLES);
  printf("Min: %lu us, Max: %lu us, Mean: %lu us, Jitter: %lu us\r\n",
         min_us, max_us, mean_us, jitter_us);

  ssd1306_Fill(0);
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Test done", Font_7x10, 1);
  ssd1306_SetCursor(0, 16);
  ssd1306_WriteString("See serial", Font_7x10, 1);
  ssd1306_UpdateScreen();

  for(;;) osDelay(1000);
}
/* USER CODE END StartOledTask */


/* USER CODE BEGIN Header_StartCpuLoadTask */
/**
  * Task di carico *molto più pesante*
  * per generare latenza su OLED.
  */
/* USER CODE END Header_StartCpuLoadTask */
void StartCpuLoadTask(void *argument)
{
  /* USER CODE BEGIN StartCpuLoadTask */

  volatile float xf = 1.0f;
  volatile uint32_t xi = 0;

  for(;;)
  {
    /* Carico moderato: qualche operazione in virgola mobile + interi */
    for (uint32_t outer = 0; outer < 5U; ++outer)
    {
      for (uint32_t i = 0; i < 100000U; ++i)
      {
        xf = xf * 1.00001f + 0.00001f;  // carico float
        xi += i;                        // carico int
        xi ^= (i << 1);
      }
    }

    /* Cedi la CPU agli altri task */
    taskYIELD();
  }

  /* USER CODE END StartCpuLoadTask */
}
