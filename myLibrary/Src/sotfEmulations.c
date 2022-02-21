/**
  ******************************************************************************
  * @file    sotfEmulations.c
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
#include "sotfEmulations.h"
#include "storageFlash.h"
#include "mavlinkMsgHandle.h"
/* Private typedef------------------------------------------------------------------------------*/

typedef struct
{
	uint32_t currentAddress;
	uint32_t oldAddress;

}sotfEmulations_private_t;

/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
extern mavlinkMsgHandle_t mavlinkCOM4;

sotfEmulations_t sotfEmu;
sotfEmulations_private_t sotfPrivate;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __SOTF_EMULATIONS_CONFIGURATION
    @{
*/#ifndef __SOTF_EMULATIONS_CONFIGURATION
#define __SOTF_EMULATIONS_CONFIGURATION
/** @brief  sotfEmulations_configuration
    @return none
*/
void sotfEmulations_configuration(void)
{
	sotfEmu.flashBuffer = calloc(FILE_LENGTH, sizeof(uint32_t));
}

#endif
/**
    @}
*/

/** @group __SOTF_EMULATIONS_READ_WRITE_DATA
    @{
*/#ifndef __SOTF_EMULATIONS_READ_WRITE_DATA
#define __SOTF_EMULATIONS_READ_WRITE_DATA
/** @brief  sotfEmulations_readFlash
    @return bool
*/
static bool sotfEmulations_readFlash(uint32_t *buffer)
{
	uint16_t numberOfWords = FILE_LENGTH / 4;

	printf("\n[sotfEmulations_readFlash] read flash from address 0x%x running\n", (int)ADDR_FLASH_SECTOR_10);

	storageFlash_readData(ADDR_FLASH_SECTOR_10, buffer, numberOfWords);

	printf("\n[sotfEmulations_readFlash] read flash from startAddress 0x%x - endAddress 0x%x successful\n", (int)ADDR_FLASH_SECTOR_10, (int)(ADDR_FLASH_SECTOR_10 + FILE_LENGTH));

	return true;
}

#endif
/**
    @}
*/

/** @group __SOTF_EMULATIONS_PROCESS
    @{
*/#ifndef __SOTF_EMULATIONS_PROCESS
#define __SOTF_EMULATIONS_PROCESS

/** @brief  sotfEmulations_sendStartCommand
    @return none
*/
static timeOutState_t sotfEmulations_sendStartCommand(sotfEmulations_private_t *sPrivate)
{
	static uint32_t timeSendMsg = 0;

	uint8_t isRecievedStartCmd = mavlinkMsg_isLogRequestData(2);

	if(isRecievedStartCmd == 4)
	{
		timeSendMsg = 0;
		printf("\n[sotfEmulations_sendStartCommand] successful\n");
		return TIMEOUT_STATE_DONE;
	}
	else
	{
		if(HAL_GetTick() - timeSendMsg > 1000 || timeSendMsg == 0)
		{
			timeSendMsg = HAL_GetTick();
			mavlinkMsg_send_logRequestData(0, FILE_LENGTH, 0, 2);
		}
	}

	return TIMEOUT_STATE_RUNNING;
}

/** @brief  sotfEmulations_sendFlashData
    @return none
*/
static timeOutState_t sotfEmulations_sendFlashData(sotfEmulations_private_t *sPrivate)
{
//	uint32_t offset = mavlinkCOM4.logData.ofs;
	uint32_t id 	= 0;
	uint8_t count 	= 0;
	static int32_t len 	= 0;
	uint32_t nextAddress 	= len;
	static bool firstSend 	= false;
	uint8_t *data;
	uint8_t u8_buff[4];

	if(firstSend == false)
	{
		firstSend = true;
		data = calloc(LOG_DATA_LEN, sizeof(uint8_t));

		memset(data, 0, LOG_DATA_LEN);
//		memcpy(data, sotfEmu.flashBuffer + len, LOG_DATA_LEN);
		uint16_t bufCount = 0;
		for(uint8_t i = 0; i < 22; i++)
		{
			u8_buff[3] = sotfEmu.flashBuffer[len + i];
			u8_buff[2] = sotfEmu.flashBuffer[len + i] >> 8;
			u8_buff[1] = sotfEmu.flashBuffer[len + i] >> 16;
			u8_buff[0] = sotfEmu.flashBuffer[len + i] >> 24;
			for(uint8_t j = 0; j < 4; j++)
			{
				data[bufCount + j] = u8_buff[i];
			}
			bufCount+=4;
		}

		data[88] = sotfEmu.flashBuffer[len + 23] >> 16;
		data[89] = sotfEmu.flashBuffer[len + 23] >> 24;
	}

	uint8_t isRecievedMsg = mavlinkMsg_isLogRequestData(2);

	if(isRecievedMsg == 4)
	{
		/// copy new data
		memset(data, 0, LOG_DATA_LEN);
//		memcpy(data, sotfEmu.flashBuffer + len, LOG_DATA_LEN);
		uint16_t bufCount = 0;
		for(uint8_t i = 0; i < 22; i++)
		{
			u8_buff[3] = sotfEmu.flashBuffer[len + i];
			u8_buff[2] = sotfEmu.flashBuffer[len + i] >> 8;
			u8_buff[1] = sotfEmu.flashBuffer[len + i] >> 16;
			u8_buff[0] = sotfEmu.flashBuffer[len + i] >> 24;
			for(uint8_t j = 0; j < 4; j++)
			{
				data[bufCount + j] = u8_buff[i];
			}
			bufCount+=4;
		}

		data[88] = sotfEmu.flashBuffer[len + 23] >> 16;
		data[89] = sotfEmu.flashBuffer[len + 23] >> 24;

		if((len+=LOG_DATA_LEN) > (FILE_LENGTH + LOG_DATA_LEN))
		{
			free(data);

			len 	= 0;
			firstSend 	= false;

			printf("\n[sotfEmulations_sendFlashData] successful\n");

			return TIMEOUT_STATE_DONE;
		}
	}
	else
	{
		mavlinkMsg_send_logData(nextAddress, id, count, data, 2);
		printf("\n[sotfEmulations_sendFlashData] address = %d | len = %d\n", (int)nextAddress, (int)len);
	}

	return TIMEOUT_STATE_RUNNING;
}

/** @brief  sotfEmulations_sendEndCommand
    @return none
*/
static timeOutState_t sotfEmulations_sendEndCommand(sotfEmulations_private_t *sPrivate)
{
	static uint32_t timeSendMsg = 0;

	uint8_t isRecievedEndCmd = mavlinkMsg_isLogRequestEnd(2);

	if(isRecievedEndCmd == 4)
	{
		timeSendMsg = 0;
		printf("\n[sotfEmulations_sendEndCommand] successful\n");
		return TIMEOUT_STATE_DONE;
	}
	else
	{
		if(HAL_GetTick() - timeSendMsg > 1000 || timeSendMsg == 0)
		{
			mavlinkMsg_send_logRequestEnd(2);
		}
	}

	return TIMEOUT_STATE_RUNNING;
}

/** @brief  softEmulations_process
    @return none
*/
void softEmulations_process(void)
{
	static uint8_t state = 0;

	switch(state)
	{
		case 0: /// state read flash
		{
			if(sotfEmulations_readFlash(sotfEmu.flashBuffer))
			{
				state = 2;
			}
		}break;
		case 1: /// state show flash value to console
		{
			uint16_t buffCount = 0;
			for(uint16_t i = 0; i < FILE_LENGTH ; i+=16)
			{
				printf("\n[softEmulations_process] value address 0x%x to address 0x%x\n", (int)(ADDR_FLASH_SECTOR_10 + i), (int)(ADDR_FLASH_SECTOR_10 + i + 16));
				for(uint16_t ii = 0; ii < 4; ii++)
				{
					printf("[softEmulations_process] value = 0x%x\n", (int)sotfEmu.flashBuffer[buffCount + ii]);
				}
				buffCount+=4;
			}

			state = 2;
		}break;
		case 2: /// start command
		{
			timeOutState_t _state = sotfEmulations_sendStartCommand(&sotfPrivate);
			if(_state == TIMEOUT_STATE_DONE)
			{
				state = 3;
			}
		}break;
		case 3: /// send flash data
		{
			timeOutState_t _state = sotfEmulations_sendFlashData(&sotfPrivate);
			if(_state == TIMEOUT_STATE_DONE)
			{
				free(sotfEmu.flashBuffer);
				state = 4;
			}
		}break;
		case 4: /// end command
		{
			timeOutState_t _state = sotfEmulations_sendEndCommand(&sotfPrivate);
			if(_state == TIMEOUT_STATE_DONE)
			{
				state = 5;
			}
		}break;
		case 5: /// done
		{

		}break;
	}
}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

