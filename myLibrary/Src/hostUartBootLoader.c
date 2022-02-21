/**
  ******************************************************************************
  * @file    hostUartBootLoader.c
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
#include "hostUartBootLoader.h"
#if (USE_HOST_BOOTLOADER == 1)
#include "uartBootLoader.h"
#include "serialPort.h"
#include "ringBuffer.h"
/* Private typedef------------------------------------------------------------------------------*/
typedef struct
{
	bootLoaderState_t state;
	uartBootLoader_t  boot;
}hostBootLoader_private_t;
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
extern serialPort_t 		serial_port4;
extern UART_HandleTypeDef 	huart4;
extern DMA_HandleTypeDef 	hdma_uart4_rx;

hostBootLoader_private_t 	hBootLoader;
ringBuffer_t 				rBufferRxU4;
uint8_t wData;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __HOST_UART_BOOTLOADER_CONFIGURATION
    @{
*/#ifndef __HOST_UART_BOOTLOADER_CONFIGURATION
#define __HOST_UART_BOOTLOADER_CONFIGURATION
/** @brief  hostUartBootLoaderConfiguration
    @return none
*/
void hostUartBootLoaderConfiguration(void)
{
    /// init serialPort library
    serial_port4.zPrivate.uartHandle.hdmarx = &hdma_uart4_rx;
    serial_port4.zPrivate.uartHandle.Instance = UART4;
    serial_port4.isWriteFinish = true;

	ringBufferInit(&rBufferRxU4);

	if(HAL_UART_Receive_DMA(&huart4, &wData, 1) != HAL_OK)
	{
		Error_Handler();
	}
}


#endif
/**
    @}
*/

/** @group __HOST_UART_BOOTLOADER_FUNCTION
    @{
*/#ifndef __HOST_UART_BOOTLOADER_FUNCTION
#define __HOST_UART_BOOTLOADER_FUNCTION


/** @brief  hostBootLoader_sendCmdConnect
    @return bool
*/
static void hostBootLoader_sendCmdConnect(void)
{
	uint8_t buffer[2] = {UART_BOOTLOADER_CMD_CONNECT, 0};

	serialPort_write(&serial_port4, buffer, 1);
}

/** @brief  hostBootLoaderCmdConnect
    @return bool
*/
static bool hostBootLoaderCmdConnect(void)
{
	uint8_t rData = 0;
	static uint32_t timePrintDebug = 0;
	static uint32_t timeSendCmd	= 0;

	if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
	{
		if(rBufferRxU4.head == 1)
		{
			if(rData == UART_BOOTLOADER_ACK)
			{
				timePrintDebug = 0;
				timeSendCmd	= 0;
				printf("\n[hostBootLoaderCmdConnect] boot connected !@! len = %d\n", rBufferRxU4.len);
				return true;
			}
			else
			{
				printf("\n[hostBootLoaderCmdConnect] reciever nack byte len = %d\n", rBufferRxU4.len);
			}
		}
	}
	else
	{
		if(HAL_GetTick() - timeSendCmd > 1000)
		{
			timeSendCmd = HAL_GetTick();

			hostBootLoader_sendCmdConnect();
			printf("\n[hostBootLoaderCmdConnect] send cmd connect ...\n");
		}
	}

	if(HAL_GetTick() - timePrintDebug > 1000)
	{
		timePrintDebug = HAL_GetTick();
		printf("\n[hostBootLoaderCmdConnect] waitting ack cmd ...\n");
	}

	return false;
}

/** @brief  hostBootLoader_sendCmdGet
    @return bool
*/
static bool hostBootLoader_sendCmdGet(bootLoaderGetCmd_t *cmd)
{
	uint8_t rData = 0;
	uint8_t cmdGetBuff[BOOTLOADER_CMD_LEN] = {0x00, 0xFF};
	uint8_t cmdGetRBuff[BOOTLOADER_CMD_GET_LEN];
	static uint32_t timeSendCmdGet = 0;

	memset(cmdGetRBuff, 0, BOOTLOADER_CMD_GET_LEN);

	/// cho nhan du 15 byte cmd get {ack, numberOfbyte, version, support cmd byte ...}
	if(rBufferRxU4.len == BOOTLOADER_CMD_GET_LEN)
	{
		if(ringBufferRead(&rBufferRxU4, &rData))
		{
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hostBootLoader_sendCmdGet] reciever cmd ack\n");
				for(uint8_t i = 0; i < BOOTLOADER_CMD_GET_LEN; i++)
				{
					if(ringBufferRead(&rBufferRxU4, &rData))
					{
						cmdGetRBuff[i] = rData;
						printf("\n[hostBootLoader_sendCmdGet] reciever data cmd get value = 0x%x | len = %d\n", rData, rBufferRxU4.len);
					}
				}

				cmd->numberOfbyte 			= cmdGetRBuff[0];
				cmd->version 				= cmdGetRBuff[1];
				cmd->getCmd 				= cmdGetRBuff[2];
				cmd->getVerAndRPStatus 		= cmdGetRBuff[3];
				cmd->getId 					= cmdGetRBuff[4];
				cmd->readMemoryCmd 			= cmdGetRBuff[5];
				cmd->goCmd 					= cmdGetRBuff[6];
				cmd->writeMemoryCmd 		= cmdGetRBuff[7];
				cmd->EraseCmd 				= cmdGetRBuff[8]; /*Erase command or Extended Erase command (exclusive commands)*/
				cmd->writeProtectCmd 		= cmdGetRBuff[9];
				cmd->writeUnProtectCmd 		= cmdGetRBuff[10];
				cmd->readOutProtectCmd 		= cmdGetRBuff[11];
				cmd->readOutUnProtectCmd 	= cmdGetRBuff[12];
				cmd->getChecksumCmd 		= cmdGetRBuff[13];

				return true;
			}
		}
	}
	else
	{
		if(HAL_GetTick() - timeSendCmdGet > 1000 || timeSendCmdGet == 0)
		{
			timeSendCmdGet = HAL_GetTick();

			printf("\n[hostBootLoader_sendCmdGet] send cmd get ...\n");

			serialPort_write(&serial_port4, cmdGetBuff, 2);
		}
	}

	return false;
}

