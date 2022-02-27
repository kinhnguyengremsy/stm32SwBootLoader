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
#include "mavlinkControl.h"
#include <math.h>
/* Private define------------------------------------------------------------------------------*/

/* Private typedef------------------------------------------------------------------------------*/

struct _cmdInfo_t
{
	char *cmdName;
}cmdInfo_t[MAX_NUMBER_CMD_CLI] = {
		{.cmdName = NULL},
		{.cmdName = "sendMsgHeartBeat"},
		{.cmdName = "stopMsgHeartBeat"},
		{.cmdName = "sendMsgJumTarget"},
		{.cmdName = "stopMsgJumTarget"},//4
		{.cmdName = "setDummyValue"},
		{.cmdName = "writeFlash"},
		{.cmdName = "readFlash"},

		{.cmdName = "Help"},
};
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
extern UART_HandleTypeDef 	huart1;

ringBuffer_t 		rBufferRxU1;
uint8_t 			usart1WData;
bool 				endCmd;
static uartCLI_t			*cli;

/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __UART_CLI_CONFIGURATION
    @{
*/#ifndef __UART_CLI_CONFIGURATION
#define __UART_CLI_CONFIGURATION
/** @brief  uartCLI_configuration
    @return none
*/
void uartCLI_configuration(uartCLI_t *_cli)
{
	 cli = _cli;

	if(HAL_UART_Receive_DMA(&huart1, &usart1WData, 1) != HAL_OK)
	{
		Error_Handler();
	}
}

/** @brief  __cmdCli
    @return pointer struct support uart cli
*/
uartCLI_t* __cmdCli(void)
{
	return cli;
}

#endif
/**
    @}
*/

/** @group __UART_CLI_FUNCTIONS
    @{
*/#ifndef __UART_CLI_FUNCTIONS
#define __UART_CLI_FUNCTIONS
/** @brief  uartCLI_getCmd
    @return position cmd
*/
static cmdIndex_t uartCLI_getCmd(void)
{
	uint8_t rData;
	bool isCmdSetValue = false;

	if(endCmd == true)
	{
		endCmd = false;

		uint8_t cmdLength = rBufferRxU1.len - 1; /// bo ky tu '\n'
		char cmdRecBuffer[cmdLength];

		memset(cmdRecBuffer, 0, cmdLength);

		for(uint32_t i = 0; i < cmdLength; i++)
		{
			if(ringBufferRead(&rBufferRxU1, &rData))
			{
				/// cmd value other
				if(isCmdSetValue == true)
				{
					ringBufferWrite(&cli->vSet, rData);
				}
				else
				{
					if(rData == '=')
					{
						isCmdSetValue = true;
						cli->isHaveValue = true;
					}
					else
					{
						cmdRecBuffer[i] = rData;
					}
				}
			}
		}

		/// lay ra ky tu '\n'
		ringBufferRead(&rBufferRxU1, &rData);

		for(uint8_t j = 1; j < MAX_NUMBER_CMD_CLI; j++)
		{
			uint16_t len = strlen(cmdInfo_t[j].cmdName);
			if(!memcmp(cmdInfo_t[j].cmdName, cmdRecBuffer, len))//(cmdLength - (cli->vSet.len + 1))))
			{
				printf("\n[uartCLI_getCmd] cmd name : %s\n", cmdInfo_t[j].cmdName);
				HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
				return (cmdIndex_t)j;
			}
		}

		printf("\n[uartCLI_getCmd] cmd not found");
		printf("\n[uartCLI_getCmd] please enter 'Help' to find command format\n");
	}

	return CMD_CLI_NONE;
}

/** @brief  cmdCLI_getValueSet
    @return none
*/
static void cmdCLI_getValueSet(ringBuffer_t *rB, int len, int *value)
{
	uint8_t valueSetLength = rB->len;
	uint8_t rData = 0;
	bool nextValue = false;
	uint8_t nextValueCount = 0;

	if(cli->isHaveValue == true)
	{
		cli->isHaveValue = false;

		for(uint8_t i = 0; i < valueSetLength; i++)
		{
			if(ringBufferRead(rB, &rData) == RING_BUFFER_OK)
			{
				/// get next value
				if(nextValue == true)
				{
					nextValueCount++;
					value[1] += (rData - 48) * pow(10, rB->len);
				}
				else
				{
					if(rData == ',')
					{
						nextValue = true;
						nextValueCount++;
					}
					else
					{
						value[0] += (rData - 48) * pow(10, rB->len);
					}
				}
			}
		}

		/// tinh lai gia tri value[0] khi co 2 gia tri
		if(nextValue == true)
		{
			value[0] = value[0] / pow(10, nextValueCount);
		}

		for(uint8_t j = 0; j < len; j++)
		{
			printf("\nvalue set%d = %d\n", (int)j, value[j]);
		}

	}
}

/** @brief  uartCLI_cmdHelp
    @return none
*/
static void uartCLI_cmdHelp(void)
{
	printf("\n[uartCLI_cmdHelp] command list\n");
	for(uint8_t i = 1; i < MAX_NUMBER_CMD_CLI - 1; i++)
	{
		printf("%s\n", cmdInfo_t[i].cmdName);
	}

	printf("Command nomal example : {'cmd name'}");
	printf("Command set example : {cmd name = 'value1','value2'}");

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
	static cmdIndex_t cmdReciever = 0;

	switch((uint8_t)cmdReciever)
	{
		case CMD_CLI_NONE:
		{
			cmdReciever = uartCLI_getCmd();
			HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		}break;
		case CMD_CLI_00:
		{
			cli->flagMsgHeartBeat = true;
			cmdReciever = CMD_CLI_NONE;
		}break;
		case CMD_CLI_01:
		{
			cli->flagMsgHeartBeat = false;
			cmdReciever = CMD_CLI_NONE;
		}break;
		case CMD_CLI_02:
		{
			cli->flagMsgJumTarget = true;
			cmdReciever = CMD_CLI_NONE;
		}break;
		case CMD_CLI_03:
		{
			cli->flagMsgJumTarget = false;
			cmdReciever = CMD_CLI_NONE;
		}break;
		case CMD_CLI_05:
		{
			int value[2] = {0, 0};

			cli->flagStorageWrite = true;
			cmdCLI_getValueSet(&cli->vSet, 2, value);
			cli->value[CMD_CLI_04] = value[0];
			cli->value[CMD_CLI_05] = value[1];
			cmdReciever = CMD_CLI_NONE;
		}break;
		case CMD_CLI_06:
		{
			int value = 0;

			cli->flagStorageRead = true;
			cmdCLI_getValueSet(&cli->vSet, 1, &value);
			cli->value[CMD_CLI_06] = value;
			cmdReciever = CMD_CLI_NONE;
		}break;
		case CMD_CLI_07:
		{
			uartCLI_cmdHelp();
			cmdReciever = CMD_CLI_NONE;
		}break;
	}
}


#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

