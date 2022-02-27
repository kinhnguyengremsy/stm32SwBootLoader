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
#define GREMSY_STORAGE_FLASH_EMPTY           0xEEEEEEEE
#define GREMSY_STORAGE_FLASH_RECEIVER        0xEEEEEEEA
#define GREMSY_STORAGE_FLASH_FULL            0xEEEEEEE0

#define GREMSY_STORAGE_PARAM_NUM             80

/// dinh nghia kieu data bit
#define BYTE        1
#define KBYTE       (1024*BYTE)
#define MBYTE       (1024*KBYTE)
#define GBYTE       (1024*MBYTE)
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
/* Private function prototypes------------------------------------------------------------------------------*/
/** @brief	storageFlash_styleGremsy_configuration
    @return flashStatus_t
*/
static flashStatus_t storageFlash_styleGremsy_configuration(void);
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
	flashStatus_t status = FLASH_STATUS_COMPLETE;
	printf("\n[storageFlash_configuration] : nomal config\n");

	status = storageFlash_styleGremsy_configuration();
	if(status != FLASH_STATUS_COMPLETE)
	{
		printf("\n[storageFlash_configuration] flash config gremsy style fail \n");
	}
	else
	{
		printf("\n[storageFlash_configuration] flash config gremsy style successful \n");
	}
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

	storageFlash_readData(confirmAddress, (uint8_t *)buffer, len);

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
//	storageFlash_writeData(address, (uint32_t *)bytes_temp, 1);
}

/** @brief  storageFlash_readNumber
    @return void
*/
float storageFlash_readNumber(uint32_t address)
{
	uint8_t buffer[4];
	float value = 0;

//	storageFlash_readData(address, (uint32_t *)buffer, 1);
	value = Bytes2float(buffer);

	return value;
}

/** @brief  storageFlash_test
    @return void
*/
void storageFlash_test(void)
{
//	static bool enableTestFlash = false;
//	if(enableTestFlash == false)
//	{
//		enableTestFlash = true;
//
////		char *Data = "nguyen van kinh 123456789";
////		uint32_t Data[] = {0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111, 0x11111111};
//		uint8_t Data[] ={1, 2, 3, 4, 5, 6, 7, 8, 9};
//		int numberOfWords = 0;
//
//		numberOfWords = 9;//storageFlash_getStringLen(Data);
//
//		printf("\n[swBootLoader_process] write to flash : %s\n", Data);
//
//		if(storageFlash_writeData(ADDR_FLASH_SECTOR_6, Data, numberOfWords) == false)
//		{
//			printf("\n[swBootLoader_process] write to flash : DONE\n");
//		}
//
//		if(storageFlash_writeDataNonErase(ADDR_FLASH_SECTOR_6 + 8, Data, numberOfWords) == false)
//		{
//			printf("\n[swBootLoader_process] write to flash : DONE\n");
//		}
//
//		if(storageFlash_writeDataNonErase(ADDR_FLASH_SECTOR_6 + 17, Data, numberOfWords) == false)
//		{
//			printf("\n[swBootLoader_process] write to flash : DONE\n");
//		}
//
//		uint32_t rxBuffer[numberOfWords];
//		printf("\n[swBootLoader_process] read from flash startAddress 0x%x\n", (int)ADDR_FLASH_SECTOR_6);
//
//		storageFlash_readData(ADDR_FLASH_SECTOR_6, rxBuffer, numberOfWords);
//
//		if(memcmp(rxBuffer, Data, numberOfWords) == 0)
//		printf("\n[swBootLoader_process] read from flash address 0x%x value %s\n", (int)ADDR_FLASH_SECTOR_6, rxBuffer);

//	}
}

#endif
/**
    @}
*/

