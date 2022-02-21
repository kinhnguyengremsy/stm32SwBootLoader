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
#include "uartCallback.h"
#include "ringBuffer.h"
/* Private typedef----------------------------------------------------------------------------*/
/* Private define-----------------------------------------------------------------------------*/

/* Private macro------------------------------------------------------------------------------*/
/* Private variables--------------------------------------------------------------------------*/
extern serialPort_t 		serial_port2;
extern UART_HandleTypeDef 	huart2;
extern DMA_HandleTypeDef 	hdma_usart2_rx;
uartBootLoader_t 			boot;
extern uartCallback_t 		callback;
static ringBuffer_t 		rBufferRx;
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
    /// init serialPort library
    serial_port2.zPrivate.uartHandle.hdmarx = &hdma_usart2_rx;
    serial_port2.zPrivate.uartHandle.Instance = USART2;
    serial_port2.isWriteFinish = true;

	ringBufferInit(&rBufferRx);

	if(HAL_UART_Receive_IT(&huart2, &boot.rxData, 1) != HAL_OK)
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

	serialPort_write_list(&serial_port2, buffer);
}

/** @brief	uartBootLoaderSendAck
    @return	none
*/
static void uartBootLoaderSendNack(void)
{
	uint8_t buffer[2] = {UART_BOOTLOADER_NACK, 0};

	serialPort_write_list(&serial_port2, buffer);
}

/** @brief	uartBootLoaderSendMoreByte
    @return	none
*/
static void uartBootLoaderSendMoreByte(uint8_t *buffer)
{
	serialPort_write_list(&serial_port2, buffer);
}

#endif
/**
    @}
*/

/** @group __UART_BOOTLOADER_RECIEVER_CMD_FUNCTIONS
    @{
*/#ifndef __UART_BOOTLOADER_READ_FUNCTION
#define __UART_BOOTLOADER_READ_FUNCTION

/** @brief  uartBootLoader_readData
    @return
*/
static void uartBootLoader_readData(uartCallback_t *cb, uartBootLoader_t *boot)
{
	if(cb->rxComplete == true)
	{
		cb->rxComplete = false;

		ringBufferWrite(&rBufferRx, boot->rxData);

		if(HAL_UART_Receive_IT(&huart2, &boot->rxData, 1) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

/** @brief  uartBootLoaderRecieverCmdConnect
    @return 
*/
static bool uartBootLoaderRecieverCmdConnect(uartBootLoader_t *boot)
{
	uint8_t rData = 0;
	static uint32_t timePrintDebug = 0;

	if(ringBufferRead(&rBufferRx, &rData) == RING_BUFFER_OK)
	{
		if(rBufferRx.head == 1)
		{
			if(rData == UART_BOOTLOADER_CMD_CONNECT)
			{
				uartBootLoaderSendAck();
				timePrintDebug = 0;
				printf("\n[uartBootLoaderRecieverCmdConnect] boot connected !@!\n");

				return true;
			}
			else
			{
				uartBootLoaderSendNack();
				printf("\n[uartBootLoaderRecieverCmdConnect] non valid byte !@!\n");
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
	uint8_t getCmdData[2] = {0x00, 0xFF};
	bootLoaderGetCmd_t *cmd;
	static uint8_t trueDataCount = 0;

	for(uint8_t i = 0; i < 2; i++)
	{
		if(ringBufferRead(&rBufferRx, &rData))
		{
			if(rData == getCmdData[i])
			{
				trueDataCount++;
				printf("\n[uartBootLoaderRecieverCmdGet] reciever cmd get value = %d\n", rData);
			}
		}
	}

	if(trueDataCount == 2)
	{
		cmd->numberOfbyte = 2;
		cmd->version = 1;
		cmd->support = 1;

		uint8_t buffer[4] = {UART_BOOTLOADER_ACK, cmd->numberOfbyte, cmd->version, cmd->support};
		uartBootLoaderSendMoreByte(buffer);

		printf("\n[uartBootLoaderRecieverCmdGet] send Ack byte and data of cmd ...\n");

		trueDataCount = 0;

		return true;
	}


	return false;
}

/** @brief  uartBootLoaderRecieverCmdVersionAndReadProtection
    @return bootLoaderGetVersionAndReadProtectionCmd_t
*/
static bootLoaderGetVersionAndReadProtectionCmd_t *uartBootLoaderRecieverCmdVersionAndReadProtection(uartBootLoader_t *boot)
{
	bootLoaderGetVersionAndReadProtectionCmd_t *cmd;

	cmd->version = 0;
	cmd->optionByte1 = 0;
	cmd->optionByte2 = 0;

	return cmd;
}

/** @brief  uartBootLoaderRecieverCmdGetId
    @return bootLoaderGetIdCmd_t
*/
static bootLoaderGetIdCmd_t *uartBootLoaderRecieverCmdGetId(uartBootLoader_t *boot)
{
	bootLoaderGetIdCmd_t *cmd;

	cmd->numberOfbyte = 0;
	cmd->id.byte1 = 0;
	cmd->byte2 = 0;

	return cmd;
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
	uartBootLoader_readData(&callback, &boot);

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
			static uint8_t state = 0;
			switch(state)
			{
				case 0:
				{
					if(uartBootLoaderRecieverCmdGet(&boot) == true)
					{
						state = 1;
					}
				}break;
				case 1:
				{

				}break;
			}
		}break;
		case BOOTLOADER_STATE_ERASE:
		{

		}break;
		case BOOTLOADER_STATE_WRITE:
		{

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

