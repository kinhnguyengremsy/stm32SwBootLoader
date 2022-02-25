/**
  ******************************************************************************
  * @file    .c
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
#include "mavlinkControl.h"
#include "mavlinkMsgHandle.h"
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
bool sendMsgJumTarget = false;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __MAVLINK_CONTROL_CONFIGURATION
    @{
*/#ifndef __MAVLINK_CONTROL_CONFIGURATION
#define __MAVLINK_CONTROL_CONFIGURATION
/** @brief  mavlinhControlConfiguration
    @return none
*/
void mavlinhControlConfiguration(void)
{
	mavlinkMsg_configuration();
}

#endif
/**
    @}
*/

/** @group __MAVLINK_CONTROL_FUNCTION
    @{
*/#ifndef __MAVLINK_CONTROL_FUNCTION
#define __MAVLINK_CONTROL_FUNCTION
/** @brief  mavlinkControlSendHeartBeart
 *  @param[in] channel
    @return none
*/
static void mavlinkControlSendHeartBeart(uint8_t channel)
{
	mavlinkMsg_send_heartbeat(channel);
}

/** @brief  mavlinkControl_sendCmdJumTaget
 *  @param[in] channel
    @return none
*/
static mavlinkControlSendCmdJumTaget(uint8_t channel)
{
	mavlinkMsg_send_cmdJumTarget(channel);
}

#endif
/**
    @}
*/

/** @group __MAVLINK_CONTROL_PROCESS
    @{
*/#ifndef __MAVLINK_CONTROL_PROCESS
#define __MAVLINK_CONTROL_PROCESS
/** @brief  mavlinkControl_process
    @return none
*/
void mavlinkControl_process(void)
{
	static uint32_t timeSendHeartBeat = 0;

	if(HAL_GetTick() - timeSendHeartBeat > 500)
	{
		timeSendHeartBeat = HAL_GetTick();

		mavlinkControlSendHeartBeart(1);
	}

	if(sendMsgJumTarget == true)
	{
		sendMsgJumTarget = false;

		mavlinkControlSendCmdJumTaget(1);
	}
}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/
