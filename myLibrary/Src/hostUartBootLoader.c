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
#include "ringBuffer.h"
#include "mavlinkControl.h"
#include "uartCLI.h"
#include "uartBootLoader.h"
/* Private define------------------------------------------------------------------------------*/
#define STM32_MAX_FRAME  		256	/* cmd read memory */
#define STM32_MAX_TX_FRAME  	(1 + STM32_MAX_FRAME + 1)	/* cmd write memory */

#define STM32_MAX_PAGES     	0x0000ffff
#define STM32_MASS_ERASE    	0x00100000 /* > 2 x max_pages */
/* Private typedef------------------------------------------------------------------------------*/

struct _bootLoaderCmdList
{
	uint8_t cmdHeader;
	uint8_t cmdFooter;
}bootLoaderCmdList[BOOTLOADER_CMD_GET_TOTAL] = {
		{.cmdHeader = UART_BOOTLOADER_CMD_CONNECT			, .cmdFooter = 0x00},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET				, .cmdFooter = UART_BOOTLOADER_CMD_GET 		^ 0xff},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET_VER			, .cmdFooter = UART_BOOTLOADER_CMD_GET_VER 	^ 0xff},
		{.cmdHeader = UART_BOOTLOADER_CMD_GET_ID			, .cmdFooter = UART_BOOTLOADER_CMD_GET_ID 	^ 0xff},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_MEMORY		, .cmdFooter = 0xEE},
		{.cmdHeader = UART_BOOTLOADER_CMD_GO				, .cmdFooter = 0xDE},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_MEMORY		, .cmdFooter = 0xCE},
		{.cmdHeader = UART_BOOTLOADER_CMD_ERASE				, .cmdFooter = UART_BOOTLOADER_CMD_ERASE 	^ 0xff},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_PROTECT		, .cmdFooter = 0x9C},
		{.cmdHeader = UART_BOOTLOADER_CMD_WRITE_UNPROTECT	, .cmdFooter = 0x8C},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_PROTECT		, .cmdFooter = 0x7D},
		{.cmdHeader = UART_BOOTLOADER_CMD_READ_UNPROTECT	, .cmdFooter = 0x6D},
//		{.cmdHeader = UART_BOOTLOADER_CMD_GET_CHECKSUM		, .cmdFooter = 0x5E},
};