/** @group __STORAGE_FLASH_STYLE_GREMSY
    @{
*/#ifndef __STORAGE_FLASH_STYLE_GREMSY
#define __STORAGE_FLASH_STYLE_GREMSY
/** @brief	storageFlash_format
    @return flashStatus_t
*/
static flashStatus_t storageFlash_format(void)
{
	uint32_t page0Address = ADDR_FLASH_SECTOR_10;
	uint32_t page1Address = ADDR_FLASH_SECTOR_11;

	if(storageFlash_EraseSector(page0Address, 0x20000) != 0)
	{
		printf("\n[storageFlash_format] erase page1 fail ...\n");
		return FLASH_STATUS_ER_PROGRAM;
	}

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	// ghi gia tri page reciever vao page0
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, page0Address, GREMSY_STORAGE_FLASH_RECEIVER) != HAL_OK)
	{
		printf("\n[storageFlash_format] write to page0 value reciever fail ...\n");
		return FLASH_STATUS_ER_PROGRAM;
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	/// erase page1
	if(storageFlash_EraseSector(page1Address, 0x20000) != 0)
	{
		printf("\n[storageFlash_format] erase page1 fail ...\n");
		return FLASH_STATUS_ER_PROGRAM;
	}

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	// ghi gia tri page reciever vao page1
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, page1Address, GREMSY_STORAGE_FLASH_EMPTY) != HAL_OK)
	{
		printf("\n[storageFlash_format] write to page1 value reciever fail ...\n");
		return FLASH_STATUS_ER_PROGRAM;
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	return FLASH_STATUS_COMPLETE;
}

/** @brief	storageFlash_styleGremsy_configuration
    @return flashStatus_t
*/
static flashStatus_t storageFlash_styleGremsy_configuration(void)
{
	flashStatus_t status = FLASH_STATUS_COMPLETE;
	uint32_t page0Address = ADDR_FLASH_SECTOR_10;
	uint32_t page1Address = ADDR_FLASH_SECTOR_11;
	uint32_t page0Status = (*(uint32_t *)ADDR_FLASH_SECTOR_10);
	uint32_t page1Status = (*(uint32_t *)ADDR_FLASH_SECTOR_11);

	/// clear pending flag (if any)
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR  | FLASH_FLAG_WRPERR |
            FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

	printf("\n[storageFlash_styleGremsy_configuration] page0 use sector 0x%x\n", (int)page0Address);
	printf("\n[storageFlash_styleGremsy_configuration] page1 use sector 0x%x\n", (int)page1Address);

	switch(page0Status)
	{
		case GREMSY_STORAGE_FLASH_EMPTY:
		{
			printf("\n[storageFlash_styleGremsy_configuration] page0 is empty\n");

			/// ktra page1
			if(page1Status != GREMSY_STORAGE_FLASH_RECEIVER)
			{
				printf("\n[storageFlash_styleGremsy_configuration] page0 and page1 format\n");

				/// dua flash ve mac dinh
				status = storageFlash_format();
			}
			else
			{
				printf("\n[storageFlash_styleGremsy_configuration] page1 is reciever\n");
			}
		}break;
		case GREMSY_STORAGE_FLASH_RECEIVER:
		{
			printf("\n[storageFlash_styleGremsy_configuration] page0 is reciever\n");

			if(page1Status != GREMSY_STORAGE_FLASH_EMPTY)
			{
				printf("\n[storageFlash_styleGremsy_configuration] page1 erase and write empty\n");

				if(storageFlash_EraseSector(page1Address, 0x20000) != 0)
				{
					printf("\n[storageFlash_styleGremsy_configuration] page1 erase fail\n");
					return FLASH_STATUS_ER_PROGRAM;
				}

				/* Unlock the Flash to enable the flash control register access *************/
				HAL_FLASH_Unlock();

				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, page1Address, GREMSY_STORAGE_FLASH_EMPTY) != HAL_OK)
				{
					printf("\n[storageFlash_format] write to page1 value empty fail ...\n");
					return FLASH_STATUS_ER_PROGRAM;
				}

				/* Lock the Flash to disable the flash control register access (recommended
				 to protect the FLASH memory against possible unwanted operation) *********/
				HAL_FLASH_Lock();

				status = FLASH_STATUS_COMPLETE;
			}
			else
			{
				printf("\n[storageFlash_styleGremsy_configuration] page1 is empty\n");
			}
		}break;
		case GREMSY_STORAGE_FLASH_FULL:
		{
			printf("\n[storageFlash_styleGremsy_configuration] page0 is full\n");

			if(page1Status == GREMSY_STORAGE_FLASH_RECEIVER)
			{
				printf("\n[storageFlash_styleGremsy_configuration] page0 erase and write empty\n");

				if(storageFlash_EraseSector(page0Address, 0x20000) != 0)
				{
					return FLASH_STATUS_ER_PROGRAM;
				}

				/* Unlock the Flash to enable the flash control register access *************/
				HAL_FLASH_Unlock();

				if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, page0Address, GREMSY_STORAGE_FLASH_EMPTY) != HAL_OK)
				{
					printf("\n[storageFlash_format] write to page1 value empty fail ...\n");
					return FLASH_STATUS_ER_PROGRAM;
				}

				/* Lock the Flash to disable the flash control register access (recommended
				 to protect the FLASH memory against possible unwanted operation) *********/
				HAL_FLASH_Lock();

				status = FLASH_STATUS_COMPLETE;
			}
			else
			{
				printf("\n[storageFlash_styleGremsy_configuration] page0 and page1 format\n");

				status = storageFlash_format();
			}
		}break;
		default:
		{
			printf("\n[storageFlash_styleGremsy_configuration] page0 and is other\n");

			printf("\n[storageFlash_styleGremsy_configuration] page0 and page1 format\n");

			status = storageFlash_format();
		}break;
	}

	/// a little delay wait for flash
	HAL_Delay(100);

	return status;
}

