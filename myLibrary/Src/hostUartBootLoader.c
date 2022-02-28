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
	uint32_t timeSendCmd;

	bootLoaderGetCmd_t cmdGet;
	bootLoaderGetIdCmd_t cmdGetId;

	uint8_t cmdEraseState;
	bool deviceReponseCmdErase;

	uint8_t cmdWriteMemState;
	bool deviceReponseCmdWrite;
	bool readDataSource;
	uint8_t writeData[256];

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
	host->isBootLoader = true;
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

	ringBufferInit(&rBufferHostBL);
#if(HOST_USE_UART_IT == 1)
	if(HAL_UART_Receive_IT(uart_hostBL, &wData, 1) != HAL_OK)
	{
		Error_Handler();
	}
#else
	if(HAL_UART_Receive_DMA(uart_hostBL, &wData, 1) != HAL_OK)
	{
		Error_Handler();
	}
#endif


    /// xoa bo dem uart truoc khi truyen
    __HAL_UART_FLUSH_DRREGISTER(uart_hostBL);

    HAL_Delay(500);

    /// first send cmd connect
    host->hostCmd[0] = UART_BOOTLOADER_CMD_CONNECT;
}

/** @brief  hostBootLoader_sendData
    @return number of command
*/
static void hostBootLoader_sendData(uint8_t *buffer, uint16_t len)
{

#if(HOST_USE_UART_IT == 1)
	HAL_UART_Transmit_IT(uart_hostBL, buffer, len);
#else
	HAL_UART_Transmit_DMA(uart_hostBL, buffer, len);
#endif

	while(txComplete == false);
	txComplete = false;
}

/** @brief  hostBootLoader_getCmdHeader
    @return number of command header
*/
static uint8_t hostBootLoader_getCmdHeader(uint8_t cmdIndex)
{
	return cmdIndex;
}

/** @brief  hostBootLoader_getCmdFooter
    @return number of command footer
*/
static uint8_t hostBootLoader_getCmdFooter(uint8_t cmdIndex)
{
	return (cmdIndex ^ 0xff);
}

/** @brief  hostBootLoader_readCommand
    @return number of command
*/
static void hostBootLoader_sendCommand(uint8_t cmdIndex, uint8_t len)
{
	host->isSendCmd = true;
	/// get cmd header
	host->hostCmd[0] = hostBootLoader_getCmdHeader(cmdIndex);

	/// get cmd footer
	host->hostCmd[1] = hostBootLoader_getCmdFooter(cmdIndex);

	hostBootLoader_sendData(host->hostCmd, len);

	printf("\n[hostBootLoader_sendCommand] 0x%x | 0x%x\n", host->hostCmd[0], host->hostCmd[1]);
}

/** @brief  hostBootLoader_waittingResponeCmdConnect
    @return number of command
*/
static bool hostBootLoader_waittingResponeCmdConnect(void)
{
	uint8_t rData = 0;

	if(rBufferHostBL.len == 1)
	{
		if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
		{
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hostBootLoader_waittingResponeCmdConnect] device connected !@!\n");
				return true;
			}
		}
	}

	return false;
}

/** @brief  hostBootLoader_waittingResponeCmdGet
    @return number of command
*/
static bool hostBootLoader_waittingResponeCmdGet(void)
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

/** @brief  hostBootLoader_waittingResponeCmdGetId
    @return number of command
*/
static bool hostBootLoader_waittingResponeCmdGetId(void)
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

			printf("\n[hostBootLoader_waittingResponeCmdGetId] chipId : 0x%x\n", hostPri.cmdGetId.PID);

			return true;
		}
	}

	return false;
}

