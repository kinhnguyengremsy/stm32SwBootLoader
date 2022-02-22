/**
  ******************************************************************************
  * @file    uartBootLoader.c
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
#include "uartBootLoader.h"
#if (USE_DEVICE_BOOTLOADER == 1)
#include "serialPort.h"
#include "ringBuffer.h"
#include "storageFlash.h"
/* Private typedef----------------------------------------------------------------------------*/
struct
{
	uint8_t cmdHeader;
	uint8_t cmdFooter;
}bootLoaderCmdList[BOOTLOADER_CMD_GET_TOTAL] = {
//		{.cmdHeader = 0x00, .cmdFooter = 0x00},
		{.cmdHeader = 0x00, .cmdFooter = 0x00},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET				, .cmdFooter = 0xFF},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET_VER			, .cmdFooter = 0xFE},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET_ID			, .cmdFooter = 0xFD},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_MEMORY		, .cmdFooter = 0xEE},
		{.cmdHeader = UART_BOOTLOADER_CMD_GO				, .cmdFooter = 0xDE},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_MEMORY		, .cmdFooter = 0xCE},
		{.cmdHeader = UART_BOOTLOADER_CMD_ERASE				, .cmdFooter = 0xBC},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_PROTECT		, .cmdFooter = 0x9C},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_UNPROTECT	, .cmdFooter = 0x8C},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_PROTECT		, .cmdFooter = 0x7D},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_UNPROTECT	, .cmdFooter = 0x6D},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET_CHECKSUM		, .cmdFooter = 0x5E},
};

struct
{
	char *str;
}bootLoaderCmdStr_t[13] = {
		{.str = "BOOTLOADER_CMD_NONE"},
		{.str = "BOOTLOADER_CMD_GET"},
		{.str = "BOOTLOADER_CMD_GET_VER"},
		{.str = "BOOTLOADER_CMD_GET_ID"},
		{.str = "BOOTLOADER_CMD_READ_MEMORY"},
		{.str = "BOOTLOADER_CMD_GO"},
		{.str = "BOOTLOADER_CMD_WRITE_MEMORY"},
		{.str = "BOOTLOADER_CMD_ERASE"},
		{.str = "BOOTLOADER_CMD_WRITE_PROTECT"},
		{.str = "BOOTLOADER_CMD_WRITE_UNPROTECT"},
		{.str = "BOOTLOADER_CMD_READ_PROTECT"},
		{.str = "BOOTLOADER_CMD_READ_UNPROTECT"},
		{.str = "BOOTLOADER_CMD_GET_CHECKSUM"},
};

/* Private define-----------------------------------------------------------------------------*/

/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
extern serialPort_t 		serial_port2;
extern UART_HandleTypeDef 	huart2;
extern DMA_HandleTypeDef 	hdma_usart2_rx;

uartBootLoader_t 			boot;
ringBuffer_t 				rBufferRxU2;
uint8_t usart2WData;
uint8_t findCmd = BOOTLOADER_CMD_NONE;
/* Private function prototypes----------------------------------------------------------------*/
/* Private functions--------------------------------------------------------------------------*/

/** @group __UART_BOOTLOADER_CONFIGURATION
    @{
*/#ifndef __UART_BOOTLOADER_CONFIGURATION
#define __UART_BOOTLOADER_CONFIGURATION
/** @brief  uartBootLoaderConfiguration
    @return none
*/
void uartBootLoaderConfiguration(void)
{
	huart2.Instance 				= USART2;
	huart2.Init.BaudRate 			= 9600;
	huart2.Init.WordLength 			= UART_WORDLENGTH_9B;
	huart2.Init.StopBits 			= UART_STOPBITS_1;
	huart2.Init.Parity 				= UART_PARITY_EVEN;
	huart2.Init.Mode 				= UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl 			= UART_HWCONTROL_NONE;
	huart2.Init.OverSampling 		= UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}

    /// xoa bo dem uart truoc khi truyen
    __HAL_UART_FLUSH_DRREGISTER(&huart2);

    /// init serialPort library
    serial_port2.zPrivate.uartHandle.hdmarx = &hdma_usart2_rx;
    serial_port2.zPrivate.uartHandle.Instance = USART2;
    serial_port2.isWriteFinish = true;

	ringBufferInit(&rBufferRxU2);

	if(HAL_UART_Receive_DMA(&huart2, &usart2WData, 1) != HAL_OK)
	{
		Error_Handler();
	}
}

#endif
/**
    @}
*/

