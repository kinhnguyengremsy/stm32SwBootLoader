/**
  ******************************************************************************
  * @file    mavlinkProtocol.c
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
#include "mavlinkProtocol.h"
#include "../mavlink_v2/mavlink_helpers.h"
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
mavlink_system_t    mavlink_system = {SYSID_ONBOARD, MAV_COMP_ID_SYSTEM_CONTROL};

serialPort_t serial_port2;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;

serialPort_t serial_port4;
extern UART_HandleTypeDef huart4;
extern DMA_HandleTypeDef hdma_uart4_rx;

/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group MAVLINK_PROTOCOL_CONFIGURATION
    @{
*/#ifndef MAVLINK_PROTOCOL_CONFIGURATION
#define MAVLINK_PROTOCOL_CONFIGURATION
/** @brief serialPort3_init
    @return none
*/
void mavlinkProtocol_serialPort3_init(void)
{
    /// init serialPort library
    serial_port2.zPrivate.uartHandle.hdmarx = &hdma_usart2_rx;
    serial_port2.zPrivate.uartHandle.Instance = USART2;
    serial_port2.isWriteFinish = true;

    /// init uart3 reciver hardware
    HAL_UART_Receive_DMA(&huart2, serial_port2.zPrivate.readBuffer, 256);
//    HAL_UART_Receive_IT(&huart3, &dataRx, 1);
}

/** @brief serialPort3_Deinit
    @return none
*/
void mavlinkProtocol_serialPort3_Deinit(void)
{
    /// Deinit serialPort library
    memset(&serial_port2, 0, sizeof(serialPort_t));
}

/** @brief serialPort4_init
    @return none
*/
void mavlinkProtocol_serialPort4_init(void)
{
    /// init serialPort library
    serial_port4.zPrivate.uartHandle.hdmarx = &hdma_uart4_rx;
    serial_port4.zPrivate.uartHandle.Instance = UART4;
    serial_port4.isWriteFinish = true;

    /// init uart4 reciver hardware
    HAL_UART_Receive_DMA(&huart4, serial_port4.zPrivate.readBuffer, 256);
}

/** @brief serialPort4_Deinit
    @return none
*/
void mavlinkProtocol_serialPort4_Deinit(void)
{
    /// Deinit serialPort library
    memset(&serial_port4, 0, sizeof(serialPort_t));
}

/**
 * @brief gs_init
 * The function shall initialize independence com channel
 * @param NONE
 * @return NONE
 */
void mavlinkProtocol_init(void)
{
    mavlinkProtocol_serialPort3_init();

    mavlinkProtocol_serialPort4_init();

    printf("mavlinkPinout : USART2 (tx : A2, rx : A3)  -> COM2\n");
    printf("              : UART4 (tx : C10, rx : C11) -> COM4\n");
}


#endif
/**
    @}
*/

/** @group MAVLINK_PROTOCOL_READ_DATA
	@{
*/#ifndef MAVLINK_PROTOCOL_READ_DATA
#define MAVLINK_PROTOCOL_READ_DATA
/**
 * @brief  This is function read data from gimbal
 * @return bool
 */
bool mavlinkProtocol_serialPort3_readData(mav_state_t *mav)
{
    bool ret = false;

    if(serialPort_read(&serial_port2, 256))
    {
        int i = 0;

        for(i = 0; i < serial_port2.bytesToRead; i++)
        {
            uint8_t msg_received;
            /* Paser data */
            msg_received = mavlink_parse_char(  MAVLINK_COMM_1,
            									serial_port2.readBuffer[i],
                                                &mav->rxmsg,
                                                &mav->status);
            /** 0 if no message could be decoded or bad CRC, 1 on good message and CRC*/
            if(msg_received)
            {
                ret = true;
            }
        }
    }

    return ret;
}

/**
 * @brief  This is function read data from gimbal
 * @return bool
 */
bool mavlinkProtocol_serialPort4_readData(mav_state_t *mav)
{
    bool ret = false;

    if(serialPort_read(&serial_port4, 256))
    {
        int i = 0;

        for(i = 0; i < serial_port4.bytesToRead; i++)
        {
            uint8_t msg_received;
            /* Paser data */
            msg_received = mavlink_parse_char(  MAVLINK_COMM_2,
            									serial_port4.readBuffer[i],
                                                &mav->rxmsg,
                                                &mav->status);
            /** 0 if no message could be decoded or bad CRC, 1 on good message and CRC*/
            if(msg_received)
            {
                ret = true;
            }
        }
    }

    return ret;
}



#endif
/**
	@}
*/

/** @group MAVLINK_PROTOCOL_SEND_DATA
	@{
*/#ifndef MAVLINK_PROTOCOL_SEND_DATA
#define MAVLINK_PROTOCOL_SEND_DATA
/** @brief send a buffer out a MAVLink channel
    @param[in] chan
    @param[in] buf
    @param[in] len
    @return none
*/
void comm_send_buffer(mavlink_channel_t chan, const uint8_t *buf, uint8_t len)
{
    if (!valid_channel(chan))
    {
        return;
    }
    if(chan == ONBOARD_CHANNEL)
    {
        /* Send data to serial port 2*/
        serialPort_write_list(&serial_port2, (uint8_t *)buf);
    }
    else if(chan == MAVLINK_COMM_2)
    {
        /* Send data to serial port 4 */
        serialPort_write_list(&serial_port4, (uint8_t *)buf);
    }
}



#endif
/**
	@}
*/

/** @group MAVLINK_PROTOCOL_SERIAL_TRANMITS_INTERRUPT_CALLBACK
	@{
*/#ifndef MAVLINK_PROTOCOL_SERIAL_TRANMITS_INTERRUPT_CALLBACK
#define MAVLINK_PROTOCOL_SERIAL_TRANMITS_INTERRUPT_CALLBACK
///**
//  * @brief  Tx Transfer completed callbacks.
//  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
//  *                the configuration information for the specified UART module.
//  * @retval None
//  */
//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
//{
//    if(serial_port2.zPrivate.uartHandle.Instance == USART2)
//    {
//        serialPort_tx_finish(&serial_port2);
//    }
//
//    if(serial_port4.zPrivate.uartHandle.Instance == UART4)
//    {
//        serialPort_tx_finish(&serial_port4);
//    }
//}
//
///**
//  * @brief  Rx Transfer completed callbacks.
//  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
//  *                the configuration information for the specified UART module.
//  * @retval None
//  */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	if(serial_port2.zPrivate.uartHandle.Instance == USART2)
//	{
//		/// use in dma mode nomal
////		HAL_UART_Receive_DMA(&huart3, serial_port2.zPrivate.readBuffer, 256);
////		HAL_UART_Receive_IT(&huart3, &dataRx, 1);
//	}
//}
//
///**
//  * @brief  UART error callbacks.
//  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
//  *                the configuration information for the specified UART module.
//  * @retval None
//  */
//void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
//{
//
//}

#endif
/**
	@}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