/** @brief  hostBootLoader_waittingResponeCmdErase
    @return uint8_t
*/
static uint8_t hostBootLoader_waittingResponeCmdErase(void)
{
	uint8_t rData = 0;

	switch(hostPri.cmdEraseState)
	{
		case 0: /// waitting ack
		{
			if(rBufferHostBL.len >= 1)
			{
				printf("[hostBootLoader_waittingResponeCmdErase] Erase memory ....\n");
				if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
				{
					if(rData == UART_BOOTLOADER_ACK)
					{
						hostPri.deviceReponseCmdErase = true;
						printf("[hostBootLoader_waittingResponeCmdErase] Erase memory running ....\n");
						hostPri.cmdEraseState = 1;
					}
					else
					{
						hostPri.cmdEraseState = 4;
					}
				}
			}
		}break;
		case 1: /// send number of byte
		{
			uint8_t buffer[2] = {0xff, 0xff};//, 0x00};
			hostBootLoader_sendData(buffer, 2);

			hostPri.cmdEraseState = 2;
		}break;
		case 2: // waitting ack
		{
			if(rBufferHostBL.len >= 1)
			{
				if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
				{
					if(rData == UART_BOOTLOADER_ACK)
					{
						hostPri.cmdEraseState = 3;
					}
					else
					{
						hostPri.cmdEraseState = 4;
					}
				}
			}
		}break;
		case 3:
		{
			/// clear flag reponse cmd erase
			hostPri.deviceReponseCmdErase = false;

			printf("[hostBootLoader_waittingResponeCmdErase] Erase memory successfull ....\n");
		}break;
		case 4:
		{
			/// clear flag reponse cmd erase
			hostPri.deviceReponseCmdErase = false;

			printf("[hostBootLoader_waittingResponeCmdErase] Erase memory fail ....\n");
		}break;
	}

	return hostPri.cmdEraseState;
}

/** @brief  hostBootLoader_readDataFromFlash
    @return bool
*/
static void hostBootLoader_readDataFromFlash(hostBootLoaderPrivate_t *private, uint32_t flashStartAddress)
{
    uint32_t data = 0;

    /// clear buffer
    memset(private->writeData, 0, STM32_MAX_FRAME);

    for(uint16_t i = 0; i < STM32_MAX_FRAME; i+=4)
    {
        /// read flash
        data = *(__IO uint32_t*)flashStartAddress;

        memcpy(&private->writeData[i], (uint8_t *)&data, 4);

        flashStartAddress += 4;
    }
}

/** @brief  hostBootLoader_writeMemCmd
    @return uint8_t
*/
static bool  hostBootLoader_readMemCmd(uint32_t address, uint8_t *data, uint16_t len)
{
	uint8_t cmdBuffer[2] = {hostPri.cmdGet.readMemoryCmd, hostPri.cmdGet.readMemoryCmd ^ 0xff};
	uint8_t addressBuffer[5] = {0};
	uint8_t NOBbuffer[2] = {0};
	uint8_t rData = 0;

    if(!len)
    {
    	printf("\n[hostBootLoader_readMemCmd] len error\n");
        return false;
    }

    if(len > STM32_MAX_FRAME)
    {
        printf("\n[hostBootLoader_readMemCmd] Error: READ length limit at STM32_MAX_FRAME bytes\n");

        return false;
    }

    printf("[BL_readMemory_command] Read memory ....\n");

	hostBootLoader_sendData(cmdBuffer, 2);

	while(rBufferHostBL.len < 1); /// waitting ack 1

	/// read ack 1
	if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
	{
		if(rData != UART_BOOTLOADER_ACK)
		{
			printf("\n[hostBootLoader_writeMemCmd] ack error 1\n");
			return false;
		}
	}

    addressBuffer[0] = address >> 24;
    addressBuffer[1] = (address >> 16) & 0xFF;
    addressBuffer[2] = (address >> 8) & 0xFF;
    addressBuffer[3] = address & 0xFF;
    addressBuffer[4] = addressBuffer[0] ^ addressBuffer[1] ^ addressBuffer[2] ^ addressBuffer[3];

    hostBootLoader_sendData(addressBuffer, 5);

	while(rBufferHostBL.len < 1); /// waitting ack 2

	/// read ack 2
	if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
	{
		if(rData != UART_BOOTLOADER_ACK)
		{
			printf("\n[hostBootLoader_writeMemCmd] ack error 2\n");
			return false;
		}
	}

	NOBbuffer[0] = len - 1;
	NOBbuffer[1] = NOBbuffer[0] ^ 0xff;

	hostBootLoader_sendData(NOBbuffer, 2);

	while(rBufferHostBL.len < 1); /// waitting ack 2

	/// read ack 2
	if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
	{
		if(rData != UART_BOOTLOADER_ACK)
		{
			printf("\n[hostBootLoader_writeMemCmd] ack error 2\n");
			return false;
		}
	}


	while(rBufferHostBL.len < STM32_MAX_FRAME); /// waitting data

	for(uint16_t i = 0; i < STM32_MAX_FRAME; i++)
	{
		/// read data
		if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
		{
			data[i] = rData;
		}
	}

	return true;
}

