/** 
  ******************************************************************************
  * @file    serialPort.h
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

#ifndef __SERIAL_PORT_H
#define __SERIAL_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stdlib.h"
#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "main.h"
/* Exported define ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
    UART_HandleTypeDef  uartHandle;
    DMA_HandleTypeDef   dmaTx;
    DMA_HandleTypeDef   dmaRx;
    uint16_t    uartDataCount;
    uint8_t     readBuffer[256];
    uint8_t     writeBuffer[1000];
    uint8_t     writeBufferTemp[1000];
    uint8_t     writeBufferPtr;
}serialPort_private_t;

typedef struct
{
    uint32_t time;

    uint8_t readBuffer[256];
    uint8_t bytesToRead;
    uint8_t readBufferSize;

    bool isWriteFinish;

    serialPort_private_t   zPrivate;
}serialPort_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @brief    serialPort_read
	@param[1] serial
	@param[2] buffsize
	@return   bool
*/
bool serialPort_read(serialPort_t* serial, uint16_t buffsize);

/** @brief    serialPort_tx_finish
	@param[1] serial
	@return   none
*/
void serialPort_tx_finish(serialPort_t* serial);

/** @brief    serialPort_write
	@param[1] serial
	@param[2] buff
	@param[3] len
	@return   none
*/
void serialPort_write(serialPort_t* serial, uint8_t* buff, uint8_t len);

/** @brief    serialPort_write_list
	@param[1] serial
	@param[2] buff
	@param[3] ...
	@return   none
*/
void serialPort_write_list(serialPort_t* serial, void* buff,...);

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_PORT_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

