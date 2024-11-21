/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define MAT_G2_Pin GPIO_PIN_0
#define MAT_G2_GPIO_Port GPIOC
#define MAT_B2_Pin GPIO_PIN_1
#define MAT_B2_GPIO_Port GPIOC
#define MAT_R1_Pin GPIO_PIN_0
#define MAT_R1_GPIO_Port GPIOA
#define MAT_B1_Pin GPIO_PIN_1
#define MAT_B1_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define MAT_G1_Pin GPIO_PIN_4
#define MAT_G1_GPIO_Port GPIOA
#define LD2_Pin GPIO_PIN_5
#define LD2_GPIO_Port GPIOA
#define MAT_CLK_Pin GPIO_PIN_6
#define MAT_CLK_GPIO_Port GPIOA
#define MAT_LAT_Pin GPIO_PIN_7
#define MAT_LAT_GPIO_Port GPIOA
#define MAT_R2_Pin GPIO_PIN_0
#define MAT_R2_GPIO_Port GPIOB
#define MAT_B_Pin GPIO_PIN_10
#define MAT_B_GPIO_Port GPIOB
#define MAT_A_Pin GPIO_PIN_8
#define MAT_A_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define MAT_C_Pin GPIO_PIN_4
#define MAT_C_GPIO_Port GPIOB
#define MAT_D_Pin GPIO_PIN_5
#define MAT_D_GPIO_Port GPIOB
#define MAT_OE_Pin GPIO_PIN_6
#define MAT_OE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
