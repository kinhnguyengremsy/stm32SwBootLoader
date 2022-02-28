/** 
  ******************************************************************************
  * @file    hostUartBootLoader.h
  * @author  Gremsy Team
  * @version v1.0.0
  * @date    2021
  * @brief   This file contains all the functions prototypes for the  
  *          firmware library.
  *
  ******************************************************************************
  * @Copyright
  * COPYRIGHT NOTICE: (c) 2011 Gremsy. All rights reserved.
  *
  * The information contained herein is confidential
  * property of Company. The use, copying, transfer or 
  * disclosure of such information is prohibited except
  * by express written agreement with Company.
  *
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __HOST_UART_BOOTLOADER_H
#define __HOST_UART_BOOTLOADER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/
#if (USE_HOST_BOOTLOADER == 1)
#define HOST_USE_UART_IT	0
#endif
/* Exported types ------------------------------------------------------------*/
typedef enum
{
	HOST_BOOTLOADER_STATE_IDLE,
	HOST_BOOTLOADER_STATE_CONNECTED,
	HOST_BOOTLOADER_STATE_CMD_GET,
	HOST_BOOTLOADER_STATE_GET_ID,
	HOST_BOOTLOADER_STATE_ERASE,
	HOST_BOOTLOADER_STATE_WRITE,
	HOST_BOOTLOADER_STATE_DONE,
	HOST_BOOTLOADER_STATE_ERROR,

}hostBootLoaderState_t;
typedef struct
{
	bool isBootLoader;
	bool uartConfigForHwBL;

	bool isSendCmd;
	uint8_t hostCmd[2];

	hostBootLoaderState_t state;

}hostBootLoader_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  hostUartBootLoaderConfiguration
    @return none
*/
void hostUartBootLoaderConfiguration(hostBootLoader_t *_host, UART_HandleTypeDef *huart);

/** @brief  __hostBL
    @return pointer struct support host boot loader
*/
hostBootLoader_t* __hostBL(void);

/** @brief  hostUartBootLoaderProcess
    @return none
*/
void hostUartBootLoaderProcess(void);
#endif /* __HOST_UART_BOOTLOADER_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


