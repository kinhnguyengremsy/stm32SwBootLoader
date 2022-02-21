/** 
  ******************************************************************************
  * @file    ringBuffer.h
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

#ifndef __RING_BUFFER_H
#define __RING_BUFFER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/
#define RING_BUFFER_LEN	270
/* Exported types ------------------------------------------------------------*/
typedef enum
{
	RING_BUFFER_ERROR 	= 0x00,
	RING_BUFFER_OK		= 0x01,

}ringBufferState_t;

typedef struct _ringBuffer_t
{
	uint16_t 	head;
	uint16_t 	tail;
	uint16_t 	len;
	uint8_t		buffer[RING_BUFFER_LEN];
}ringBuffer_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/
/** @brief  ringBufferInit
    @return none
*/
void ringBufferInit(ringBuffer_t *rBuff);

/** @brief  ringBufferWrite
    @return ringBufferState_t
*/
ringBufferState_t ringBufferWrite(ringBuffer_t *rBuff, uint8_t wData);

/** @brief  ringBufferRead
    @return ringBufferState_t
*/
ringBufferState_t ringBufferRead(ringBuffer_t *rBuff, uint8_t *rData);

#endif /* __RING_BUFFER_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


