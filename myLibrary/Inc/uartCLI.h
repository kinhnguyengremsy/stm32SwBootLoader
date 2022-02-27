/** 
  ******************************************************************************
  * @file    uartCLI.h
  * @author  Gremsy Team
  * @version v1.0.0
  * @date    2021
  * @brief   This file contains all the functions prototypes for the  
  *          firmware library.
  *
  ******************************************************************************
  * @Copyright
  * COPYRIGHT NOTICE: (c) 2011 Gremsy. All rights reserved.
  *
  * The information contained herein is confidential
  * property of Company. The use, copying, transfer or 
  * disclosure of such information is prohibited except
  * by express written agreement with Company.
  *
  ******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/

#ifndef __UART_CLI_H
#define __UART_CLI_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "ringBuffer.h"
/* Exported define ------------------------------------------------------------*/
#define MAX_NUMBER_CMD_CLI	(8 + 1)
/* Exported types ------------------------------------------------------------*/
typedef enum _cmdIndex_t
{
	CMD_CLI_NONE,
	CMD_CLI_00,
	CMD_CLI_01,
	CMD_CLI_02,
	CMD_CLI_03,
	CMD_CLI_04,
	CMD_CLI_05,
	CMD_CLI_06,
	CMD_CLI_07,
	CMD_CLI_08,

}cmdIndex_t;

typedef struct
{
	bool flagMsgHeartBeat;
	bool flagMsgJumTarget;
	bool flagStorageWrite;
	bool flagStorageRead;

	int value[10];
	bool isHaveValue;

	ringBuffer_t vSet;
}uartCLI_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  uartCLI_configuration
    @return none
*/
void uartCLI_configuration(uartCLI_t *_cli);

/** @brief  __cmdCli
    @return pointer struct support uart cli
*/
uartCLI_t* __cmdCli(void);

/** @brief  uartCLI_process
    @return none
*/
void uartCLI_process(void);

#endif /* __UART_CLI_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


