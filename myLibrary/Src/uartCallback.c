/**
  ******************************************************************************
  * @file    uartCallback.c
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
#include "uartCallback.h"
#include "serialPort.h"
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
extern serialPort_t serial_port2;
extern serialPort_t serial_port4;

uartCallback_t callback;
uartCallback_t callback4;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __UART_TX_CALLBACK
    @{
*/#ifndef __UART_TX_CALLBACK
#define __UART_TX_CALLBACK
/** @brief
    @return
*/
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(serial_port2.zPrivate.uartHandle.Instance == USART2)
    {
        serialPort_tx_finish(&serial_port2);
    }

    if(serial_port4.zPrivate.uartHandle.Instance == UART4)
    {
        serialPort_tx_finish(&serial_port4);
    }
}

#endif
/**
    @}
*/

/** @group __UART_RX_CALLBACK
    @{
*/#ifndef __UART_RX_CALLBACK
#define __UART_RX_CALLBACK
/** @brief
    @return
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(serial_port2.zPrivate.uartHandle.Instance == USART2)
	{
		callback.rxComplete = true;
	}

	if(serial_port4.zPrivate.uartHandle.Instance == UART4)
	{
		callback4.rxComplete = true;
	}
}

#endif
/**
    @}
*/

/** @group __UART_TX_RX_CALLBACK
    @{
*/#ifndef __UART_TX_RX_CALLBACK
#define __UART_TX_RX_CALLBACK
/** @brief
    @return
*/


#endif
/**
    @}
*/

/** @group __UART_ERROR_CALLBACK
    @{
*/#ifndef __UART_ERROR_CALLBACK
#define __UART_ERROR_CALLBACK
/** @brief  
    @return 
*/


#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

