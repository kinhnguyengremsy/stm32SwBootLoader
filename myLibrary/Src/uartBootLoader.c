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
struct _bootLoaderCmdList
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
		{.cmdHeader = UART_BOOTLOADER_CMD_ERASE				, .cmdFooter = 0xBB},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_PROTECT		, .cmdFooter = 0x9C},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_UNPROTECT	, .cmdFooter = 0x8C},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_PROTECT		, .cmdFooter = 0x7D},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_UNPROTECT	, .cmdFooter = 0x6D},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET_CHECKSUM		, .cmdFooter = 0x5E},
};

struct _bootLoaderCmdStr_t
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

bool txComplete;
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
	huart2.Init.BaudRate 			= 115200 ;
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

	HAL_UART_Transmit_DMA(&huart2, buffer, 1);
	while(txComplete == false)
	{

	}
	txComplete = false;

//	serialPort_write(&serial_port2, buffer, 1);
}

/** @brief	uartBootLoaderSendAck
    @return	none
*/
static void uartBootLoaderSendNack(void)
{
	uint8_t buffer[2] = {UART_BOOTLOADER_NACK, 0};

	HAL_UART_Transmit_DMA(&huart2, buffer, 1);
	while(txComplete == false)
	{

	}
	txComplete = false;

//	serialPort_write(&serial_port2, buffer, 1);
}