/** @brief  hostBootLoader_sendCmdGetVer
    @return bool
*/
static bool hostBootLoader_sendCmdGetVer(bootLoaderGetVerCmd_t *cmd)
{
	uint8_t rData = 0;
	uint8_t cmdGetVerBuff[BOOTLOADER_CMD_LEN] = {0x01, 0xFE};
	uint8_t cmdGetRBuff[BOOTLOADER_CMD_GET_VER_LEN];
	static uint32_t timeSendCmdGet = 0;

	if(rBufferRxU4.len == BOOTLOADER_CMD_GET_VER_LEN)
	{
		if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
		{
			/// kiem tra ack
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hostBootLoader_sendCmdGetVer] reciever cmd ack\n");
				for(uint8_t i = 0; i < BOOTLOADER_CMD_GET_VER_LEN; i++)
				{
					if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
					{
						cmdGetRBuff[i] = rData;
						printf("\n[hostBootLoader_sendCmdGetVer] reciever data cmd get ver value = 0x%x | len = %d\n", rData, rBufferRxU4.len);
					}
				}

				cmd->version 	= cmdGetRBuff[0];
				cmd->optionByte1 = cmdGetRBuff[1];
				cmd->optionByte2 = cmdGetRBuff[2];

				return true;
			}
		}
	}
	else
	{
		if(HAL_GetTick() - timeSendCmdGet > 1000 || timeSendCmdGet == 0)
		{
			timeSendCmdGet = HAL_GetTick();

			printf("\n[hostBootLoader_sendCmdGetVer] send cmd get ver ...\n");

			serialPort_write(&serial_port4, cmdGetVerBuff, 2);
		}
	}

	return false;
}

/** @brief  hostBootLoader_sendCmdGetId
    @return bool
*/
static bool hostBootLoader_sendCmdGetId(bootLoaderGetIdCmd_t *cmd)
{
	uint8_t rData = 0;
	uint8_t cmdGetIdBuff[BOOTLOADER_CMD_LEN] = {0x02, 0xFD};
	uint8_t cmdGetRBuff[BOOTLOADER_CMD_GET_ID_LEN];
	static uint32_t timeSendCmdGet = 0;

	if(rBufferRxU4.len == BOOTLOADER_CMD_GET_ID_LEN)
	{
		if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
		{
			/// kiem tra ack
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hostBootLoader_sendCmdGetId] reciever cmd ack\n");
				for(uint8_t i = 0; i < BOOTLOADER_CMD_GET_ID_LEN; i++)
				{
					if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
					{
						cmdGetRBuff[i] = rData;
						printf("\n[hostBootLoader_sendCmdGetId] reciever data cmd get id value = 0x%x | len = %d\n", rData, rBufferRxU4.len);
					}
				}

				cmd->numberOfbyte 	= cmdGetRBuff[0];
				cmd->byte3 = cmdGetRBuff[1];
				cmd->byte4 = cmdGetRBuff[2];

				cmd->PID = cmd->byte3 | cmd->byte4 << 8;

				return true;
			}
		}
	}
	else
	{
		if(HAL_GetTick() - timeSendCmdGet > 1000 || timeSendCmdGet == 0)
		{
			timeSendCmdGet = HAL_GetTick();

			printf("\n[hostBootLoader_sendCmdGetId] send cmd get id ...\n");

			serialPort_write(&serial_port4, cmdGetIdBuff, 2);
		}
	}

	return false;
}