/** @group __UART_BOOTLOADER_SEND_CMD_FUNCTION
    @{
*/#ifndef __UART_BOOTLOADER_SEND_FUNCTION
#define __UART_BOOTLOADER_SEND_FUNCTION
/** @brief	uartBootLoaderSendAck
    @return	none
*/
static void uartBootLoaderSendAck(void)
{
	uint8_t buffer[2] = {UART_BOOTLOADER_ACK, 0};

	serialPort_write(&serial_port2, buffer, 1);
}

/** @brief	uartBootLoaderSendAck
    @return	none
*/
static void uartBootLoaderSendNack(void)
{
	uint8_t buffer[2] = {UART_BOOTLOADER_NACK, 0};

	serialPort_write(&serial_port2, buffer, 1);
}

/** @brief	uartBootLoaderSendMoreByte
    @return	none
*/
static void uartBootLoaderSendMoreByte(uint8_t *buffer, uint16_t len)
{
	serialPort_write(&serial_port2, buffer, len);
}

/** @brief	uartBootLoaderChecksumCalculator
 *  @param[in] beginChecksum : so checksum ban dau
 *  @param[in] buffer : mang can tinh checksum
 *  @param[in] len : do dai cua mang tinh checksum
    @return	none
*/
uint8_t uartBootLoaderChecksumCalculator(uint8_t beginChecksum, uint8_t *buffer, uint16_t len)
{
	uint8_t checksum = beginChecksum;

	for(uint16_t i = 0 ; i < len; i++)
	{
		checksum ^= buffer[i];
	}

	return checksum;
}

#endif
/**
    @}
*/

/** @group __UART_BOOTLOADER_RECIEVER_CMD_FUNCTIONS
    @{
*/#ifndef __UART_BOOTLOADER_READ_FUNCTION
#define __UART_BOOTLOADER_READ_FUNCTION

/** @brief  uartBootLoaderRecieverCmdConnect
    @return 
*/
static bool uartBootLoaderRecieverCmdConnect(uartBootLoader_t *boot)
{
	uint8_t rData = 0;
	static uint32_t timePrintDebug = 0;

	if(rBufferRxU2.len == 1)
	{
		if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
		{
			if(rData == UART_BOOTLOADER_CMD_CONNECT)
			{
				uartBootLoaderSendAck();
				timePrintDebug = 0;
				printf("\n[uartBootLoaderRecieverCmdConnect] boot connected !@! len = %d\n", rBufferRxU2.len);

				return true;
			}
			else
			{
				uartBootLoaderSendNack();
				printf("\n[uartBootLoaderRecieverCmdConnect] non valid byte !@! len = %d\n", rBufferRxU2.len);
			}
		}
	}


	if(HAL_GetTick() - timePrintDebug > 1000)
	{
		timePrintDebug = HAL_GetTick();
		printf("\n[uartBootLoaderRecieverCmdConnect] waitting cmd connect ...\n");
	}

	return false;
}

