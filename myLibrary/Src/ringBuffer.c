/**
  ******************************************************************************
  * @file    ringBuffer.c
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
#include "ringBuffer.h"
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/

/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __RING_BUFFER_CONFIGURATION
    @{
*/#ifndef __RING_BUFFER_CONFIGURATION
#define __RING_BUFFER_CONFIGURATION
/** @brief  ringBufferInit
    @return none
*/
void ringBufferInit(ringBuffer_t *rBuff)
{
	rBuff->head = 0;
	rBuff->tail = 0;
	rBuff->len = 0;
}

#endif
/**
    @}
*/

/** @group __RING_BUFFER_FUNCTION
    @{
*/#ifndef __RING_BUFFER_FUNCTION
#define __RING_BUFFER_FUNCTION
/** @brief  ringBufferWrite
    @return ringBufferState_t
*/
ringBufferState_t ringBufferWrite(ringBuffer_t *rBuff, uint8_t wData)
{
	if(rBuff->len >= RING_BUFFER_LEN)
	{
		return RING_BUFFER_ERROR;
	}

	rBuff->buffer[rBuff->tail] = wData;
	rBuff->tail = (rBuff->tail + 1) % RING_BUFFER_LEN;
	rBuff->len++;

	return RING_BUFFER_OK;
}

/** @brief  ringBufferRead
    @return ringBufferState_t
*/
ringBufferState_t ringBufferRead(ringBuffer_t *rBuff, uint8_t *rData)
{
	if(rBuff->len == 0)
	{
		return RING_BUFFER_ERROR;
	}

	*rData = rBuff->buffer[rBuff->head];

	rBuff->head = (rBuff->head + 1) % RING_BUFFER_LEN;
	rBuff->len--;

	return RING_BUFFER_OK;
}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