/** @brief  hostBootLoader_sendCmdErase
    @return bool
*/
static bool hostBootLoader_sendCmdErase(void)
{
	uint8_t rData = 0;
	uint8_t cmdEraseBuff[BOOTLOADER_CMD_LEN] = {0x43, 0xBC};
	static uint32_t timeSendCmdGet = 0;

	if(rBufferRxU4.len == BOOTLOADER_CMD_ERASE_LEN)
	{
		if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
		{
			/// kiem tra ack
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hostBootLoader_sendCmdErase] reciever cmd ack\n");

				return true;
			}
		}
	}
	else
	{
		if(HAL_GetTick() - timeSendCmdGet > 3000 || timeSendCmdGet == 0)
		{
			timeSendCmdGet = HAL_GetTick();

			printf("\n[hostBootLoader_sendCmdErase] send cmd erase ...\n");

			serialPort_write(&serial_port4, cmdEraseBuff, 2);
		}
	}

	return false;
}

/** @brief  hostBootLoader_sendCmdWriteMem
    @return bool
*/
static bootLoaderCmdWriteResult_t hostBootLoader_sendCmdWriteMem(void)
{
	uint8_t rData = 0;
	uint8_t cmdWriteMemBuff[BOOTLOADER_CMD_LEN] = {0x31, 0xCe};
	static uint32_t timeSendCmdGet = 0;
	static uint8_t cmdWriteState = 0;
	static uint32_t startAddress = 0x08020000;
	uint32_t maxLen = 0x267c;
	uint32_t endAddress = startAddress + maxLen;
	static bool firstSendCmd = false;

	switch(cmdWriteState)
	{
		case 0: /// send cmd write watting 1 byte ack
		{

			if(firstSendCmd == false)
			{
				firstSendCmd = true;
			}
			else
			{
				startAddress |= 256;
			}

			if(rBufferRxU4.len == 1)
			{
				if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
				{
					/// kiem tra ack
					if(rData == UART_BOOTLOADER_ACK)
					{
						printf("\n[hostBootLoader_sendCmdWriteMem] reciever cmd ack\n");

						cmdWriteState = 1;
					}
					else
					{
						return BOOTLOADER_CMD_WRITE_RESULT_ERROR;
						printf("\n[hostBootLoader_sendCmdWriteMem] reciever cmd nack\n");
					}
				}
			}
			else
			{
				if(HAL_GetTick() - timeSendCmdGet > 1000 || timeSendCmdGet == 0)
				{
					timeSendCmdGet = HAL_GetTick();

					printf("\n[hostBootLoader_sendCmdWriteMem] send cmd write 1 ...\n");

					serialPort_write(&serial_port4, cmdWriteMemBuff, 2);
				}
			}
		}break;
		case 1: /// send 4 byte address and 1 byte checksum
		{
			uint8_t addressBuffer[5];

			addressBuffer[0] = startAddress;
			addressBuffer[1] = startAddress >> 8;
			addressBuffer[2] = startAddress >> 16;
			addressBuffer[3] = startAddress >> 24;

			uint8_t checksum = uartBootLoaderChecksumCalculator(0, addressBuffer, 4);

			printf("\n[hostBootLoader_sendCmdWriteMem] address = 0x%x \n byte1 = 0x%x | byte2 = 0x%x | byte3 = 0x%x | byte4 = 0x%x | checksum = 0x%x\n"
					, (int)startAddress, (int)addressBuffer[0], (int)addressBuffer[1], (int)addressBuffer[2], (int)addressBuffer[3], (int)checksum);

			addressBuffer[4] = checksum;

			serialPort_write(&serial_port4, addressBuffer, 5);

			cmdWriteState = 2;
		}break;
		case 2:
		{
			/// ktra ack
			if(rBufferRxU4.len == 1)
			{
				if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
				{
					if(rData == UART_BOOTLOADER_ACK)
					{
						printf("\n[hostBootLoader_sendCmdWriteMem] reciever cmd ack\n");

						cmdWriteState = 3;
					}
					else
					{
						return BOOTLOADER_CMD_WRITE_RESULT_ERROR;
						printf("\n[hostBootLoader_sendCmdWriteMem] reciever cmd nack\n");
					}
				}
			}
		}break;
		case 3:
		{
			uint8_t payloadBuffer[129];

//			payloadBuffer = calloc(129, sizeof(uint8_t));

			payloadBuffer[0] = 127;

			/// create dummy data
			for(uint16_t i = 1; i < 127; i++)
			{
				payloadBuffer[i] = i;
			}

			uint8_t checksum = uartBootLoaderChecksumCalculator(payloadBuffer[0], payloadBuffer, 127);

			printf("\n[hostBootLoader_sendCmdWriteMem] checksum = 0x%x\n", (int)checksum);

			payloadBuffer[127] = checksum;

//			serialPort_write(&serial_port4, payloadBuffer, 100);
//			serialPort_write(&serial_port4, payloadBuffer + 100, 100);
//			serialPort_write(&serial_port4, payloadBuffer + 200, 58);
			HAL_UART_Transmit_DMA(&huart4, payloadBuffer, 129);

			cmdWriteState = 4;

//			free(payloadBuffer);
		}break;
		case 4:
		{
			/// ktra ack
			if(rBufferRxU4.len == 1)
			{
				if(ringBufferRead(&rBufferRxU4, &rData) == RING_BUFFER_OK)
				{
					if(rData == UART_BOOTLOADER_ACK)
					{
						printf("\n[hostBootLoader_sendCmdWriteMem] reciever cmd ack\n");

						cmdWriteState = 5;
					}
					else
					{
						return BOOTLOADER_CMD_WRITE_RESULT_ERROR;
						printf("\n[hostBootLoader_sendCmdWriteMem] reciever cmd nack\n");
					}
				}
			}
		}break;
		case 5:
		{
			if(startAddress < endAddress)
			{
				cmdWriteState = 0;
			}
			else
			{
				return BOOTLOADER_CMD_WRITE_RESULT_OK;
			}

		}break;
	}

	return BOOTLOADER_CMD_WRITE_RESULT_IDLE;
}

