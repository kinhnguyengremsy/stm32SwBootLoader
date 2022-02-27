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
#include "uartCLI.h"
#include "storageFlash.h"
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
bool sendMsgJumTarget = false;
uartCLI_t cli;

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

	storageFlash_configuration();

	uartCLI_configuration(&cli);
	memset(&cli, 0, sizeof(uartCLI_t));

	__cmdCli()->flagMsgHeartBeat = true;
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
static void mavlinkControlSendCmdJumTaget(uint8_t channel)
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
	static uint16_t storageFlashDummyValue = 0;
	static uint16_t storageFlashDummyReadValue = 0;
	static uint16_t storageFlashDummyReadAddress = 0;
	static uint16_t storageFlashDummyAddress = 0;

	uartCLI_process();

	mavlinkMsg_readData();

	if(HAL_GetTick() - timeSendHeartBeat > 500)
	{
		timeSendHeartBeat = HAL_GetTick();

		if(__cmdCli()->flagMsgHeartBeat == true)
		mavlinkControlSendHeartBeart(1);
	}

	if(__cmdCli()->flagMsgJumTarget == true)
	{
		__cmdCli()->flagMsgJumTarget = false;
		mavlinkControlSendCmdJumTaget(1);
	}

	if(__cmdCli()->flagStorageRead == true)
	{
		__cmdCli()->flagStorageRead = false;
		storageFlash_styleGremsy_read(__cmdCli()->value[7], &storageFlashDummyReadValue);
		printf("\n[storageFlash_test_read] address = 0x%x | value = %d\n", (int)__cmdCli()->value[7], (int)storageFlashDummyReadValue);
		storageFlashDummyReadAddress++;
		storageFlashDummyReadValue = 0;
	}

	if(__cmdCli()->flagStorageWrite == true)
	{
		__cmdCli()->flagStorageWrite = false;
		storageFlash_styleGremsy_write(__cmdCli()->value[5], __cmdCli()->value[6]);
		printf("\n[storageFlash_test_write] address = 0x%x | value = %d\n", (int)__cmdCli()->value[5], (int)__cmdCli()->value[6]);
		storageFlashDummyAddress++;
	}
}

#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