/** @brief  uartBootLoaderRecieverCmdGet
    @return bootLoaderGetCmd_t
*/
static bool uartBootLoaderRecieverCmdGet(uartBootLoader_t *boot)
{
	uint8_t rData = 0;
	uint8_t getCmdData[BOOTLOADER_CMD_LEN] = {0x00, 0xFF};
	bootLoaderGetCmd_t cmd;
	static uint8_t trueDataCount = 0;

	if(rBufferRxU2.len == BOOTLOADER_CMD_LEN)
	{
		for(uint8_t i = 0; i < BOOTLOADER_CMD_LEN; i++)
		{
			if(ringBufferRead(&rBufferRxU2, &rData))
			{
				if(rData == getCmdData[i])
				{
					trueDataCount++;
					printf("\n[uartBootLoaderRecieverCmdGet] reciever cmd get value = 0x%x | len = %d\n", (int)rData, rBufferRxU2.len);
				}
			}
		}
	}

	if(trueDataCount == BOOTLOADER_CMD_LEN)
	{
		cmd.numberOfbyte 			= BOOTLOADER_CMD_GET_LEN;
		cmd.version 				= BOOTLOADER_VERSION;
		cmd.getCmd 					= UART_BOOTLOADER_CMD_GET;
		cmd.getVerAndRPStatus 		= UART_BOOTLOADER_CMD_GET_VER;
		cmd.getId 					= UART_BOOTLOADER_CMD_GET_ID;
		cmd.readMemoryCmd 			= UART_BOOTLOADER_CMD_READ_MEMORY;
		cmd.goCmd 					= UART_BOOTLOADER_CMD_GO;
		cmd.writeMemoryCmd 			= UART_BOOTLOADER_CMD_WRITE_MEMORY;
		cmd.EraseCmd 				= UART_BOOTLOADER_CMD_ERASE; /*Erase command or Extended Erase command (exclusive commands)*/
		cmd.writeProtectCmd 		= UART_BOOTLOADER_CMD_WRITE_PROTECT;
		cmd.writeUnProtectCmd 		= UART_BOOTLOADER_CMD_WRITE_UNPROTECT;
		cmd.readOutProtectCmd 		= UART_BOOTLOADER_CMD_READ_PROTECT;
		cmd.readOutUnProtectCmd 	= UART_BOOTLOADER_CMD_READ_UNPROTECT;
		cmd.getChecksumCmd 			= UART_BOOTLOADER_CMD_GET_CHECKSUM;

		uint8_t buffer[BOOTLOADER_CMD_GET_LEN] = {	UART_BOOTLOADER_ACK
													, cmd.numberOfbyte
													, cmd.version
													, cmd.getCmd
													, cmd.getVerAndRPStatus
													, cmd.getId
													, cmd.readMemoryCmd
													, cmd.goCmd
													, cmd.writeMemoryCmd
													, cmd.EraseCmd
													, cmd.writeProtectCmd
													, cmd.writeUnProtectCmd
													, cmd.readOutProtectCmd
													, cmd.readOutUnProtectCmd
													, cmd.getChecksumCmd
													, UART_BOOTLOADER_ACK};

		uartBootLoaderSendMoreByte(buffer, BOOTLOADER_CMD_GET_LEN);

		printf("\n[uartBootLoaderRecieverCmdGet] send Ack byte , data of cmd get , ack byte end...\n");

		trueDataCount = 0;

		return true;
	}


	return false;
}

/** @brief  uartBootLoaderRecieverCmdGetVer
    @return bool
*/
static bool uartBootLoaderRecieverCmdGetVer(uartBootLoader_t *boot)
{
	bootLoaderGetVerCmd_t cmd;
	uint8_t getVerCmd[BOOTLOADER_CMD_LEN] = {0x01, 0xFE};
	uint8_t rData = 0;
	static uint8_t trueDataCount = 0;

	if(rBufferRxU2.len == BOOTLOADER_CMD_LEN)
	{
		for(uint8_t i = 0; i < BOOTLOADER_CMD_LEN; i++)
		{
			if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
			{
				if(rData == getVerCmd[i])
				{
					trueDataCount++;
					printf("\n[uartBootLoaderRecieverCmdGetVer] reciever cmd get ver value = 0x%x | len = %d\n", rData, rBufferRxU2.len);
				}
			}
		}
	}

	if(trueDataCount == BOOTLOADER_CMD_LEN)
	{
		cmd.version = BOOTLOADER_VERSION;
		cmd.optionByte1 = 0x00;
		cmd.optionByte2 = 0x00;

		uint8_t buffer[BOOTLOADER_CMD_GET_VER_LEN] = {	UART_BOOTLOADER_ACK
														, cmd.version
														, cmd.optionByte1
														, cmd.optionByte2
														, UART_BOOTLOADER_ACK};

		uartBootLoaderSendMoreByte(buffer, BOOTLOADER_CMD_GET_VER_LEN);

		printf("\n[uartBootLoaderRecieverCmdGet] send Ack byte and data of cmd get ver ...\n");

		trueDataCount = 0;

		return true;
	}

	return false;
}

/** @brief  uartBootLoaderRecieverCmdGetId
    @return bootLoaderGetIdCmd_t
*/
static bool uartBootLoaderRecieverCmdGetId(uartBootLoader_t *boot)
{
	bootLoaderGetIdCmd_t cmd;
	uint8_t getIdCmd[BOOTLOADER_CMD_LEN] = {0x02, 0xFD};
	uint8_t rData = 0;
	static uint8_t trueDataCount = 0;

	if(rBufferRxU2.len == BOOTLOADER_CMD_LEN)
	{
		for(uint8_t i = 0; i < BOOTLOADER_CMD_LEN; i++)
		{
			if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
			{
				if(rData == getIdCmd[i])
				{
					trueDataCount++;
					printf("\n[uartBootLoaderRecieverCmdGetId] reciever cmd get id value = 0x%x | len = %d\n", rData, rBufferRxU2.len);
				}
			}
		}
	}

	if(trueDataCount == BOOTLOADER_CMD_LEN)
	{
		cmd.numberOfbyte = 5;
		cmd.byte3 = 0x04;
		cmd.byte4 = 0x30;

		uint8_t buffer[BOOTLOADER_CMD_GET_ID_LEN] = {	UART_BOOTLOADER_ACK
														, cmd.numberOfbyte
														, cmd.byte3
														, cmd.byte4
														, UART_BOOTLOADER_ACK};

		uartBootLoaderSendMoreByte(buffer, BOOTLOADER_CMD_GET_ID_LEN);

		printf("\n[uartBootLoaderRecieverCmdGet] send Ack byte and data of cmd get id ...\n");

		trueDataCount = 0;

		return true;
	}

	return false;
}

