/** 
  ******************************************************************************
  * @file    uartBootLoader.h
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

#ifndef __UART_BOOTLOADER_H
#define __UART_BOOTLOADER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/

#define UART_BOOTLOADER_ACK     0x79
#define UART_BOOTLOADER_NACK    0x1F

#define UART_BOOTLOADER_CMD_CONNECT              0x7F
#define UART_BOOTLOADER_CMD_GET                  0x00
#define UART_BOOTLOADER_CMD_GET_VER              0x01
#define UART_BOOTLOADER_CMD_GET_ID               0x02
#define UART_BOOTLOADER_CMD_READ_MEMORY          0x11
#define UART_BOOTLOADER_CMD_GO                   0x21
#define UART_BOOTLOADER_CMD_WRITE_MEMORY         0x31
#define UART_BOOTLOADER_CMD_ERASE                0x44
#define UART_BOOTLOADER_CMD_WRITE_PROTECT        0x63
#define UART_BOOTLOADER_CMD_WRITE_UNPROTECT      0x73
#define UART_BOOTLOADER_CMD_READ_PROTECT         0x82
#define UART_BOOTLOADER_CMD_READ_UNPROTECT       0x92
/* Exported types ------------------------------------------------------------*/

typedef enum _bootLoaderState_t
{
	BOOTLOADER_STATE_IDLE = 0x00,
	BOOTLOADER_STATE_CONNECTED,
	BOOTLOADER_STATE_ERASE,
	BOOTLOADER_STATE_WRITE,
	BOOTLOADER_STATE_DONE,
	BOOTLOADER_STATE_ERROR,

}bootLoaderState_t;

typedef struct _bootLoaderGetCmd_t
{
	uint8_t numberOfbyte;
	uint8_t version;
	uint8_t support;

}bootLoaderGetCmd_t;

typedef struct _bootLoaderGetVersionAndReadProtectionCmd_t
{
	uint8_t version;
	uint8_t optionByte1;
	uint8_t optionByte2;

}bootLoaderGetVersionAndReadProtectionCmd_t;

typedef struct _bootLoaderGetIdCmd_t
{
	uint8_t numberOfbyte;

	union _id
	{
		uint8_t byte1;
		uint8_t Pid;
	}id;

	uint8_t byte2;

}bootLoaderGetIdCmd_t;

typedef struct _uartBootLoader_t
{
//	uint8_t *txBuffer;
//	uint8_t *rxBuffer;

	uint8_t rxData;

	bootLoaderState_t state;

	bootLoaderGetCmd_t getCmd;
	bootLoaderGetVersionAndReadProtectionCmd_t versionAndReadProtection;
	bootLoaderGetIdCmd_t getId;

}uartBootLoader_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  uartBootLoaderConfiguration
    @return none
*/
void uartBootLoaderConfiguration(void);

/** @brief  uartBootLoaderProcess
    @return none
*/
void uartBootLoaderProcess(void);

#endif /* __UART_BOOTLOADER_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