/** @brief  storageFlash_findPageFull
    @return uint32_t page address
*/
static uint32_t storageFlash_findPageFull(void)
{
	uint32_t pageAddress = 0;

	uint32_t page0Status = (*(uint32_t *)ADDR_FLASH_SECTOR_10);
	uint32_t page1Status = (*(uint32_t *)ADDR_FLASH_SECTOR_11);

    if(page0Status == GREMSY_STORAGE_FLASH_FULL)
    {
    	pageAddress = ADDR_FLASH_SECTOR_10;
    }
    else if(page1Status == GREMSY_STORAGE_FLASH_FULL)
    {
    	pageAddress = ADDR_FLASH_SECTOR_11;
    }
    else
    {
    	printf("\n[storageFlash_findPageFull] no find page full\n");
    }

	return pageAddress;
}

/** @brief  storageFlash_findPageEmpty
    @return uint32_t page address
*/
static uint32_t storageFlash_findPageEmpty(void)
{
	uint32_t pageAddress = 0;

	uint32_t page0Status = (*(uint32_t *)ADDR_FLASH_SECTOR_10);
	uint32_t page1Status = (*(uint32_t *)ADDR_FLASH_SECTOR_11);

    if(page0Status == GREMSY_STORAGE_FLASH_EMPTY)
    {
    	pageAddress = ADDR_FLASH_SECTOR_10;
    }
    else if(page1Status == GREMSY_STORAGE_FLASH_EMPTY)
    {
    	pageAddress = ADDR_FLASH_SECTOR_11;
    }
    else
    {
    	printf("\n[storageFlash_findPageEmpty] no find page empty\n");
    }

	return pageAddress;
}

/** @brief  storageFlash_findPageReciever
    @return uint32_t page address
*/
static uint32_t storageFlash_findPageReciever(void)
{
	uint32_t pageAddress = 0;

	uint32_t page0Status = (*(uint32_t *)ADDR_FLASH_SECTOR_10);
	uint32_t page1Status = (*(uint32_t *)ADDR_FLASH_SECTOR_11);

    if(page0Status == GREMSY_STORAGE_FLASH_RECEIVER)
    {
    	pageAddress = ADDR_FLASH_SECTOR_10;
    }
    else if(page1Status == GREMSY_STORAGE_FLASH_RECEIVER)
    {
    	pageAddress = ADDR_FLASH_SECTOR_11;
    }
    else
    {
    	printf("\n[storageFlash_findPageReciever] no find page reciever\n");
    }

	return pageAddress;
}