/** @brief  uartBootLoaderRecieverCmdErase
    @return bool
*/
static bool uartBootLoaderRecieverCmdErase(uartBootLoader_t *boot)
{
	uint8_t getIdCmd[BOOTLOADER_CMD_LEN] = {0x43, 0xBC};
	uint8_t rData = 0;
	static uint8_t trueDataCount = 0;

	if(rBufferRxU2.len == BOOTLOADER_CMD_LEN)
	{
		for(uint8_t i = 0; i < BOOTLOADER_CMD_LEN; i++)
		{
			if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
			{
				if(rData == getIdCmd[i])
				{
					trueDataCount++;
					printf("\n[uartBootLoaderRecieverCmdErase] reciever cmd get id value = 0x%x | len = %d\n", rData, rBufferRxU2.len);
				}
			}
		}
	}

	if(trueDataCount == BOOTLOADER_CMD_LEN)
	{
		/// erase flash sector 5
		if(storageFlash_EraseSector(ADDR_FLASH_SECTOR_5, 0x00020000) == 0)
		{
			uint8_t buffer[BOOTLOADER_CMD_ERASE_LEN] = {UART_BOOTLOADER_ACK};

			uartBootLoaderSendMoreByte(buffer, BOOTLOADER_CMD_ERASE_LEN);

			printf("\n[uartBootLoaderRecieverCmdErase] send Ack byte and data of cmd erase ...\n");

			trueDataCount = 0;

			return true;
		}
	}

	return false;
}

/** @brief  uartBootLoaderRecieverCmdWriteMem1
    @return bool
*/
static bool uartBootLoaderRecieverCmdWriteMem1(void)
{
	uint8_t getWriteMem[BOOTLOADER_CMD_LEN] = {0x31, 0xCE};
	uint8_t rData = 0;
	static uint8_t trueDataCount = 0;

	if(rBufferRxU2.len == BOOTLOADER_CMD_LEN)
	{
		for(uint8_t i = 0; i < BOOTLOADER_CMD_LEN; i++)
		{
			if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
			{
				if(rData == getWriteMem[i])
				{
					trueDataCount++;
					printf("\n[uartBootLoaderRecieverCmdWriteMem1] reciever cmd write mem1 value = 0x%x | len = %d\n", rData, rBufferRxU2.len);
				}
			}
		}
	}

	if(trueDataCount == BOOTLOADER_CMD_LEN)
	{
		uartBootLoaderSendAck();

		printf("\n[uartBootLoaderRecieverCmdWriteMem1] send Ack byte ...\n");

		return true;
	}

	return false;
}

/** @brief  uartBootLoaderRecieverCmdWriteMem2
    @return bootLoaderCmdWriteResult_t
*/
static bootLoaderCmdWriteResult_t uartBootLoaderRecieverCmdWriteMem2(uartBootLoader_t *boot)
{
	static bool cmdWrite = false;
	uint8_t startAddressBuffer[5]; /// start address and checksum buffer
	uint8_t rData = 0;
	const uint8_t numberOfbyteRec = 5;

	memset(startAddressBuffer, 0, 5);

	/// ktra cmd write
	if(cmdWrite == true)
	{
		/// cho nhan 4 byte address va 1 byte checksum
		if(rBufferRxU2.len == numberOfbyteRec)
		{
			for(uint8_t i = 0; i < numberOfbyteRec; i++)
			{
				if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
				{
					startAddressBuffer[i] = rData;
					printf("\n[uartBootLoaderRecieverCmdWriteMem2] reciever cmd write mem2 value = 0x%x | len = %d\n", rData, rBufferRxU2.len);
				}
			}

			uint8_t checksum = uartBootLoaderChecksumCalculator(0, startAddressBuffer, 4);

			printf("\n[uartBootLoaderRecieverCmdWriteMem2] checksum calculator value = 0x%x | checksum rec value = 0x%x\n", checksum, startAddressBuffer[4]);

			if(checksum == startAddressBuffer[4])
			{
				boot->flashAddress = startAddressBuffer[0];
				boot->flashAddress |= startAddressBuffer[1] << 8;
				boot->flashAddress |= startAddressBuffer[2] << 16;
				boot->flashAddress |= startAddressBuffer[3] << 24;

				uartBootLoaderSendAck();

				printf("\n[uartBootLoaderRecieverCmdWriteMem2] send Ack byte ...\n");

				cmdWrite = false;

				return BOOTLOADER_CMD_WRITE_RESULT_OK;
			}
			else
			{
				uartBootLoaderSendNack();

				printf("\n[uartBootLoaderRecieverCmdWriteMem2] send Nack byte ...\n");

				return BOOTLOADER_CMD_WRITE_RESULT_ERROR;
			}
		}
	}
	else
	{
		cmdWrite = uartBootLoaderRecieverCmdWriteMem1();
	}

	return BOOTLOADER_CMD_WRITE_RESULT_IDLE;
}

