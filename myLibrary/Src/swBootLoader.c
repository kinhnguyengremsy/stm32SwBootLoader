/**
  ******************************************************************************
  * @file    .c
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
#include "swBootLoader.h"
#include "mavlinkMsgHandle.h"
#include "storageFlash.h"
/* Private typedef------------------------------------------------------------------------------*/

/* Private define------------------------------------------------------------------------------*/

/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
extern mavlinkMsgHandle_t mavlinkCOM2;
extern mavlinkMsgHandle_t mavlinkCOM4;

swBootLoader_t swBL;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __SW_BOOTLOADER_CONFIGURATION
    @{
*/#ifndef __SW_BOOTLOADER_CONFIGURATION
#define __SW_BOOTLOADER_CONFIGURATION
/** @brief	swBootLoaderConfiguration
    @return none
*/
void swBootLoaderConfiguration(void)
{
	mavlinkMsg_configuration();

	storageFlash_configuration();
}

#endif
/**
    @}
*/

/** @group __SW_BOOTLOADER_FUNCTION
    @{
*/#ifndef __SW_BOOTLOADER_FUNCTION
#define __SW_BOOTLOADER_FUNCTION
/** @brief	swBootLoader_jumToApplication
    @return none
*/
static void swBootLoader_jumToApplication(void)
{
	/* Turn off Peripheral, Clear Interrupt Flag*/
	HAL_RCC_DeInit();

	/* Clear Pending Interrupt Request, turn  off System Tick*/
	HAL_DeInit();

	/* Turn off fault harder*/
	SCB->SHCSR &= ~( SCB_SHCSR_USGFAULTENA_Msk |\
	SCB_SHCSR_BUSFAULTENA_Msk | \
	SCB_SHCSR_MEMFAULTENA_Msk ) ;

	/* Set Main Stack Pointer*/
	__set_MSP(*((volatile uint32_t*) ADDRESS_START_APPLICATION));

	uint32_t JumpAddress = *((volatile uint32_t*) (ADDRESS_START_APPLICATION + 4));

	/* Set Program Counter to Blink LED Apptication Address*/
	void (*reset_handler)(void) = (void*)JumpAddress;
	reset_handler();
}

#endif
/**
    @}
*/

/** @group __SW_BOOTLOADER_PROCESS
    @{
*/#ifndef __SW_BOOTLOADER_PROCESS
#define __SW_BOOTLOADER_PROCESS

/** @brief  swBootLoader_wattingStartTranferFile
    @return none
*/
static timeOutState_t swBootLoader_wattingStartTranferFile(void)
{
	(mavlinkMsg_isLogRequestData(1) == 2) ? (swBL.startReciever = 2) : (swBL.startReciever = 0);

	if(swBL.startReciever == 2)
	{
		/// get file length
		swBL.fileLength = mavlinkCOM2.logRequestData.count;

		printf("\n[swBootLoader_stateRecieverData] recieved msg start tranfer file length : %d\n", (int)swBL.fileLength);

		/// send to log request data to sotf
		mavlinkMsg_send_logRequestData(0, swBL.fileLength, 0, 1);

		return TIMEOUT_STATE_DONE;
	}

	return TIMEOUT_STATE_RUNNING;
}

/** @brief  swBootLoader_process
    @return none
*/
static timeOutState_t swBootLoader_stateRecieverData(void)//(uint32_t *buffer)
{
	uint8_t waittingData = 0;
	static uint32_t timeOutRecieverData = 0;
	static uint8_t timeOutCount = 0;
	static uint32_t len = 0;
	static bool eraseFlash = false;
	static uint32_t startAddress = ADDRESS_START_APPLICATION;

	{
		/// kiem tra flag stop tranfer file
		swBL.stopReciever = mavlinkMsg_isLogRequestEnd(1);
		if(swBL.stopReciever == 0)
		{
			/// kiem tra flag reciever file
			waittingData = mavlinkMsg_isLogData(1);
			if(waittingData == 2)
			{
				uint32_t numberOfByteRemain = swBL.fileLength - len;
				/// kiem tra so byte con lai
				if((numberOfByteRemain) < 90)
				{
					len+=numberOfByteRemain;
				}
				else
				{
					len+=LOG_DATA_LEN;
				}


				if(eraseFlash == false)
				{
					eraseFlash = true;

					printf("\n[swBootLoader_stateRecieverData] Erase Flash running\n");

					storageFlash_EraseSector(ADDR_FLASH_SECTOR_5, 0x00020000);
					storageFlash_EraseSector(ADDR_FLASH_SECTOR_6, 0x00020000);
					storageFlash_EraseSector(ADDR_FLASH_SECTOR_7, 0x00020000);
					storageFlash_EraseSector(ADDR_FLASH_SECTOR_8, 0x00020000);
					storageFlash_EraseSector(ADDR_FLASH_SECTOR_9, 0x00020000);

					printf("\n[swBootLoader_stateRecieverData] Erase Flash successful\n");

					storageFlash_writeData(startAddress, mavlinkCOM2.logData.data, LOG_DATA_LEN);
				}
				else
				{
					storageFlash_writeDataNonErase(startAddress, mavlinkCOM2.logData.data, LOG_DATA_LEN);
				}

				startAddress+=LOG_DATA_LEN;
//				printf("\n[swBootLoader_stateRecieverData] done write %d\n", (int)len);

				/// send address current to sotf
				mavlinkMsg_send_logRequestData(len, 0, 0, 1);
			}

			/// check timeOut reciverData
			if(HAL_GetTick() - timeOutRecieverData > 1000)
			{
				timeOutRecieverData = HAL_GetTick();

				if(++timeOutCount > 120)
				{
					timeOutRecieverData = 0;
					timeOutCount = 0;
					len = 0;
					eraseFlash = false;

					printf("\n[swBootLoader_stateRecieverData] reciever data error timeOut\n");

					return TIMEOUT_STATE_ERROR;
				}
			}
		}
		else if(swBL.stopReciever == 2)
		{
			timeOutRecieverData = 0;
			timeOutCount = 0;
			len = 0;
			eraseFlash = false;

			mavlinkMsg_send_logRequestEnd(1);
			printf("\n[swBootLoader_stateRecieverData] reciever data successful\n");

			return TIMEOUT_STATE_DONE;
		}
	}

	return TIMEOUT_STATE_RUNNING;
}

timeOutState_t swBootLoader_writeToFlash(void)
{

//	if(storageFlash_writeData(ADDRESS_START_APPLICATION, swBL.u32_buffer, swBL.fileLength / 4) == false)
//	{
//		return TIMEOUT_STATE_DONE;
//	}

	return TIMEOUT_STATE_RUNNING;
}

/** @brief  swBootLoader_process
    @return none
*/
static timeOutState_t swBootLoader_runReciever(void)
{
	switch(swBL._state)
	{
		case SW_BOOTLOADER_STATE_IDLE:
		{
			/// check mavlink connection
			uint8_t mavlinkConnection = mavlinkMsg_checkConnection(1);
			(mavlinkConnection == 2) ? (swBL._state = SW_BOOTLOADER_STATE_START_RECIVER_DATA) : (swBL._state = SW_BOOTLOADER_STATE_IDLE);
//			storageFlash_test();
//			swBootLoader_jumToApplication();
		}break;
		case SW_BOOTLOADER_STATE_START_RECIVER_DATA:
		{
			timeOutState_t state = swBootLoader_wattingStartTranferFile();
			if(state == TIMEOUT_STATE_DONE)
			{
				swBL._state = SW_BOOTLOADER_STATE_RECIVER_DATA;

				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_RESET);

//				swBL.buffer = calloc(swBL.fileLength, sizeof(uint8_t));
//				swBL.u32_buffer = calloc(swBL.fileLength / 4, sizeof(uint32_t));
			}
		}break;
		case SW_BOOTLOADER_STATE_RECIVER_DATA:
		{
			static uint32_t timeLed = 0;
			if(HAL_GetTick() - timeLed > 200)
			{
				timeLed = HAL_GetTick();

				HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_14);
			}

			timeOutState_t state = swBootLoader_stateRecieverData();
			if(state == TIMEOUT_STATE_DONE)
			{
				timeLed = 0;
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
				swBL._state = SW_BOOTLOADER_STATE_DONE;//SW_BOOTLOADER_STATE_CHECK_FW;

//				printf("\n[swBootLoader_runReciever] file len = %d\n", (int)swBL.fileLength);
//
//				uint32_t u8_buffCount = 0;
//				uint32_t u32_buffCount = 0;
//				while(u32_buffCount < swBL.fileLength / 4)
//				{
//					swBL.u32_buffer[u32_buffCount] = swBL.buffer[u8_buffCount];
//					u8_buffCount++;
//					swBL.u32_buffer[u32_buffCount] |= swBL.buffer[u8_buffCount] << 8;
//					u8_buffCount++;
//					swBL.u32_buffer[u32_buffCount] |= swBL.buffer[u8_buffCount] << 16;
//					u8_buffCount++;
//					swBL.u32_buffer[u32_buffCount] |= swBL.buffer[u8_buffCount] << 24;
//					u8_buffCount++;
//
//					u32_buffCount++;
//				}
//
//				/// free u8 buffer
//				printf("\nfree u8 buffer .............................\n");
//				free(swBL.buffer);

//				for(uint32_t i = 0; i < swBL.fileLength / 4; i+=4)
//				{
//					for(uint8_t j = 0; j < 4; j++)
//					{
//						printf("|0x%x", (int)swBL.u32_buffer[i+j]);
//					}
//					printf("\n");
//				}
			}

		}break;
		case SW_BOOTLOADER_STATE_CHECK_FW:
		{
//			char *fwStr = "GREMSY54760";
//			uint16_t fwStrLen = storageFlash_getStringLen(fwStr);
//			uint32_t buffer[fwStrLen];
//
//			bool checkFw = storageFlash_confirmData(fwStr, buffer, fwStrLen);
//			(checkFw == true) ? (swBL._state = SW_BOOTLOADER_STATE_STORAGE_FLASH) : (swBL._state = SW_BOOTLOADER_STATE_CHECK_FW);

			swBL._state = SW_BOOTLOADER_STATE_STORAGE_FLASH;

		}break;
		case SW_BOOTLOADER_STATE_STORAGE_FLASH:
		{
			timeOutState_t state = swBootLoader_writeToFlash();
			if(state == TIMEOUT_STATE_DONE)
			{
				printf("\nfree u32 buffer .............................\n");
				free(swBL.u32_buffer);

				printf("\nJum to application ...........................\n");
				swBootLoader_jumToApplication();
			}
		}break;
		case SW_BOOTLOADER_STATE_VERIFY_DATA:
		{
			{

			}
		}break;
		case SW_BOOTLOADER_STATE_DONE:
		{
			printf("\nJum to application ...........................\n");
			swBootLoader_jumToApplication();
		}break;
		case SW_BOOTLOADER_STATE_ERROR:
		{

		}break;
	}

	return TIMEOUT_STATE_RUNNING;
}

/** @brief  swBootLoader_process
    @return none
*/
void swBootLoader_process(void)
{
	mavlinkMsg_process();

	#if (IS_RECIEVER == 1)
		swBootLoader_runReciever();
	#else

	#endif
}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