/** @brief  hostBootLoader_writeMemCmd
    @return uint8_t
*/
static bool hostBootLoader_verifyMemory(uint32_t address, uint8_t *dataVerify, uint16_t len)
{
	uint8_t cmpBuffer[STM32_MAX_FRAME];
	uint16_t offset = 0;
	uint16_t rlen = 0;

	printf("\n[hostBootLoader_verifyMemory] Verify memory address 0x%x\n", (int)address);

	while(offset < len)
	{
        rlen = len - offset;
        rlen = rlen < STM32_MAX_FRAME ? rlen : STM32_MAX_FRAME;

        uint8_t state = 0;
        state = hostBootLoader_readMemCmd(address + offset, cmpBuffer + offset, rlen);
        if(state != true)
        {
        	printf("\n[hostBootLoader_verifyMemory] read memory address 0x%x fail\n", (int)(address + offset));
            return false;
        }

        offset += rlen;
	}

    /// compare data
    for(uint16_t i = 0; i < len; i++)
    {
        if(dataVerify[i] != cmpBuffer[i])
        {
			printf("Failed to verify at address 0x%08x, expected 0x%02x and found 0x%02x\n", (int)(address + i), (int)dataVerify[i], (int)cmpBuffer[i]);

			return false;
        }
    }

    printf("[hostBootLoader_verifyMemory] Verify memory successfull\n");

	return true;
}

/** @brief  hostBootLoader_writeMemCmd
    @return bool
*/
static bool hostBootLoader_writeMemCmd(uint32_t address, uint8_t *data, uint16_t len)
{
	uint8_t cmdBuffer[2] = {hostPri.cmdGet.writeMemoryCmd, hostPri.cmdGet.writeMemoryCmd ^ 0xff};
	uint8_t addressBuffer[5] = {0};
	uint8_t cs = 0;
	uint8_t dataBuffer[STM32_MAX_FRAME + 2] = {0};
	uint16_t i, aligned_len;
	uint8_t rData = 0;

	if(!len)
	{
		printf("\n[hostBootLoader_writeMemCmd] len error 1\n");
		return false;
	}

	if(len > STM32_MAX_FRAME)
	{
		printf("\n[hostBootLoader_writeMemCmd] len error 2\n");
		return false;
	}

	uint32_t check = address & 0x30;
	if(check)
	{
		printf("\n[hostBootLoader_writeMemCmd] address error 1\n");
		return false;
	}

	hostBootLoader_sendData(cmdBuffer, 2);

	while(rBufferHostBL.len < 1); /// waitting ack 1

	/// read ack 1
	if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
	{
		if(rData == UART_BOOTLOADER_NACK)
		{
			printf("\n[hostBootLoader_writeMemCmd] ack error 1\n");
			return false;
		}
		else
		{
//			hostPri.deviceReponseCmdWrite = true;
		}
	}

    addressBuffer[0] = address >> 24;
    addressBuffer[1] = (address >> 16) & 0xFF;
    addressBuffer[2] = (address >> 8) & 0xFF;
    addressBuffer[3] = address & 0xFF;
    addressBuffer[4] = addressBuffer[0] ^ addressBuffer[1] ^ addressBuffer[2] ^ addressBuffer[3];

    hostBootLoader_sendData(addressBuffer, 5);

	while(rBufferHostBL.len < 1); /// waitting ack 2

	/// read ack 2
	if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
	{
		if(rData == UART_BOOTLOADER_NACK)
		{
			printf("\n[hostBootLoader_writeMemCmd] ack error 2\n");
			return false;
		}
	}

    aligned_len = (len + 3) & ~3;
    cs = aligned_len - 1;
    dataBuffer[0] = aligned_len - 1;
    for (i = 0; i < len; i++)
    {
        cs ^= data[i];
        dataBuffer[i + 1] = data[i];
    }
    /* padding data */
    for (i = len; i < aligned_len; i++)
    {
        cs ^= 0xFF;
        dataBuffer[i + 1] = 0xFF;
    }
    dataBuffer[aligned_len + 1] = cs;

    hostBootLoader_sendData(dataBuffer, aligned_len + 2);

	while(rBufferHostBL.len < 1); /// waitting ack 3

	/// read ack 3
	if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
	{
		if(rData == UART_BOOTLOADER_NACK)
		{
			printf("\n[hostBootLoader_writeMemCmd] ack error 3\n");
			return false;
		}
		else
		{
//			hostPri.deviceReponseCmdWrite = false;
			printf("\n[hostBootLoader_writeMemCmd] write memory successful\n");
		}
	}

	return true;
}