/** @brief  uartBootLoaderRecieverCmdWriteMem2
    @return bootLoaderCmdWriteResult_t
*/
static bootLoaderCmdWriteResult_t uartBootLoaderRecieverCmdWriteMem3(uartBootLoader_t *boot)
{
	uint8_t rData = 0;
	static bootLoaderCmdWriteResult_t result;
	static uint16_t numberOfBytes = 0;
	static uint8_t state = 0;
	static uint8_t payload[256];



	if(result == BOOTLOADER_CMD_WRITE_RESULT_OK)
	{
		/// cho nhan 1 byte (number of bytes), max 256 byte data, 1 byte checksum
		switch(state)
		{
			case 0:
			{
				/// nhan 1 byte number of bytes
				if(rBufferRxU2.len == 1)
				{
					if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
					{
						numberOfBytes = rData;
						printf("\n[uartBootLoaderRecieverCmdWriteMem3] have %d bytes\n", (int)numberOfBytes);

						state = 1;
					}
				}
			}break;
			case 1:
			{
				/// cho nhan data byte
				if(rBufferRxU2.len == numberOfBytes)
				{
					memset(payload, 0, 256);

					for(uint16_t i = 1; i < numberOfBytes + 1; i++)
					{
						if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
						{
							///copy data to buffer
							payload[i] = rData;

//							printf("\n[uartBootLoaderRecieverCmdWriteMem3] byte %d = 0x%x\n", (int)i, (int)rData);
						}
					}

					state = 2;
				}
			}break;
			case 2:
			{
				/// cho nhan checksum
				if(rBufferRxU2.len == 1)
				{
					if(ringBufferRead(&rBufferRxU2, &rData))
					{
						uint8_t checksum = uartBootLoaderChecksumCalculator(numberOfBytes, payload, numberOfBytes + 1);

						if(checksum == payload[numberOfBytes])
						{
							numberOfBytes = 0;
							state = 0;

							uartBootLoaderSendAck();

							printf("\n[uartBootLoaderRecieverCmdWriteMem3] send Ack byte ...\n");

							printf("\n[uartBootLoaderRecieverCmdWriteMem3] reciever cmd write successful !!!\n");

							result = BOOTLOADER_CMD_WRITE_RESULT_IDLE;
							numberOfBytes = 0;
							state = 0;

							return BOOTLOADER_CMD_WRITE_RESULT_OK;
						}
					}
				}
			}break;
		}
	}
	else if(result == BOOTLOADER_CMD_WRITE_RESULT_ERROR)
	{
		uartBootLoaderSendNack();

		printf("\n[uartBootLoaderRecieverCmdWriteMem3] send Nack byte ...\n");

		return result;
	}
	else
	{
		result = uartBootLoaderRecieverCmdWriteMem2(boot);
	}

	return BOOTLOADER_CMD_WRITE_RESULT_IDLE;
}

