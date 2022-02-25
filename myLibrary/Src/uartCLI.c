/**
  ******************************************************************************
  * @file    uartCLI.c
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
#include "uartCLI.h"
#include "ringBuffer.h"
/* Private typedef------------------------------------------------------------------------------*/
struct _cmdInfo_t
{
	char *cmdName;
}cmdInfo_t[2] = {
		{.cmdName = "{.cmdName = }"},
		{.cmdName = "sendMsgJumTarget"}
};
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
ringBuffer_t 		rBufferRxU1;
uint8_t 			usart1WData;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __UART_CLI_CONFIGURATION
    @{
*/#ifndef __UART_CLI_CONFIGURATION
#define __UART_CLI_CONFIGURATION
/** @brief  uartCLI_configuration
    @return none
*/
void uartCLI_configuration(void)
{

}

#endif
/**
    @}
*/

/** @group __UART_CLI_FUNCTIONS
    @{
*/#ifndef __UART_CLI_FUNCTIONS
#define __UART_CLI_FUNCTIONS
/** @brief  
    @return 
*/
static bool uartCLI_getCmd(char *cmd)
{
	uint8_t rData;
	uint32_t cmdLenght = strlen(cmd);
	uint8_t cmdRecBuffer[cmdLength];

	if(rBufferRxU1.len >= cmdLenght)
	{
		memset(cmdRecBuffer, 0, cmdLenght);
		for(uint32_t i = 0; i < cmdLenght; i++)
		{
			if(ringBufferRead(&rBufferRxU1, &rData))
			{
				sprintf(cmdRecBuffer, "%x", rData);
			}

			printf("");

		}
	}
}

#endif
/**
    @}
*/

/** @group __UART_CLI_PROCESS
    @{
*/#ifndef __UART_CLI_PROCESS
#define __UART_CLI_PROCESS
/** @brief  uartCLI_process
    @return none
*/
void uartCLI_process(void)
{

}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