typedef struct
{
	bootLoaderGetCmd_t cmdGet;
	bootLoaderGetIdCmd_t cmdGetId;

}hostBootLoaderPrivate_t;
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
extern bool txComplete;
ringBuffer_t 						rBufferHostBL;
uint8_t 							wData;
static hostBootLoader_t				*host;
static UART_HandleTypeDef 			*uart_hostBL;
hostBootLoaderPrivate_t				hostPri;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __HOST_UART_BOOTLOADER_CONFIGURATION
    @{
*/#ifndef __HOST_UART_BOOTLOADER_CONFIGURATION
#define __HOST_UART_BOOTLOADER_CONFIGURATION
/** @brief  hostUartBootLoaderConfiguration
    @return none
*/
void hostUartBootLoaderConfiguration(hostBootLoader_t *_host, UART_HandleTypeDef *huart)
{
	printf("\n[hostUartBootLoaderConfiguration] use mavlink + hw host boot loader + uart cli ...\n");
	host = _host;
	uart_hostBL = huart;
//	mavlinhControlConfiguration();

	/// debug
//	host->isBootLoader = true;
}

/** @brief  __hostBL
    @return pointer struct support host boot loader
*/
hostBootLoader_t* __hostBL(void)
{
	return host;
}


/** @brief  getTime
    @return bool
*/
static bool getTime(uint32_t *time, uint32_t timeCmp)
{
	if(HAL_GetTick() - *time > timeCmp || *time == 0)
	{
		*time = HAL_GetTick();

		return true;
	}

	return false;
}

#endif
/**
    @}
*/

/** @group __HOST_UART_BOOTLOADER_FUNCTION
    @{
*/#ifndef __HOST_UART_BOOTLOADER_FUNCTION
#define __HOST_UART_BOOTLOADER_FUNCTION
/** @brief  hostBootLoader_uartConfigForBL
    @return none
*/
static void hostBootLoader_uartConfigForBL(void)
{
	HAL_UART_DeInit(uart_hostBL);

	HAL_Delay(500);

	uart_hostBL->Instance 					= USART2;
	uart_hostBL->Init.BaudRate 				= 115200;//460800 ;
	uart_hostBL->Init.WordLength 			= UART_WORDLENGTH_9B;
	uart_hostBL->Init.StopBits 				= UART_STOPBITS_1;
	uart_hostBL->Init.Parity 				= UART_PARITY_EVEN;
	uart_hostBL->Init.Mode 					= UART_MODE_TX_RX;
	uart_hostBL->Init.HwFlowCtl 			= UART_HWCONTROL_NONE;
	uart_hostBL->Init.OverSampling 			= UART_OVERSAMPLING_16;
	if (HAL_UART_Init(uart_hostBL) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_Delay(500);

	ringBufferInit(&rBufferHostBL);

	if(HAL_UART_Receive_IT(uart_hostBL, &wData, 1) != HAL_OK)
	{
		Error_Handler();
	}

    /// xoa bo dem uart truoc khi truyen
    __HAL_UART_FLUSH_DRREGISTER(uart_hostBL);
}

/** @brief  hostBootLoader_sendData
    @return number of command
*/
static void hostBootLoader_sendData(uint8_t *buffer, uint16_t len)
{
	HAL_UART_Transmit_IT(uart_hostBL, buffer, len);
	while(txComplete == false);
	txComplete = false;
}

/** @brief  hostBootLoader_getCmdHeader
    @return number of command header
*/
static uint8_t hostBootLoader_getCmdHeader(uint8_t cmdIndex)
{
	return (bootLoaderCmdList[cmdIndex].cmdHeader);
}

/** @brief  hostBootLoader_getCmdFooter
    @return number of command footer
*/
static uint8_t hostBootLoader_getCmdFooter(uint8_t cmdIndex)
{
	return (bootLoaderCmdList[cmdIndex].cmdFooter);
}

/** @brief  hostBootLoader_readCommand
    @return number of command
*/
static void hostBootLoader_sendCommand(uint8_t cmdIndex)
{
	host->isSendCmd = true;
	/// get cmd header
	host->hostCmd[0] = hostBootLoader_getCmdHeader(cmdIndex);

	/// get cmd footer
	host->hostCmd[1] = hostBootLoader_getCmdFooter(cmdIndex);

	printf("\n[hostBootLoader_sendCommand] 0x%x | 0x%x\n", host->hostCmd[0], host->hostCmd[1]);
}

/** @brief  hoatBootLoader_waittingResponeCmdConnect
    @return number of command
*/
static bool hoatBootLoader_waittingResponeCmdConnect(void)
{
	uint8_t rData = 0;

	if(rBufferHostBL.len >= 1)
	{
		if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
		{
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hoatBootLoader_waittingResponeCmdConnect] device connected !@!\n");
				return true;
			}
		}
	}

	return false;
}

/** @brief  hoatBootLoader_waittingResponeCmdGet
    @return number of command
*/
static bool hoatBootLoader_waittingResponeCmdGet(void)
{
	uint8_t rData = 0;
	uint8_t buffer[BOOTLOADER_CMD_GET_LEN];

	if(rBufferHostBL.len >= BOOTLOADER_CMD_GET_LEN)
	{
		for(uint8_t i = 0; i < BOOTLOADER_CMD_GET_LEN; i++)
		{
			if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
			{
				buffer[i] = rData;
			}
		}

		/// kiem tra ack
		if(buffer[0] == UART_BOOTLOADER_ACK && buffer[14] == UART_BOOTLOADER_ACK)
		{
			hostPri.cmdGet.numberOfbyte 			= buffer[1];
			hostPri.cmdGet.version 					= buffer[2];
			hostPri.cmdGet.getCmd 					= buffer[3];
			hostPri.cmdGet.getVerAndRPStatus 		= buffer[4];
			hostPri.cmdGet.getId 					= buffer[5];
			hostPri.cmdGet.readMemoryCmd 			= buffer[6];
			hostPri.cmdGet.goCmd 					= buffer[7];
			hostPri.cmdGet.writeMemoryCmd 			= buffer[8];
			hostPri.cmdGet.EraseCmd 				= buffer[9];
			hostPri.cmdGet.writeProtectCmd 			= buffer[10];
			hostPri.cmdGet.writeUnProtectCmd 		= buffer[11];
			hostPri.cmdGet.readOutProtectCmd 		= buffer[12];
			hostPri.cmdGet.readOutUnProtectCmd 		= buffer[13];

			return true;
		}
	}

	return false;
}