/** @brief  uartBootLoaderStateConnected
    @return none
*/
static bootLoaderCmd_t uartBootLoaderReadCmd(void)
{
	uint8_t rData = 0;
	uint8_t cmdBuffer[2] = {0, 0};
	uint8_t trueCmdCount = 0;

	if(rBufferRxU2.len == BOOTLOADER_CMD_LEN)
	{
		/// lay header byte
		if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
		{
			cmdBuffer[0] = rData;
		}

		/// lay footer byte
		if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
		{
			cmdBuffer[1] = rData;
		}

		for(uint8_t i = 0; i < BOOTLOADER_CMD_GET_TOTAL; i++)
		{
			/// compare cmd header
			if(cmdBuffer[0] == bootLoaderCmdList[i].cmdHeader)
			{
				trueCmdCount++;
			}

			/// compare cmd header
			if(cmdBuffer[1] == bootLoaderCmdList[i].cmdFooter)
			{
				trueCmdCount++;
			}

			/// compare true
			if(trueCmdCount == BOOTLOADER_CMD_LEN)
			{
				printf("\n[uartBootLoaderReadCmd] found %s\n", bootLoaderCmdStr_t[i].str);
				return (bootLoaderCmd_t)i;
			}
			else
			{
				trueCmdCount = 0;
			}
		}

		printf("\n[uartBootLoaderReadCmd] not found cmd | byte1 = 0x%x byte2 = 0x%x\n", cmdBuffer[0], cmdBuffer[1]);

		uartBootLoaderSendNack();

		printf("\n[uartBootLoaderReadCmd] send Nack byte ...\n");
	}

	return BOOTLOADER_CMD_NONE;
}

/** @brief  uartBootLoaderResponseCmdGet
    @return bool
*/
static bool uartBootLoaderResponseCmdGet(void)
{
	bootLoaderGetCmd_t cmd;

	cmd.numberOfbyte 			= 11;
	cmd.version 				= BOOTLOADER_VERSION;
	cmd.getCmd 					= UART_BOOTLOADER_CMD_GET;
	cmd.getVerAndRPStatus 		= UART_BOOTLOADER_CMD_GET_VER;
	cmd.getId 					= UART_BOOTLOADER_CMD_GET_ID;
	cmd.readMemoryCmd 			= UART_BOOTLOADER_CMD_READ_MEMORY;
	cmd.goCmd 					= UART_BOOTLOADER_CMD_GO;
	cmd.writeMemoryCmd 			= UART_BOOTLOADER_CMD_WRITE_MEMORY;
	cmd.EraseCmd 				= UART_BOOTLOADER_CMD_ERASE; /*Erase command or Extended Erase command (exclusive commands)*/
	cmd.writeProtectCmd 		= UART_BOOTLOADER_CMD_WRITE_PROTECT;
	cmd.writeUnProtectCmd 		= UART_BOOTLOADER_CMD_WRITE_UNPROTECT;
	cmd.readOutProtectCmd 		= UART_BOOTLOADER_CMD_READ_PROTECT;
	cmd.readOutUnProtectCmd 	= UART_BOOTLOADER_CMD_READ_UNPROTECT;
	cmd.getChecksumCmd 			= UART_BOOTLOADER_CMD_GET_CHECKSUM;

	uint8_t buffer[BOOTLOADER_CMD_GET_LEN] = {	UART_BOOTLOADER_ACK
												, cmd.numberOfbyte
												, cmd.version
												, cmd.getCmd
												, cmd.getVerAndRPStatus
												, cmd.getId
												, cmd.readMemoryCmd
												, cmd.goCmd
												, cmd.writeMemoryCmd
												, cmd.EraseCmd
												, cmd.writeProtectCmd
												, cmd.writeUnProtectCmd
												, cmd.readOutProtectCmd
												, cmd.readOutUnProtectCmd
												, cmd.getChecksumCmd
												, UART_BOOTLOADER_ACK};

	uartBootLoaderSendMoreByte(buffer, BOOTLOADER_CMD_GET_LEN);

	printf("\n[uartBootLoaderRecieverCmdGet] send Ack byte , data of cmd get , ack byte end...\n");

	return true;
}

/** @brief  uartBootLoaderResponseCmdGetVer
    @return bool
*/
static bool uartBootLoaderResponseCmdGetVer(void)
{
	bootLoaderGetVerCmd_t cmd;

	cmd.version = BOOTLOADER_VERSION;
	cmd.optionByte1 = 0x0a;
	cmd.optionByte2 = 0x0a;

	uint8_t buffer[BOOTLOADER_CMD_GET_VER_LEN] = {	UART_BOOTLOADER_ACK
													, cmd.version
													, cmd.optionByte1
													, cmd.optionByte2
													, UART_BOOTLOADER_ACK};

	uartBootLoaderSendMoreByte(buffer, BOOTLOADER_CMD_GET_VER_LEN);

	printf("\n[uartBootLoaderResponseCmdGetVer] send Ack byte, data of cmd get ver and ack byte end...\n");

	return true;
}

