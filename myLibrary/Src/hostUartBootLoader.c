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
#include "uartcallback.h"
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
extern uartCallback_t 		callback4;

hostBootLoader_private_t 	hBootLoader;
static ringBuffer_t 		rBufferRx;
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

	ringBufferInit(&rBufferRx);

	if(HAL_UART_Receive_IT(&huart4, &hBootLoader.boot.rxData, 1) != HAL_OK)
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

/** @brief  hostBootLoader_readData
    @return
*/
static void hostBootLoader_readData(uartCallback_t *cb, uartBootLoader_t *boot)
{
	if(cb->rxComplete == true)
	{
		cb->rxComplete = false;

		ringBufferWrite(&rBufferRx, boot->rxData);

		if(HAL_UART_Receive_IT(&huart4, &boot->rxData, 1) != HAL_OK)
		{
			Error_Handler();
		}
	}
}

/** @brief  hostBootLoader_sendCmdConnect
    @return bool
*/
static void hostBootLoader_sendCmdConnect(void)
{
	uint8_t buffer[2] = {UART_BOOTLOADER_CMD_CONNECT, 0};

	serialPort_write_list(&serial_port4, buffer);
}

/** @brief  hostBootLoaderCmdConnect
    @return bool
*/
static bool hostBootLoaderCmdConnect(void)
{
	uint8_t rData = 0;
	static uint32_t timePrintDebug = 0;
	static uint32_t timeSendCmd	= 0;

	if(ringBufferRead(&rBufferRx, &rData) == RING_BUFFER_OK)
	{
		if(rBufferRx.head == 1)
		{
			if(rData == UART_BOOTLOADER_ACK)
			{
				timePrintDebug = 0;
				timeSendCmd	= 0;
				printf("\n[hostBootLoaderCmdConnect] boot connected !@!\n");
				return true;
			}
			else
			{
				printf("\n[hostBootLoaderCmdConnect] reciever nack byte !@!\n");
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
	uint8_t cmdGetBuff[2] = {0x00, 0xFF};
	uint8_t cmdGetRBuff[3] = {0, 0, 0};
	static uint32_t timeSendCmdGet = 0;

	if(ringBufferRead(&rBufferRx, &rData))
	{
		if(rBufferRx.head == 1)
		{
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hostBootLoader_sendCmdGet] reciever cmd ack\n");
				for(uint8_t i = 0; i < 3; i++)
				{
					if(ringBufferRead(&rBufferRx, &rData))
					{
						cmdGetRBuff[i] = rData;
						printf("\n[hostBootLoader_sendCmdGet] reciever data cmd get value = %d\n", rData);
					}
				}

				cmd->numberOfbyte 	= cmdGetRBuff[0];
				cmd->version 		= cmdGetRBuff[1];
				cmd->support 		= cmdGetRBuff[2];

				return true;
			}
		}
	}
	else
	{
		if(HAL_GetTick() - timeSendCmdGet > 1000 || timeSendCmdGet == 0)
		{
			timeSendCmdGet = HAL_GetTick();

			printf("\n[hostBootLoader_sendCmdGet] send cmd get\n");

			serialPort_write_list(&serial_port4, cmdGetBuff);
		}
	}

	return false;
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
	hostBootLoader_readData(&callback4, &hBootLoader.boot);

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
			static uint8_t state = 0;
			switch(state)
			{
				case 0:
				{
					if(hostBootLoader_sendCmdGet(&hBootLoader.boot.getCmd) == true)
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