/** @brief	uartBootLoaderSendMoreByte
    @return	none
*/
static void uartBootLoaderSendMoreByte(uint8_t *buffer, uint16_t len)
{
//	serialPort_write(&serial_port2, buffer, len);
	HAL_UART_Transmit_DMA(&huart2, buffer, len);
	while(txComplete == false)
	{

	}
	txComplete = false;
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
//												, cmd.getChecksumCmd
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
	static uint32_t rAddress = 0;
	static uint32_t oldRaddress = 0x08000000;
	static uint32_t realAddress = ADDRESS_START_APPLICATION;
	uint8_t nBuffer[2] = {0, 0};
	static uint8_t state = 0;

	switch(state)
	{
		case 0:
		{
			uartBootLoaderSendAck();

			printf("\n[uartBootLoaderResponseCmdReadMem] send Ack byte 1...\n");

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

				rAddress = addressBuffer[3];
				rAddress |= addressBuffer[2] << 8;
				rAddress |= addressBuffer[1] << 16;
				rAddress |= addressBuffer[0] << 24;

				uint8_t checksum = uartBootLoaderChecksumCalculator(0, addressBuffer, 4);
				uint8_t rChecksum = addressBuffer[4];
				printf("\n[uartBootLoaderResponseCmdReadMem] address = 0x%x | checksum = 0x%x | rChecksum = 0x%x\n", (int)rAddress, (int)checksum, (int)rChecksum);

				if(checksum == rChecksum)
				{
					uartBootLoaderSendAck();

					printf("\n[uartBootLoaderResponseCmdReadMem] send Ack byte 2...\n");

					state = 2;
				}
				else
				{
					uartBootLoaderSendNack();

					printf("\n[uartBootLoaderResponseCmdReadMem] send nack byte 2...\n");
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

				uint8_t checksum = 0;
				checksum = nBuffer[0];
				checksum ^= 0xff;

				uint8_t rChecksum = nBuffer[1];

				printf("\n[uartBootLoaderResponseCmdReadMem] checksum = 0x%x | rChecksum = %d\n", checksum, rChecksum);

				if(checksum != rChecksum)
				{
					uartBootLoaderSendNack();

					printf("\n[uartBootLoaderResponseCmdReadMem] send nack byte 3...\n");
					state = 10;
				}
				else
				{
					uartBootLoaderSendAck();

					printf("\n[uartBootLoaderResponseCmdReadMem] send Ack byte 2...\n");
				}

				state = 3;
			}
		}break;
		case 3:
		{
			uint8_t buffer[STM32_MAX_FRAME - 1];

			uint32_t addressOffset = rAddress - oldRaddress;
			oldRaddress = rAddress;
			realAddress += addressOffset;

			printf("\n[uartBootLoaderResponseCmdReadMem] addressOffset = 0x%x | realAddress = 0x%x\n", (int)addressOffset, (int)realAddress);

			storageFlash_readData(realAddress, buffer, STM32_MAX_FRAME - 1);

			HAL_UART_Transmit_DMA(&huart2, buffer, STM32_MAX_FRAME - 1);
			while(txComplete == false)
			{

			}
			txComplete = false;

			printf("\n[uartBootLoaderResponseCmdReadMem] send %d successful\n", STM32_MAX_FRAME - 1);

			state = 0;
			rAddress = 0;

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

/** @brief  usartBootLoaderFindValue
    @return bool
*/
static uint16_t usartBootLoaderFindValue(uint32_t value, uint32_t *buffer, uint16_t len)
{
	for(uint16_t i = 0; i < len; i++)
	{
		if(value == buffer[i])
		{
			return i;
		}
	}

	return 0;
}

/** @brief  uartBootLoaderResponseCmdErase
    @return bool
*/
static bool uartBootLoaderResponseCmdErase(void)
{
	static uint8_t state = 0;
	uint8_t rData = 0;
//	uint8_t massEraseBuffer[3] = {0xff, 0xff, 0x00};
	uint16_t externedEraseBuffer[3] = {0xffff, 0xfffe, 0xfffd};
	uint16_t externedErase = 0;
	uint8_t eraseBuffer[2] = {0, 0};
	uint16_t trueDataCount = 0;
	uint32_t startSectorAddress = ADDRESS_START_APPLICATION;
	uint32_t offsets = 0x00020000;

	switch(state)
	{
		case 0:
		{
			/// response ack to host
			uartBootLoaderSendAck();
			printf("\n[uartBootLoaderResponseCmdErase] send Ack byte ...\n");

			state = 1;
		}break;
		case 1:
		{
			if(rBufferRxU2.len == 2)
			{
				for(uint8_t i = 0; i < 2; i++)
				{
					if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
					{
						eraseBuffer[i] = rData;
						printf("\n[uartBootLoaderResponseCmdErase] reciever byte = 0x%x | len = %d\n", rData, rBufferRxU2.len);
					}
				}

				externedErase = eraseBuffer[1] << 8 | eraseBuffer[0];
				printf("\n[uartBootLoaderResponseCmdErase] externedErase = 0x%x\n", rData, rBufferRxU2.len);

				trueDataCount = usartBootLoaderFindValue((uint32_t)externedErase, (uint32_t)externedEraseBuffer, 3);

				switch(trueDataCount)
				{
					case 0:
					{
						storageFlash_EraseSector(startSectorAddress, offsets);
						storageFlash_EraseSector(startSectorAddress + offsets, offsets);
					}break;
					case 1: /// mass Erase
					{

					}break;
					case 2: /// Bank1 Erase
					{

					}break;
					case 3: /// Bank2 Erase
					{

					}break;
				}

				if(trueDataCount == 3)
				{
					/// response ack to host
					uartBootLoaderSendAck();
					printf("\n[uartBootLoaderResponseCmdErase] send Ack byte ...\n");
				}

				state = 2;
			}
		}break;
		case 2:
		{
			state = 0;

			return true;
		}break;
	}

	return false;
}


/** @brief  uartBootLoaderResponseCmdWriteMem
    @return bool
*/
static bool uartBootLoaderResponseCmdWriteMem(void)
{
	static uint8_t state = 0;
	uint8_t rData = 0;
	uint8_t addressBuffer[5] = {0, 0, 0, 0, 0};
	static uint32_t rAddress = 0;
	static uint32_t oldRaddress = 0x08000000;
	static uint32_t realAddress = ADDRESS_START_APPLICATION;
	uint8_t checksum = 0;

	switch(state)
	{
		case 0:
		{
			/// response ack to host
			uartBootLoaderSendAck();
			printf("\n[uartBootLoaderResponseCmdWriteMem] send Ack byte...\n");

			state = 1;
		}break;
		case 1:
		{
			/// read address and checksum
			if(rBufferRxU2.len == 5)
			{
				for(uint8_t i = 0; i < 5; i++)
				{
					if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
					{
						addressBuffer[i] = rData;
						printf("\n[uartBootLoaderResponseCmdWriteMem] reciever byte = 0x%x | len = %d\n", rData, rBufferRxU2.len);
					}
				}

				rAddress = addressBuffer[3];
				rAddress |= addressBuffer[2] << 8;
				rAddress |= addressBuffer[1] << 16;
				rAddress |= addressBuffer[0] << 24;

				checksum = uartBootLoaderChecksumCalculator(0, addressBuffer, 4);

				printf("\n[uartBootLoaderResponseCmdWriteMem] reciever address = 0x%x | checksum = %d\n", (int)rAddress, (int)checksum);

				if(checksum == addressBuffer[4])
				{
					uartBootLoaderSendAck();

					printf("\n[uartBootLoaderResponseCmdWriteMem] send Ack byte...\n");
					state = 2;
				}
				else
				{
					uartBootLoaderSendNack();

					printf("\n[uartBootLoaderResponseCmdWriteMem] state 1 send nack byte...\n");
					state = 10;
				}
			}
		}break;
		case 2:
		{
			state = 3;
		}break;
		case 3:
		{
			uint8_t data[STM32_MAX_FRAME + 2];
			uint8_t rChecksum = 0;

			if(rBufferRxU2.len == STM32_MAX_FRAME + 2)
			{
				for(uint16_t i = 0; i < STM32_MAX_FRAME + 2; i++)
				{
					if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
					{
						data[i] = rData;
//						printf("\n[uartBootLoaderResponseCmdWriteMem] reciever byte = 0x%x | len = %d\n", data[i], rBufferRxU2.len);
					}
				}
				/*data[0] : number of bytes
				data[1 - 256] : 256 byte data
				data[257] : checksum*/
				checksum = uartBootLoaderChecksumCalculator(0, data + 1, 256);
				checksum ^= data[0];

				rChecksum = data[STM32_MAX_FRAME + 1];

				printf("\n[uartBootLoaderResponseCmdWriteMem] checksum = %d | rChecksum = %d\n", checksum, rChecksum);

				if(checksum == rChecksum)
				{
					uint32_t addressOffset = rAddress - oldRaddress;
					oldRaddress = rAddress;
					realAddress += addressOffset;

					printf("\n[uartBootLoaderResponseCmdWriteMem] addressOffset = 0x%x | realAddress = 0x%x\n", (int)addressOffset, (int)realAddress);

					/// write to flash
					storageFlash_writeData(realAddress, data + 1, 255);

					/// send ack
					uartBootLoaderSendAck();

					printf("\n[uartBootLoaderResponseCmdWriteMem] send Ack byte...\n");
					state = 4;
				}
				else
				{
					uartBootLoaderSendNack();

					printf("\n[uartBootLoaderResponseCmdWriteMem] state 3 send nack byte...\n");
				}
			}
		}break;
		case 4:
		{
			state = 0;
			rAddress = 0;

			return true;
		}break;
	}

	return false;
}

/** @brief  uartBootLoaderResponseCmdGo
    @return bool
*/
static bool uartBootLoaderResponseCmdGo(void)
{
	static uint8_t state = 0;
	uint8_t rData = 0;
	uint8_t addressBuffer[5] = {0, 0, 0, 0, 0};
	uint32_t rAddress = 0;

	switch(state)
	{
		case 0:
		{
			/// response ack to host
			uartBootLoaderSendAck();
			printf("\n[uartBootLoaderResponseCmdGo] send Ack byte...\n");

			state = 1;
		}break;
		case 1:
		{
			if(rBufferRxU2.len == 5)
			{
				for(uint8_t i = 0; i < 5; i++)
				{
					if(ringBufferRead(&rBufferRxU2, &rData) == RING_BUFFER_OK)
					{
						addressBuffer[i] = rData;
						printf("\n[uartBootLoaderResponseCmdGo] reciever byte = 0x%x | len = %d\n", rData, rBufferRxU2.len);
					}
				}

				rAddress = addressBuffer[0];
				rAddress |= addressBuffer[1] << 8  ;
				rAddress |= addressBuffer[2] << 16 ;
				rAddress |= addressBuffer[3] << 24 ;

				uint8_t checksum = uartBootLoaderChecksumCalculator(0, addressBuffer, 4);
				uint8_t rChecksum = addressBuffer[4];

				printf("\n[uartBootLoaderResponseCmdGo] checksum = 0x%x | rChecksum = %d\n", checksum, rChecksum);

				if(checksum == rChecksum)
				{
					/// send ack to host
					uartBootLoaderSendAck();
					printf("\n[uartBootLoaderResponseCmdGo] send Ack byte...\n");

					state = 2;
				}
				else
				{
					uartBootLoaderSendNack();

					printf("\n[uartBootLoaderResponseCmdGo] state 1 send nack byte...\n");
					state = 10;
				}
			}
		}break;
		case 2:
		{
			state = 0;

			return true;
		}break;
	}

	return false;
}

/** @brief  uartBootLoaderStateConnected
    @return bool
*/
static void uartBootLoaderStateConnected(uartBootLoader_t *boot)
{

	switch((uint8_t)boot->rCmd)
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
			if(uartBootLoaderResponseCmdGo() == true)
			{
				boot->state = BOOTLOADER_STATE_DONE;
				printf("\n[uartBootLoaderStateConnected] jum to application\n");
			}
		}break;
		case BOOTLOADER_CMD_WRITE_MEMORY:
		{
			if(uartBootLoaderResponseCmdWriteMem() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
		}break;
		case BOOTLOADER_CMD_ERASE:
		{
			if(uartBootLoaderResponseCmdErase() == true)
			{
				boot->rCmd = BOOTLOADER_CMD_NONE;
			}
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

/** @brief	uartBootLoaderJumToApplication
    @return none
*/
static void uartBootLoaderJumToApplication(void)
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
			uartBootLoaderStateConnected(&boot);
		}break;
		case BOOTLOADER_STATE_ERASE:
		{

		}break;
		case BOOTLOADER_STATE_WRITE:
		{

		}break;
		case BOOTLOADER_STATE_DONE:
		{
			uartBootLoaderJumToApplication();
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