/** @brief  uartBootLoaderResponseCmdGetId
    @return bool
*/
static bool uartBootLoaderResponseCmdGetId(void)
{
	bootLoaderGetIdCmd_t cmd;

	cmd.numberOfbyte = 1;
	cmd.byte3 = 0x04;
	cmd.byte4 = 0x13;

	uint8_t buffer[BOOTLOADER_CMD_GET_ID_LEN] = {	UART_BOOTLOADER_ACK
													, cmd.numberOfbyte
													, cmd.byte3
													, cmd.byte4
													, UART_BOOTLOADER_ACK};

	uartBootLoaderSendMoreByte(buffer, BOOTLOADER_CMD_GET_ID_LEN);

	printf("\n[uartBootLoaderResponseCmdGetId] send Ack byte, data of cmd get id and ack byte end...\n");

	return true;
}

/** @brief  uartBootLoaderResponseCmdReadMem
    @return bool
*/
static bool uartBootLoaderResponseCmdReadMem(void)
{
	uint8_t rData = 0;
	uint8_t addressBuffer[5] = {0, 0, 0, 0, 0};
	uint8_t nBuffer[2] = {0, 0};
	static uint8_t state = 0;

	switch(state)
	{
		case 0:
		{
			uartBootLoaderSendAck();

			printf("\n[uartBootLoaderResponseCmdGetId] send Ack byte 1...\n");

			state = 1;
		}break;
		case 1:
		{
			if(rBufferRxU2.len == 5)
			{
				for(uint8_t i = 0; i < 5;i++)
				if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
				{
					printf("\n[uartBootLoaderResponseCmdReadMem] reciever byte = 0x%x | len = %d\n", rData, rBufferRxU2.len);
					addressBuffer[i] = rData;
				}

				uint8_t checksum = uartBootLoaderChecksumCalculator(0, addressBuffer, 4);
				printf("\n[uartBootLoaderResponseCmdReadMem] address checksum = 0x%x\n", checksum);

				if(checksum == addressBuffer[4])
				{
					uartBootLoaderSendAck();

					printf("\n[uartBootLoaderResponseCmdGetId] send Ack byte 2...\n");

					state = 2;
				}
				else
				{
					uartBootLoaderSendNack();

					printf("\n[uartBootLoaderResponseCmdGetId] send nack byte 2...\n");
					state = 10;
				}

			}
		}break;
		case 2:
		{
			if(rBufferRxU2.len == 2)
			{
				for(uint8_t i = 0 ; i < 2; i++)
				if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
				{
					printf("\n[uartBootLoaderResponseCmdReadMem] reciever byte = 0x%x | len = %d\n", rData, rBufferRxU2.len);
					nBuffer[i] = rData;
				}

				uint8_t checksum = uartBootLoaderChecksumCalculator(0, nBuffer, 2);
				printf("\n[uartBootLoaderResponseCmdReadMem] numberOfByte checksum = 0x%x\n", checksum);

				if(checksum != nBuffer[1])
				{
					uartBootLoaderSendNack();

					printf("\n[uartBootLoaderResponseCmdGetId] send nack byte 3...\n");
					state = 10;
				}

				state = 3;
			}
		}break;
		case 3:
		{
			uint8_t *buffer;
			buffer = calloc((nBuffer[0] + 2), sizeof(uint8_t));

			for(uint8_t i = 0; i < nBuffer[0] + 1; i++)
			{
				*buffer++ = 123;
			}

			*buffer++ = UART_BOOTLOADER_ACK;

			uartBootLoaderSendMoreByte(buffer, 2);

			free(buffer);

			state = 0;

			return true;
		}break;
	}

	return false;
}

/** @brief  uartBootLoaderResponseCmdReadOP
    @return bool
*/
static bool uartBootLoaderResponseCmdReadOP(void)
{
	uartBootLoaderSendNack();

	printf("\n[uartBootLoaderResponseCmdReadOP] send nack byte ...\n");

	return true;
}

/** @brief  uartBootLoaderResponseCmdReadOUP
    @return bool
*/
static bool uartBootLoaderResponseCmdReadOUP(void)
{
	uartBootLoaderSendAck();
	uartBootLoaderSendAck();
	printf("\n[uartBootLoaderResponseCmdGetId] send Ack 2 byte...\n");

	return true;
}

