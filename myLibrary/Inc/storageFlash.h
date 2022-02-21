/** 
  ******************************************************************************
  * @file    storageFlash.h
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

#ifndef __STORAGE_FLASH_H
#define __STORAGE_FLASH_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/
/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief	storageFlash_configuration
    @return none
*/
void storageFlash_configuration(void);

/** @brief	storageFlash_getStringLen
    @return int
*/
int storageFlash_getStringLen(char *str);

/** @brief  storageFlash_readData
    @return uint32_t
*/
void storageFlash_readData(uint32_t address, uint32_t *buffer, uint16_t numberOfWords);

/** @brief  storageFlash_confirmData
    @return uint32_t
*/
bool storageFlash_confirmData(char *dataConfirm, uint32_t *buffer, uint16_t length);

/** @brief  storageFlash_EraseSector
    @return bool
*/
uint32_t storageFlash_EraseSector(uint32_t startSectorAddress, uint32_t offsets);

/** @brief  storageFlash_writeData
    @return void
*/
//bool storageFlash_writeData(uint32_t startSectorAddress, uint32_t *Data, uint16_t numberOfWords);
uint32_t storageFlash_writeData(uint32_t startSectorAddress, uint8_t *Data, uint16_t numberOfWords);

/** @brief  storageFlash_writeDataNonErase
    @return void
*/
uint32_t storageFlash_writeDataNonErase(uint32_t startSectorAddress, uint8_t *Data, uint16_t numberOfWords);

/** @brief  storageFlash_writeNumber
    @return void
*/
void storageFlash_writeNumber(uint32_t address, float number);

/** @brief  storageFlash_readNumber
    @return void
*/
float storageFlash_readNumber(uint32_t address);

/** @brief  storageFlash_test
    @return void
*/
void storageFlash_test(void);

#endif /* __STORAGE_FLASH_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


