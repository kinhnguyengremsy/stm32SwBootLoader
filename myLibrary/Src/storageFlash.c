/**
  ******************************************************************************
  * @file    storageFlash.c
  * @author  Gremsy Team
  * @version v100
  * @date    2021
  * @brief   This file contains all the functions prototypes for the  
  *          firmware library.
  *
  ************************************************************
  ******************
  * @par
  * COPYRIGHT NOTICE: (c) 2011 Gremsy.
  * All rights reserved.Firmware coding style V1.0.beta
  *
  * The information contained herein is confidential
  * property of Company. The use, copying, transfer or
  * disclosure of such information is prohibited except
  * by express written agreement with Company.
  *
  ******************************************************************************
*/
/* Includes------------------------------------------------------------------------------*/
#include "storageFlash.h"
#include "string.h"
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
bool enableTestFlash = false;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __STORAGE_FLASH_CONFIGURATION
    @{
*/#ifndef __STORAGE_FLASH_CONFIGURATION
#define __STORAGE_FLASH_CONFIGURATION
/** @brief	storageFlash_configuration
    @return none
*/
void storageFlash_configuration(void)
{
	printf("\n[storageFlash_configuration] : \n");
}

/** @brief	float2Bytes
    @return none
*/
void float2Bytes(uint8_t * ftoa_bytes_temp, float float_variable)
{
    union
	{
      float a;
      uint8_t bytes[4];
    } thing;

    thing.a = float_variable;

    for (uint8_t i = 0; i < 4; i++)
    {
      ftoa_bytes_temp[i] = thing.bytes[i];
    }

}

/** @brief	Bytes2float
    @return float
*/
float Bytes2float(uint8_t * ftoa_bytes_temp)
{
    union
	{
      float a;
      uint8_t bytes[4];
    } thing;

    for (uint8_t i = 0; i < 4; i++)
    {
    	thing.bytes[i] = ftoa_bytes_temp[i];
    }

   float float_variable =  thing.a;
   return float_variable;
}

/** @brief	storageFlash_getStringLen
    @return int
*/
int storageFlash_getStringLen(char *str)
{
	return ((strlen(str) / 4) + ((strlen(str) % 4) != 0));
}

#endif
/**
    @}
*/

/** @group __STORAGE_FLASH_READ_WRITE_DATA
    @{
*/#ifndef __STORAGE_FLASH_READ_WRITE_DATA
#define __STORAGE_FLASH_READ_WRITE_DATA

/** @brief  storageFlash_GetSector
    @return uint32_t
*/
static uint32_t storageFlash_GetSector(uint32_t Address)
{
	uint32_t sector = 0;

	if((Address < 0x08003FFF) && (Address >= 0x08000000))
	{
		sector = FLASH_SECTOR_0;
	}
	else if((Address < 0x08007FFF) && (Address >= 0x08004000))
	{
		sector = FLASH_SECTOR_1;
	}
	else if((Address < 0x0800BFFF) && (Address >= 0x08008000))
	{
		sector = FLASH_SECTOR_2;
	}
	else if((Address < 0x0800FFFF) && (Address >= 0x0800C000))
	{
		sector = FLASH_SECTOR_3;
	}
	else if((Address < 0x0801FFFF) && (Address >= 0x08010000))
	{
		sector = FLASH_SECTOR_4;
	}
	else if((Address < 0x0803FFFF) && (Address >= 0x08020000))
	{
		sector = FLASH_SECTOR_5;
	}
	else if((Address < 0x0805FFFF) && (Address >= 0x08040000))
	{
		sector = FLASH_SECTOR_6;
	}
	else if((Address < 0x0807FFFF) && (Address >= 0x08060000))
	{
		sector = FLASH_SECTOR_7;
	}
	else if((Address < 0x0809FFFF) && (Address >= 0x08080000))
	{
		sector = FLASH_SECTOR_8;
	}
	else if((Address < 0x080BFFFF) && (Address >= 0x080A0000))
	{
		sector = FLASH_SECTOR_9;
	}
	else if((Address < 0x080DFFFF) && (Address >= 0x080C0000))
	{
		sector = FLASH_SECTOR_10;
	}
	else if((Address < 0x080FFFFF) && (Address >= 0x080E0000))
	{
		sector = FLASH_SECTOR_11;
	}

	return sector;
}

/** @brief  storageFlash_readData
    @return uint32_t
*/
void storageFlash_readData(uint32_t address, uint8_t *buffer, uint16_t numberOfWords)
{
	do
	{
#if (USE_CONSOLE_DEBUG == 1)
		printf("\n[storageFlash_readData] read from address 0x%x\n", (int)address);
#endif
		*buffer = *(uint8_t *)address;
		address ++;//= 4;
		buffer ++;
	}
	while((numberOfWords--) != 0);
}

/** @brief  storageFlash_confirmData
    @return uint32_t
*/
bool storageFlash_confirmData(char *dataConfirm, uint32_t *buffer, uint16_t length)
{
	bool ret = false;
	uint32_t confirmAddress = ADDR_FLASH_SECTOR_8;
	int len = length;

	printf("\n[storageFlash_confirmData] RUNNING data = \"%s\"\n", dataConfirm);

	storageFlash_readData(confirmAddress, buffer, len);

	if(memcmp(dataConfirm, buffer, len) == 0)
	{
		printf("\n[storageFlash_confirmData] DONE data = \"%s\"\n", (char *)buffer);

		ret = true;
	}

	return ret;
}

/** @brief  storageFlash_EraseSector
    @return bool
*/
uint32_t storageFlash_EraseSector(uint32_t startSectorAddress, uint32_t offsets)
{
	static FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t SECTORError;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Get the number of sector to erase from 1st sector */

	uint32_t StartSector = storageFlash_GetSector(startSectorAddress);
	uint32_t EndSectorAddress = startSectorAddress + offsets;
	uint32_t EndSector = storageFlash_GetSector(EndSectorAddress);

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = StartSector;
	EraseInitStruct.NbSectors     = (EndSector - StartSector) + 1;

	printf("\n[storageFlash_EraseSector] StartSector : 0x%x - EndSector 0x%x\n", (int)startSectorAddress, (int)EndSectorAddress);

	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	 you have to make sure that these data are rewritten before they are accessed during code
	 execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	 DCRST and ICRST bits in the FLASH_CR register. */
	if (HAL_FLASHEx_Erase(&EraseInitStruct, & SECTORError) != HAL_OK)
	{
		return HAL_FLASH_GetError();
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return 0;
}

/** @brief  storageFlash_writeData
    @return bool
*/
//bool storageFlash_writeData(uint32_t startSectorAddress, uint32_t *Data, uint16_t numberOfWords)
uint32_t storageFlash_writeData(uint32_t startSectorAddress, uint8_t *Data, uint16_t numberOfWords)
{
	int sofar = 0;

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/* Program the user Flash area word by word
	(area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

//	printf("\n[storageFlash_writeData] start write flash data\n");

	while (sofar < numberOfWords)
	{
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, startSectorAddress, Data[sofar]) == HAL_OK)
		{
//			printf("\n[storageFlash_writeData] write to address 0x%x value = 0x%x\n", (int)startSectorAddress, (int)Data[sofar]);
			HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);

			startSectorAddress ++;//= 4;  // use StartPageAddress += 2 for half word and 8 for double word
			sofar++;
		}
		else
		{
			/* Error occurred while writing data in Flash memory*/
			return HAL_FLASH_GetError();
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return 0;
}

/** @brief  storageFlash_writeDataNonErase
    @return void
*/
uint32_t storageFlash_writeDataNonErase(uint32_t startSectorAddress, uint8_t *Data, uint16_t numberOfWords)
{
	printf("\nfunction not found .................................\n");

	return 0;
}

/** @brief  storageFlash_writeNumber
    @return void
*/
void storageFlash_writeNumber(uint32_t address, float number)
{
	uint8_t bytes_temp[4];

	float2Bytes(bytes_temp, number);
	storageFlash_writeData(address, (uint32_t *)bytes_temp, 1);
}

/** @brief  storageFlash_readNumber
    @return void
*/
float storageFlash_readNumber(uint32_t address)
{
	uint8_t buffer[4];
	float value;

	storageFlash_readData(address, (uint32_t *)buffer, 1);
	value = Bytes2float(buffer);

	return value;
}

/** @brief  storageFlash_test
    @return void
*/
void storageFlash_test(void)
{
	if(enableTestFlash == false)
	{
		enableTestFlash = true;

//		char *Data = "nguyen van kinh 123456789";
//		uint32_t Data[] = {0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111};
		uint8_t Data[] ={1, 2, 3, 4, 5, 6, 7, 8, 9};
		int numberOfWords = 0;

		numberOfWords = 9;//storageFlash_getStringLen(Data);

		printf("\n[swBootLoader_process] write to flash : %s\n", Data);

		if(storageFlash_writeData(ADDR_FLASH_SECTOR_6, Data, numberOfWords) == false)
		{
			printf("\n[swBootLoader_process] write to flash : DONE\n");
		}

		if(storageFlash_writeDataNonErase(ADDR_FLASH_SECTOR_6 + 8, Data, numberOfWords) == false)
		{
			printf("\n[swBootLoader_process] write to flash : DONE\n");
		}

		if(storageFlash_writeDataNonErase(ADDR_FLASH_SECTOR_6 + 17, Data, numberOfWords) == false)
		{
			printf("\n[swBootLoader_process] write to flash : DONE\n");
		}

		uint32_t rxBuffer[numberOfWords];
		printf("\n[swBootLoader_process] read from flash startAddress 0x%x\n", (int)ADDR_FLASH_SECTOR_6);

		storageFlash_readData(ADDR_FLASH_SECTOR_6, rxBuffer, numberOfWords);

		if(memcmp(rxBuffer, Data, numberOfWords) == 0)
		printf("\n[swBootLoader_process] read from flash address 0x%x value %s\n", (int)ADDR_FLASH_SECTOR_6, rxBuffer);

	}
}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