/** @brief  uartBootLoaderStateConnected
    @return bool
*/
static void uartBootLoaderStateConnected(uartBootLoader_t *boot)
{

	switch(boot->rCmd)
	{
		case BOOTLOADER_CMD_NONE:
		{
			/// waitting cmd
			boot->rCmd = uartBootLoaderReadCmd();
		}break;
		case BOOTLOADER_CMD_GET:
		{
			if(uartBootLoaderResponseCmdGet() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
		}break;
		case BOOTLOADER_CMD_GET_VER:
		{
			if(uartBootLoaderResponseCmdGetVer() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
		}break;
		case BOOTLOADER_CMD_GET_ID:
		{
			if(uartBootLoaderResponseCmdGetId() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
		}break;
		case BOOTLOADER_CMD_READ_MEMORY:
		{
			if(uartBootLoaderResponseCmdReadMem() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
		}break;
		case BOOTLOADER_CMD_GO:
		{

		}break;
		case BOOTLOADER_CMD_WRITE_MEMORY:
		{

		}break;
		case BOOTLOADER_CMD_ERASE:
		{

		}break;
		case BOOTLOADER_CMD_WRITE_PROTECT:
		{

		}break;
		case BOOTLOADER_CMD_WRITE_UNPROTECT:
		{

		}break;
		case BOOTLOADER_CMD_READ_PROTECT:
		{
			if(uartBootLoaderResponseCmdReadOP() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
		}break;
		case BOOTLOADER_CMD_READ_UNPROTECT:
		{
			if(uartBootLoaderResponseCmdReadOUP() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
		}break;
		case BOOTLOADER_CMD_GET_CHECKSUM:
		{

		}break;
	}
}

#endif
/**
    @}
*/

/** @group __UART_BOOTLOADER_PROCESS
    @{
*/#ifndef __UART_BOOTLOADER_PROCESS
#define __UART_BOOTLOADER_PROCESS
/** @brief  uartBootLoaderProcess
    @return none
*/
void uartBootLoaderProcess(void)
{

	switch(boot.state)
	{
		case BOOTLOADER_STATE_IDLE:
		{
			if(uartBootLoaderRecieverCmdConnect(&boot) == true)
			{
				boot.state = BOOTLOADER_STATE_CONNECTED;
			}
		}break;
		case BOOTLOADER_STATE_CONNECTED:
		{
//			static bootLoaderGetState_t state = BOOTLOADER_GET_STATE_IDLE;
//			switch(state)
//			{
//				case BOOTLOADER_GET_STATE_IDLE:
//				{
//					state = BOOTLOADER_GET_STATE_CMD_GET_ID;
//				}break;
//				case BOOTLOADER_GET_STATE_CMD_GET_ID:
//				{
//					if(uartBootLoaderRecieverCmdGetId(&boot) == true)
//					{
//						state = BOOTLOADER_GET_STATE_CMD_GET;
//					}
//				}break;
//				case BOOTLOADER_GET_STATE_CMD_GET:
//				{
//					if(uartBootLoaderRecieverCmdGet(&boot) == true)
//					{
//						state = BOOTLOADER_GET_STATE_CMD_GET_ID2;
//					}
//				}break;
//				case BOOTLOADER_GET_STATE_CMD_GET_ID2:
//				{
//					if(uartBootLoaderRecieverCmdGetId(&boot) == true)
//					{
//						state = BOOTLOADER_GET_STATE_CMD_GET_VER;
//					}
//				}break;
//				case BOOTLOADER_GET_STATE_CMD_GET_VER:
//				{
//					if(uartBootLoaderRecieverCmdGetVer(&boot) == true)
//					{
//						state = BOOTLOADER_GET_STATE_DONE;
//					}
//				}break;
//				case BOOTLOADER_GET_STATE_DONE:
//				{
//					boot.state = BOOTLOADER_STATE_ERASE;
//				}break;
//			}
			uartBootLoaderStateConnected(&boot);
		}break;
		case BOOTLOADER_STATE_ERASE:
		{
			if(uartBootLoaderRecieverCmdErase(&boot) == true)
			{
				boot.state = BOOTLOADER_STATE_WRITE;
			}
		}break;
		case BOOTLOADER_STATE_WRITE:
		{
			static uint8_t writeState = 0;
			switch(writeState)
			{
				case 0:
				{
					bootLoaderCmdWriteResult_t result = uartBootLoaderRecieverCmdWriteMem3(&boot);
					if(result == BOOTLOADER_CMD_WRITE_RESULT_OK)
					{
						boot.state = BOOTLOADER_STATE_DONE;
					}
					else if(result == BOOTLOADER_CMD_WRITE_RESULT_ERROR)
					{
						boot.state = BOOTLOADER_STATE_ERROR;
					}

				}break;
				case 1:
				{

				}break;
			}
		}break;
		case BOOTLOADER_STATE_DONE:
		{

		}break;
		case BOOTLOADER_STATE_ERROR:
		{

		}break;
	}
}

#endif
/**
    @}
*/
#endif
/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