/** @brief  storageFlash_writeAndVerifyPageFull
 *  @param[in] address (use style gremsy) of value write
 *  @param[in] value : value write to flash
    @return flashStatus_t
*/
static flashStatus_t storageFlash_writeAndVerifyPageFull(uint16_t address, uint16_t value)
{
	uint32_t startAddress = 0;
	uint32_t endAddress = 0;
	uint32_t runAddress = 0;

	/// chuyen address (2 bytes) va value (2 bytes) thanh 4 byte
	uint32_t valueFlash = (uint32_t)address << 16 | value;

	/// tim page sang sang ghi du lieu
	uint32_t pageAddress = storageFlash_findPageReciever();

	startAddress = pageAddress;
	endAddress = pageAddress + 128*KBYTE - 2;

	/// dia chi bat dau ghi vao flash
	runAddress = startAddress;

	/// kiem tra cung nho trong
	while(runAddress < endAddress)
	{
		if(*(__IO uint32_t*)runAddress == 0xffffffff)
		{
			/* Unlock the Flash to enable the flash control register access *************/
			HAL_FLASH_Unlock();

			if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, runAddress, valueFlash) == HAL_OK)
			{
				return FLASH_STATUS_COMPLETE;
			}

			/* Lock the Flash to disable the flash control register access (recommended
			 to protect the FLASH memory against possible unwanted operation) *********/
			HAL_FLASH_Lock();
		}
		else
		{
			runAddress += 4;
		}
	}

	return FLASH_STATUS_ER_PROGRAM;
}

/** @brief  storageFlash_readPageFull
 *  @param[in] address (use style gremsy) of value read
 *  @param[in] *data : value read to flash
    @return bool
*/
static bool storageFlash_readPageFull(uint16_t address, uint16_t *data)
{
	uint32_t pageFullAddress = 0;

	uint32_t startAddress = 0;
	uint32_t endAddress = 0;

	uint32_t runAddress = 0;
	uint16_t addressValue = 0xfffe;

	pageFullAddress = storageFlash_findPageFull();
	if(pageFullAddress == 0)
	{
		return false;
	}

	startAddress = pageFullAddress;
	endAddress = startAddress + 128*KBYTE - 2;

	/// lay dia chi bat dau doc
	runAddress = endAddress;

	/// doc du lieu
	while(runAddress > (startAddress + 2))
	{
        /* Get the current location content to be compared with virtual address */
		addressValue = (*(__IO uint16_t*)runAddress);

        /* Compare the read address with the virtual address */
        if(addressValue == address)
        {
            /* Get content of Address-2 which is variable value */
            *data = (*(__IO uint16_t*)(runAddress - 2));

            return true;
        }
        else
        {
        	runAddress = runAddress - 4;
        }
	}

	return false;
}

/** @brief  storageFlash_styleGremsy_write
    @return bool
*/
static flashStatus_t storageFlash_tranferData(uint16_t address, uint16_t value)
{
	uint32_t addressEmpty = 0;
	uint32_t addressReciever = 0;

	uint32_t pageEmpty = storageFlash_findPageEmpty();
	if(pageEmpty == ADDR_FLASH_SECTOR_10)
	{
		addressEmpty 	= ADDR_FLASH_SECTOR_10;
		addressReciever = ADDR_FLASH_SECTOR_11;
	}
	else if(pageEmpty == ADDR_FLASH_SECTOR_11)
	{
		addressEmpty 	= ADDR_FLASH_SECTOR_11;
		addressReciever = ADDR_FLASH_SECTOR_10;
	}
	else
	{
		return FLASH_STATUS_ER_OPERATION;
	}

	/* Unlock the Flash to enable the flash control register access *************/
	HAL_FLASH_Unlock();

	/// write header data switch page empty to page reciver
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addressEmpty, GREMSY_STORAGE_FLASH_RECEIVER) != HAL_OK)
	{
		return FLASH_STATUS_ER_PROGRAM;
		printf("\n[storageFlash_tranferData] write hearder flash switch lage empty to reciever fail\n");
	}