#endif
/**
    @}
*/

/** @group __HOST_UART_BOOTLOADER_PROCESS
    @{
*/#ifndef __HOST_UART_BOOTLOADER_PROCESS
#define __HOST_UART_BOOTLOADER_PROCESS
/** @brief  hostUartBootLoaderProcess
    @return none
*/
void hostUartBootLoaderProcess(void)
{

	switch(hBootLoader.state)
	{
		case BOOTLOADER_STATE_IDLE:
		{
			if(hostBootLoaderCmdConnect() == true)
			{
				hBootLoader.state = BOOTLOADER_STATE_CONNECTED;
			}
		}break;
		case BOOTLOADER_STATE_CONNECTED:
		{
			static bootLoaderGetState_t state = BOOTLOADER_GET_STATE_CMD_GET;
			switch(state)
			{
				case BOOTLOADER_GET_STATE_CMD_GET:
				{
					if(hostBootLoader_sendCmdGet(&hBootLoader.boot.getCmd) == true)
					{
						state = BOOTLOADER_GET_STATE_CMD_GET_VER;
					}
				}break;
				case BOOTLOADER_GET_STATE_CMD_GET_VER:
				{
					if(hostBootLoader_sendCmdGetVer(&hBootLoader.boot.getVer) == true)
					{
						state = BOOTLOADER_GET_STATE_CMD_GET_ID;
					}
				}break;
				case BOOTLOADER_GET_STATE_CMD_GET_ID:
				{
					if(hostBootLoader_sendCmdGetId(&hBootLoader.boot.getId) == true)
					{
						state = BOOTLOADER_GET_STATE_DONE;
					}
				}break;
				case BOOTLOADER_GET_STATE_DONE:
				{
					hBootLoader.state = BOOTLOADER_STATE_ERASE;
				}
			}
		}break;
		case BOOTLOADER_STATE_ERASE:
		{
			if(hostBootLoader_sendCmdErase() == true)
			{
				hBootLoader.state = BOOTLOADER_STATE_WRITE;
			}
		}break;
		case BOOTLOADER_STATE_WRITE:
		{
			if(hostBootLoader_sendCmdWriteMem() == BOOTLOADER_CMD_WRITE_RESULT_OK)
			{
				hBootLoader.state = BOOTLOADER_STATE_DONE;
			}
			else if(hostBootLoader_sendCmdWriteMem() == BOOTLOADER_CMD_WRITE_RESULT_ERROR)
			{
				hBootLoader.state = BOOTLOADER_STATE_ERROR;
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