/** @brief  hostBootLoader_waittingResponeCmdWriteMem
    @return bool
*/
static bool hostBootLoader_waittingResponeCmdWriteMem(void)
{
    uint32_t flashAddress = 0x08020000;
    uint32_t maxDataLength = 0xe98;//0x2b6b8;
    uint32_t start = 0x08020000;
    uint32_t end = start + maxDataLength;
    uint32_t left;
    uint16_t len = 0;
    uint32_t addr = start;
    uint32_t offset = 0;
    uint16_t size = end - start;

    while(addr < end && offset < size)
    {
    	switch(hostPri.cmdWriteMemState)
    	{
    		case 0: /// read source data from flash
    		{
    			hostBootLoader_readDataFromFlash(&hostPri, flashAddress);

    			hostPri.cmdWriteMemState = 1;
    		}break;
    		case 1: /// calculator address offset and write to memory
    		{
    	        left = end - addr;
    	        len = STM32_MAX_FRAME > left ? left : STM32_MAX_FRAME;
    	        len = len > (size - offset) ? (size - offset) : len;

    			if(hostBootLoader_writeMemCmd(addr, hostPri.writeData, STM32_MAX_FRAME) == true)
    			{
    				hostPri.cmdWriteMemState = 2;
    			}
    		}break;
    		case 2: /// verify memory after write
    		{
    			static bool state = false;
    			state = hostBootLoader_verifyMemory(addr, hostPri.writeData, STM32_MAX_FRAME);
    			if(state == true)
    			{
    				state = false;

    				hostPri.cmdWriteMemState = 3;
    			}
    		}break;
    		case 3: /// calculator next address
    		{
    	        addr += len;
    	        offset += len;
    	        flashAddress += len;
    	        printf("\r[hostBootLoader_waittingResponeCmdWriteMem] Wrote address 0x%08x (%.2f%%) ", (int)addr, (100.0f / size) * offset);

    	        hostPri.cmdWriteMemState = 0;
    		}break;
    		case 4:
    		{

    		}break;
    		case 5:
    		{

    		}break;
    	}
    }

	return true;
}

/** @brief  hostBootLoader_waittingResponeCmdGo
    @return bool
*/
static bool hostBootLoader_waittingResponeCmdGo(void)
{
	uint8_t rData = 0;

	if(rBufferHostBL.len == 1)
	{
		if(ringBufferRead(&rBufferHostBL, &rData) == RING_BUFFER_OK)
		{
			if(rData == UART_BOOTLOADER_ACK)
			{
				printf("\n[hostBootLoader_waittingResponeCmdGo] device disconnect !@!\n");
				return true;
			}
		}
	}
	return false;
}

