/** 
  ******************************************************************************
  * @file    swBootLoader.h
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

#ifndef __SW_BOOTLOADER_H
#define __SW_BOOTLOADER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/
#define ADDRESS_START_APPLICATION ADDR_FLASH_SECTOR_5
/* Exported types ------------------------------------------------------------*/
typedef enum
{
	SW_BOOTLOADER_STATE_IDLE,
	SW_BOOTLOADER_STATE_START_RECIVER_DATA,
	SW_BOOTLOADER_STATE_RECIVER_DATA,
	SW_BOOTLOADER_STATE_CHECK_FW,
	SW_BOOTLOADER_STATE_STORAGE_FLASH,
	SW_BOOTLOADER_STATE_VERIFY_DATA,
	SW_BOOTLOADER_STATE_DONE,
	SW_BOOTLOADER_STATE_ERROR,

}swBootLoaderState_t;

typedef struct
{
	uint32_t 	currentAddress;
	uint32_t 	fileLength;

	uint8_t 	*buffer;
	uint32_t 	*u32_buffer;

	uint8_t 	startReciever;
	uint8_t		stopReciever;

	swBootLoaderState_t _state;

}swBootLoader_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief	swBootLoaderConfiguration
    @return none
*/
void swBootLoaderConfiguration(void);

/** @brief  swBootLoader_process
    @return none
*/
void swBootLoader_process(void);
#endif /* __SW_BOOTLOADER_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