/** @brief  hoatBootLoader_waittingResponeCmdGetId
    @return number of command
*/
static bool hoatBootLoader_waittingResponeCmdGetId(void)
{
	uint8_t rData = 0;
	uint8_t buffer[BOOTLOADER_CMD_GET_ID_LEN];

	if(rBufferHostBL.len >= BOOTLOADER_CMD_GET_ID_LEN)
	{
		for(uint8_t i = 0; i < BOOTLOADER_CMD_GET_ID_LEN; i++)
		{
			if(ringBufferRead(&rBufferHostBL, &rData))
			buffer[i] = rData;
		}

		if(buffer[0] == UART_BOOTLOADER_ACK && buffer[4] == UART_BOOTLOADER_ACK)
		{
			hostPri.cmdGetId.numberOfbyte = buffer[1];
			hostPri.cmdGetId.byte3 = buffer[2];
			hostPri.cmdGetId.byte4 = buffer[3];
			hostPri.cmdGetId.PID = hostPri.cmdGetId.byte3 << 8 | hostPri.cmdGetId.byte4;

			printf("\n[hoatBootLoader_waittingResponeCmdGetId] chipId : 0x%x\n", hostPri.cmdGetId.PID);

			return true;
		}
	}

	return false;
}

/** @brief  hostBootLoader_readCommand
    @return number of command
*/
static hostBootLoaderState_t hostBootLoader_readCommand(void)
{
	static hostBootLoaderState_t state = HOST_BOOTLOADER_STATE_IDLE;
	if(host->isSendCmd == true)
	{
		host->isSendCmd = false;

		if(host->state == HOST_BOOTLOADER_STATE_IDLE)
		{
			uint8_t data = UART_BOOTLOADER_CMD_CONNECT;
			hostBootLoader_sendData(&data, 1);
		}
		else
		{
			hostBootLoader_sendData(host->hostCmd, 2);
		}

	}
	else
	{
		switch(host->hostCmd[0])
		{
			case UART_BOOTLOADER_CMD_CONNECT:
			{
				if(hoatBootLoader_waittingResponeCmdConnect() == true)
				{
					state = HOST_BOOTLOADER_STATE_CONNECTED;
				}
			}break;
			case UART_BOOTLOADER_CMD_GET:
			{
				if(hoatBootLoader_waittingResponeCmdGet() == true)
				{
					state = HOST_BOOTLOADER_STATE_CMD_GET;
				}
			}break;
			case UART_BOOTLOADER_CMD_GET_VER:
			{

			}break;
			case UART_BOOTLOADER_CMD_GET_ID:
			{
				if(hoatBootLoader_waittingResponeCmdGetId() == true)
				{
					state = HOST_BOOTLOADER_STATE_GET_ID;
				}
			}break;
			case UART_BOOTLOADER_CMD_READ_MEMORY:
			{

			}break;
			case UART_BOOTLOADER_CMD_GO:
			{

			}break;
			case UART_BOOTLOADER_CMD_WRITE_MEMORY:
			{

			}break;
			case UART_BOOTLOADER_CMD_ERASE:
			{

			}break;
		}
	}

	return state;
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
	static uint32_t timeSendCmd = 0;

	if(__cmdCli()->flagMsgJumTarget == true)
	{
		host->isBootLoader = true;
	}

	if(host->isBootLoader == true)
	{
		if(host->uartConfigForHwBL == false)
		{
			host->uartConfigForHwBL = true;

			printf("\n[hostUartBootLoaderProcess] re config uart for boot loader\n");

			hostBootLoader_uartConfigForBL();
		}
		else
		{
			/// boot loader process
			host->state = hostBootLoader_readCommand();

			switch(host->state)
			{
				case HOST_BOOTLOADER_STATE_IDLE:
				{
					if(getTime(&timeSendCmd, 1000) == true)
					{
						printf("\n[hoatBootLoader_waittingResponeCmdConnect] waitting device connect ...\n");
						hostBootLoader_sendCommand(BOOTLOADER_CMD_NONE);
					}

				}break;
				case HOST_BOOTLOADER_STATE_CONNECTED:
				{
					if(getTime(&timeSendCmd, 1000) == true)
					{
						printf("\n[hoatBootLoader_waittingResponeCmdConnect] waitting device reponse cmd get ...\n");
						hostBootLoader_sendCommand(BOOTLOADER_CMD_GET);
					}
				}break;
				case HOST_BOOTLOADER_STATE_CMD_GET:
				{
					if(getTime(&timeSendCmd, 1000) == true)
					{
						printf("\n[hoatBootLoader_waittingResponeCmdConnect] waitting device reponse cmd getId ...\n");
						hostBootLoader_sendCommand(BOOTLOADER_CMD_GET_ID);
					}
				}break;
				case HOST_BOOTLOADER_STATE_GET_ID:
				{

				}break;
				case HOST_BOOTLOADER_STATE_ERASE:
				{

				}break;
				case HOST_BOOTLOADER_STATE_WRITE:
				{

				}break;
				case HOST_BOOTLOADER_STATE_DONE:
				{

				}break;
				case HOST_BOOTLOADER_STATE_ERROR:
				{

				}break;
			}
		}
	}
	else
	{
//		mavlinkControl_process();
	}
}

#endif
/**
    @}
*/
#endif
/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

