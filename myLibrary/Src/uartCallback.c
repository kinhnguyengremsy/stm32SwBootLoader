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
#if (USE_HOST_BOOTLOADER == 1)
	#include "hostUartBootLoader.h"
	#include "serialPort.h"
#endif
#if (USE_DEVICE_BOOTLOADER == 1)
	#include "ringBuffer.h"
#endif
#if (USE_MAVLINK_BOOTLOADER == 1)
	#include "serialPort.h"
#endif
#if (USE_MAVLINK_CONTROL == 1)
	#include "serialPort.h"
#endif

#if (USE_COMMAND_LINE_INTERFACE == 1)
	#include "ringBuffer.h"
#endif
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/

/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
#if (USE_HOST_BOOTLOADER == 1)
	extern serialPort_t 		serial_port2;
	extern UART_HandleTypeDef 	huart2;
	extern ringBuffer_t 		rBufferHostBL;
	extern uint8_t 				wData;
	bool txComplete;
#endif
#if (USE_DEVICE_BOOTLOADER == 1)
	extern UART_HandleTypeDef 	huart2;
	extern ringBuffer_t 		rBufferRxU2;
	extern uint8_t 				usart2WData;
	bool txComplete;
#endif
#if (USE_MAVLINK_BOOTLOADER == 1)
	extern serialPort_t 		serial_port2;
	extern UART_HandleTypeDef 	huart2;
#endif

#if (USE_MAVLINK_CONTROL == 1 && USE_HOST_BOOTLOADER == 0)
	extern serialPort_t 		serial_port2;
	extern UART_HandleTypeDef 	huart2;
#endif

#if (USE_COMMAND_LINE_INTERFACE == 1)
	extern UART_HandleTypeDef 	huart1;
	extern ringBuffer_t 		rBufferRxU1;
	extern uint8_t 				usart1WData;
	extern bool 				endCmd;
#endif

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

	#if (USE_DEVICE_BOOTLOADER == 1)
		if(huart->Instance == huart2.Instance)
		{
			txComplete = true;
		}
	#endif

	#if (USE_HOST_BOOTLOADER == 1)
		if(huart->Instance == huart2.Instance)
		{
			if(__hostBL()->isBootLoader == true)
			{
				txComplete = true;
			}
			else
			{
				serialPort_tx_finish(&serial_port2);
			}
		}
	#endif

	#if (USE_MAVLINK_BOOTLOADER == 1)
		if(huart->Instance == huart2.Instance)
		{
			serialPort_tx_finish(&serial_port2);
		}
	#endif
	#if (USE_MAVLINK_CONTROL == 1 && USE_HOST_BOOTLOADER == 0)
		if(huart->Instance == huart2.Instance)
		{
			serialPort_tx_finish(&serial_port2);
		}
	#endif
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
	#if (USE_DEVICE_BOOTLOADER == 1)
		if(huart->Instance == huart2.Instance)
		{
			ringBufferWrite(&rBufferRxU2, usart2WData);

		    /// xoa bo dem uart truoc khi truyen
		    __HAL_UART_FLUSH_DRREGISTER(&huart2);
		}
	#endif

	#if (USE_HOST_BOOTLOADER == 1)
		if(huart->Instance == huart2.Instance)
		{
			if(__hostBL()->isBootLoader == true)
			{
				ringBufferWrite(&rBufferHostBL, wData);
#if(HOST_USE_UART_IT == 1)
				if(HAL_UART_Receive_IT(&huart2, &wData, 1) != HAL_OK)
				{
					Error_Handler();
				}
#endif

			    /// xoa bo dem uart truoc khi truyen
			    __HAL_UART_FLUSH_DRREGISTER(&huart2);
			}
		}
	#endif

	#if (USE_MAVLINK_BOOTLOADER == 1)
		if(huart->Instance == huart2.Instance)
		{

		}
	#endif
	#if (USE_MAVLINK_CONTROL == 1 && USE_HOST_BOOTLOADER == 0)
		if(huart->Instance == huart2.Instance)
		{

		}
	#endif

	#if (USE_COMMAND_LINE_INTERFACE == 1)
		if(huart->Instance == huart1.Instance)
		{
			ringBufferWrite(&rBufferRxU1, usart1WData);

			if(usart1WData == '\n')
			{
				endCmd = true;
			}
		}
	#endif
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

/** @group __UART_ERROR_DEINIT
    @{
*/#ifndef __UART_ERROR_DEINIT
#define __UART_ERROR_DEINIT
/** @brief
    @return
*/


#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