//	/* Lock the Flash to disable the flash control register access (recommended
//	 to protect the FLASH memory against possible unwanted operation) *********/
//	HAL_FLASH_Lock();
//
//	/* Unlock the Flash to enable the flash control register access *************/
//	HAL_FLASH_Unlock();

	/// write header data switch page reciever to page full
	if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, addressReciever, GREMSY_STORAGE_FLASH_FULL) != HAL_OK)
	{
		return FLASH_STATUS_ER_PROGRAM;
		printf("\n[storageFlash_tranferData] write hearder flash switch lage reciever to full fail\n");
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	/// write value vao page reciever moi duoc chuyen
	flashStatus_t status = storageFlash_writeAndVerifyPageFull(address, value);
	if(status != FLASH_STATUS_COMPLETE)
	{
		printf("\n[storageFlash_tranferData] write value to reciever fail\n");
		return FLASH_STATUS_ER_PROGRAM;
	}

    /* Transfer process: transfer variables from old to the new active page */
    for (int i = 0; i < GREMSY_STORAGE_PARAM_NUM; i++)
    {
        if (i != address)  /* Check each variable except the one passed as parameter */
        {
            uint16_t dataVar;

            /* Read the other last variable updates */
            bool readResult = storageFlash_readPageFull(i, &dataVar);

            /* In case variable corresponding to the virtual address was found */
            if (readResult == true)
            {
                /* Transfer the variable to the new active page */
                flashStatus_t status = storageFlash_writeAndVerifyPageFull(i, dataVar);

                /* If program operation was failed, a Flash error code is returned */
                if (status != FLASH_STATUS_COMPLETE)
                {
                	printf("\n[storageFlash_tranferData] write value to reciever fail\n");
                    return FLASH_STATUS_ER_PROGRAM;
                }
            }
        }
    }

	return FLASH_STATUS_COMPLETE;
}

/** @brief  storageFlash_styleGremsy_write
    @return bool
*/
flashStatus_t storageFlash_styleGremsy_write(uint16_t address, uint16_t value)
{
	flashStatus_t status = FLASH_STATUS_COMPLETE;

	status = storageFlash_writeAndVerifyPageFull(address, value);
	if(status == FLASH_STATUS_ER_PROGRAM)
	{
		/// chuyen flash page
		status= storageFlash_tranferData(address, value);
	}

	return status;
}

/** @brief  storageFlash_styleGremsy_read
    @return bool
*/
bool storageFlash_styleGremsy_read(uint16_t address, uint16_t *data)
{
	uint32_t pageFullAddress = 0;

	uint32_t startAddress = 0;
	uint32_t endAddress = 0;

	uint32_t runAddress = 0;
	uint16_t addressValue = 0xfffe;

	pageFullAddress = storageFlash_findPageReciever();
	if(pageFullAddress == 0)
	{
		return false;
	}

	startAddress = pageFullAddress;
	endAddress = startAddress + 128*KBYTE - 2;

	/// lay dia chi bat dau doc
	runAddress = endAddress;

	/// doc du lieu
	while(runAddress > (startAddress + 2))
	{
        /* Get the current location content to be compared with virtual address */
		addressValue = (*(__IO uint16_t*)runAddress);

        /* Compare the read address with the virtual address */
        if(addressValue == address)
        {
            /* Get content of Address-2 which is variable value */
            *data = (*(__IO uint16_t*)(runAddress - 2));

            return true;
        }
        else
        {
        	runAddress = runAddress - 4;
        }
	}

	return false;
}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

