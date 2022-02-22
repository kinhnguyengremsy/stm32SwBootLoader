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

#define BOOTLOADER_VERSION		0x31

#define UART_BOOTLOADER_ACK     0x79
#define UART_BOOTLOADER_NACK    0x1F

#define UART_BOOTLOADER_CMD_CONNECT              0x7F
#define UART_BOOTLOADER_CMD_GET                  0x00
#define UART_BOOTLOADER_CMD_GET_VER              0x01
#define UART_BOOTLOADER_CMD_GET_ID               0x02
#define UART_BOOTLOADER_CMD_READ_MEMORY          0x11
#define UART_BOOTLOADER_CMD_GO                   0x21
#define UART_BOOTLOADER_CMD_WRITE_MEMORY         0x31
#define UART_BOOTLOADER_CMD_ERASE                0x43
#define UART_BOOTLOADER_CMD_WRITE_PROTECT        0x63
#define UART_BOOTLOADER_CMD_WRITE_UNPROTECT      0x73
#define UART_BOOTLOADER_CMD_READ_PROTECT         0x82
#define UART_BOOTLOADER_CMD_READ_UNPROTECT       0x92
#define UART_BOOTLOADER_CMD_GET_CHECKSUM	     0xA1

#define BOOTLOADER_CMD_LEN			 			 2
#define BOOTLOADER_CMD_GET_LEN					 16
#define BOOTLOADER_CMD_GET_VER_LEN				 5
#define BOOTLOADER_CMD_GET_ID_LEN 				 5
#define BOOTLOADER_CMD_ERASE_LEN 				 1

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

typedef enum _bootLoaderGetState_t
{
	BOOTLOADER_GET_STATE_IDLE,
	BOOTLOADER_GET_STATE_CMD_GET_ID,
	BOOTLOADER_GET_STATE_CMD_GET,
	BOOTLOADER_GET_STATE_CMD_GET_ID2,
	BOOTLOADER_GET_STATE_CMD_GET_VER,
	BOOTLOADER_GET_STATE_DONE,
	BOOTLOADER_GET_STATE_ERROR,

}bootLoaderGetState_t;

typedef enum bootLoaderCmdWriteResult_t
{
	BOOTLOADER_CMD_WRITE_RESULT_IDLE,
	BOOTLOADER_CMD_WRITE_RESULT_ERROR,
	BOOTLOADER_CMD_WRITE_RESULT_OK,
}bootLoaderCmdWriteResult_t;

typedef enum
{
	BOOTLOADER_CMD_NONE = 0x00,
	BOOTLOADER_CMD_GET,
	BOOTLOADER_CMD_GET_VER,
	BOOTLOADER_CMD_GET_ID,
	BOOTLOADER_CMD_READ_MEMORY,
	BOOTLOADER_CMD_GO,
	BOOTLOADER_CMD_WRITE_MEMORY,
	BOOTLOADER_CMD_ERASE,
	BOOTLOADER_CMD_WRITE_PROTECT,
	BOOTLOADER_CMD_WRITE_UNPROTECT,
	BOOTLOADER_CMD_READ_PROTECT,
	BOOTLOADER_CMD_READ_UNPROTECT,
	BOOTLOADER_CMD_GET_CHECKSUM,
	BOOTLOADER_CMD_GET_TOTAL,

}bootLoaderCmd_t;

typedef struct _bootLoaderGetCmd_t
{
	uint8_t numberOfbyte;
	uint8_t version;
	uint8_t getCmd;
	uint8_t getVerAndRPStatus;
	uint8_t getId;
	uint8_t readMemoryCmd;
	uint8_t goCmd;
	uint8_t writeMemoryCmd;
	uint8_t EraseCmd;
	uint8_t writeProtectCmd;
	uint8_t writeUnProtectCmd;
	uint8_t readOutProtectCmd;
	uint8_t readOutUnProtectCmd;
	uint8_t getChecksumCmd;

}bootLoaderGetCmd_t;

typedef struct _bootLoaderGetVersionAndReadProtectionCmd_t
{
	uint8_t version;
	uint8_t optionByte1;
	uint8_t optionByte2;

}bootLoaderGetVerCmd_t;

typedef struct _bootLoaderGetIdCmd_t
{
	uint8_t numberOfbyte;
	uint8_t byte3;
	uint8_t byte4;

	uint16_t PID;

}bootLoaderGetIdCmd_t;

typedef struct _uartBootLoader_t
{
//	uint8_t *txBuffer;
//	uint8_t *rxBuffer;

	uint8_t rxData;

	bootLoaderState_t state;

	bootLoaderCmd_t rCmd;

	bootLoaderGetCmd_t getCmd;
	bootLoaderGetVerCmd_t getVer;
	bootLoaderGetIdCmd_t getId;

	uint32_t flashAddress;

}uartBootLoader_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  uartBootLoaderConfiguration
    @return none
*/
void uartBootLoaderConfiguration(void);

/** @brief	uartBootLoaderChecksumCalculator
 *  @param[in] beginChecksum : so checksum ban dau
 *  @param[in] buffer : mang can tinh checksum
 *  @param[in] len : do dai cua mang tinh checksum
    @return	none
*/
uint8_t uartBootLoaderChecksumCalculator(uint8_t beginChecksum, uint8_t *buffer, uint16_t len);

/** @brief  uartBootLoaderProcess
    @return none
*/
void uartBootLoaderProcess(void);

#endif /* __UART_BOOTLOADER_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