/** @brief  hostBootLoader_readCommand
    @return number of command
*/
static hostBootLoaderState_t hostBootLoader_readCommand(void)
{
	static hostBootLoaderState_t state = HOST_BOOTLOADER_STATE_CONNECTED;

		switch(host->hostCmd[0])
		{
			case UART_BOOTLOADER_CMD_CONNECT:
			{
				if(hostBootLoader_waittingResponeCmdConnect() == true)
				{
					hostPri.timeSendCmd = 0;
					state = HOST_BOOTLOADER_STATE_CMD_GET;
				}
				else
				{

				}
			}break;
			case UART_BOOTLOADER_CMD_GET:
			{
				if(hostBootLoader_waittingResponeCmdGet() == true)
				{
					hostPri.timeSendCmd = 0;
					state = HOST_BOOTLOADER_STATE_GET_ID;
				}
				else
				{

				}
			}break;
			case UART_BOOTLOADER_CMD_GET_VER:
			{

			}break;
			case UART_BOOTLOADER_CMD_GET_ID:
			{
				if(hostBootLoader_waittingResponeCmdGetId() == true)
				{
					hostPri.timeSendCmd = 0;
					state = HOST_BOOTLOADER_STATE_ERASE;
				}
				else
				{

				}
			}break;
			case UART_BOOTLOADER_CMD_READ_MEMORY:
			{

			}break;
			case UART_BOOTLOADER_CMD_GO:
			{
				if(hostBootLoader_waittingResponeCmdGo() == true)
				{
					state = HOST_BOOTLOADER_STATE_DONE;
				}
			}break;
			case UART_BOOTLOADER_CMD_WRITE_MEMORY:
			{
				if(hostBootLoader_waittingResponeCmdWriteMem() == true)
				{
					hostPri.timeSendCmd = 0;
					state = HOST_BOOTLOADER_STATE_CMD_GO;
				}
			}break;
			case UART_BOOTLOADER_CMD_ERASE:
			{
				static uint8_t _state = 0;
				_state = hostBootLoader_waittingResponeCmdErase();
				if(_state == 3)
				{
					_state = 0;
					hostPri.timeSendCmd = 0;
					state = HOST_BOOTLOADER_STATE_WRITE;
				}
				else if(state == 4)
				{
					_state = 0;
					state = HOST_BOOTLOADER_STATE_ERROR;
				}
				else
				{

				}
			}break;
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
	static uint8_t lostConnectCount = 0;

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
				case HOST_BOOTLOADER_STATE_CONNECTED:
				{
					if(getTime(&hostPri.timeSendCmd, 1000) == true)
					{
						if(++lostConnectCount > 5)
						{
							lostConnectCount = 0;
							hostBootLoader_uartConfigForBL();

							printf("\n[hostBootLoader_waittingResponeCmdConnect] device lost connect -> try again ...\n");
						}
						else
						{
							printf("\n[hostBootLoader_waittingResponeCmdConnect] waitting device connect ...\n");
							hostBootLoader_sendCommand(UART_BOOTLOADER_CMD_CONNECT, 1);
						}
					}
				}break;
				case HOST_BOOTLOADER_STATE_CMD_GET:
				{
					if(getTime(&hostPri.timeSendCmd, 1000) == true)
					{
						printf("\n[hostBootLoader_waittingResponeCmdConnect] waitting device reponse cmd get ...\n");
						hostBootLoader_sendCommand(hostPri.cmdGet.getCmd, 2);
					}
				}break;
				case HOST_BOOTLOADER_STATE_GET_ID:
				{
					if(getTime(&hostPri.timeSendCmd, 1000) == true)
					{
						printf("\n[hostBootLoader_waittingResponeCmdConnect] waitting device reponse cmd getId ...\n");
						hostBootLoader_sendCommand(hostPri.cmdGet.getId, 2);
					}
				}break;
				case HOST_BOOTLOADER_STATE_ERASE:
				{
					if(getTime(&hostPri.timeSendCmd, 1000) == true)
					{
						if(hostPri.deviceReponseCmdErase == false)
						{
							printf("\n[hostBootLoader_waittingResponeCmdConnect] waitting device reponse cmd erase ...\n");
							hostBootLoader_sendCommand(hostPri.cmdGet.EraseCmd, 2);
						}
					}
				}break;
				case HOST_BOOTLOADER_STATE_WRITE:
				{
					if(getTime(&hostPri.timeSendCmd, 1000) == true)
					{
						if(hostPri.deviceReponseCmdWrite == false)
						{
							printf("\n[hostBootLoader_waittingResponeCmdConnect] waitting device reponse cmd write memory ...\n");
							host->hostCmd[0] = hostPri.cmdGet.writeMemoryCmd;
							host->hostCmd[1] = host->hostCmd[0] ^ 0xff;
						}
					}
				}break;
				case HOST_BOOTLOADER_STATE_CMD_GO:
				{
					if(getTime(&hostPri.timeSendCmd, 1000) == true)
					{
						printf("\n[hostBootLoader_waittingResponeCmdConnect] waitting device reponse cmd go ...\n");
						hostBootLoader_sendCommand(hostPri.cmdGet.goCmd, 2);
					}
				}break;
				case HOST_BOOTLOADER_STATE_DONE:
				{
					if(getTime(&timeSendCmd, 5000) == true)
					{
						printf("\n[hostUartBootLoaderProcess] Upragde fw successfull ..............\n");
					}
				}break;
				case HOST_BOOTLOADER_STATE_ERROR:
				{
					if(getTime(&timeSendCmd, 1000) == true)
					{
						printf("\n[hostUartBootLoaderProcess] HOST_BOOTLOADER_STATE_ERROR\n");
					}
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

