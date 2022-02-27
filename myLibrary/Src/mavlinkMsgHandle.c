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
#include "mavlinkMsgHandle.h"
#include "mavlinkProtocol.h"
/* Private typedef------------------------------------------------------------------------------*/
typedef struct _mavlinkMsg_private_t
{
	bool enableSendHeartBeat[2];

	bool seenHeartBeatCOM2;
	bool seenHeartBeatCOM4;

	bool isMsg_logRequestDataCOM2;
	bool isMsg_logRequestDataCOM4;

	bool isMsg_logDataCOM2;
	bool isMsg_logDataCOM4;

	bool isMsg_logRequestEndCOM2;
	bool isMsg_logRequestEndCOM4;

}mavlinkMsg_private_t;
/* Private define------------------------------------------------------------------------------*/

/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/

mav_state_t mav_state_COM2;
mav_state_t mav_state_COM4;

mavlinkMsgHandle_t mavlinkCOM2;
mavlinkMsgHandle_t mavlinkCOM4;

mavlinkMsg_private_t mavlinkPrivate;
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __MAVLINK_MSG_CONFIGURATION
    @{
*/#ifndef __MAVLINK_MSG_CONFIGURATION
#define __MAVLINK_MSG_CONFIGURATION
/** @brief  mavlinkMsg_configuration
    @return none
*/
void mavlinkMsg_configuration(void)
{
	mavlinkProtocol_init();
}

#endif
/**
    @}
*/

/** @group __MAVLINK_MSG_HANDLE_FUNCTION
    @{
*/#ifndef __MAVLINK_MSG_HANDLE_FUNCTION
#define __MAVLINK_MSG_HANDLE_FUNCTION
/** @brief  mavlinkMsg_sendHeartBeat
    @return none
*/
static void mavlinkMsg_sendHeartBeat(mavlink_channel_t channel)
{
	mavlink_message_t 	msg;
	mavlink_heartbeat_t heartBeat;
	uint16_t 			len = 0;

	heartBeat.type          = MAV_TYPE_ONBOARD_TESTER;
	heartBeat.autopilot     = MAV_AUTOPILOT_INVALID;
	heartBeat.base_mode     = 0;
    heartBeat.custom_mode   = 0;
    heartBeat.system_status = MAV_STATE_ACTIVE;

    /*
      save and restore sequence number for chan, as it is used by
      generated encode functions
     */

    mavlink_status_t    *chan_status = mavlink_get_channel_status(channel);
    uint8_t saved_seq = chan_status->current_tx_seq;

    mavlink_msg_heartbeat_encode_chan(  1,
                                        MAV_COMP_ID_SYSTEM_CONTROL,
                                        channel,
                                        &msg,
                                        &heartBeat);

    chan_status->current_tx_seq = saved_seq;

    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    len = mavlink_msg_to_send_buffer(msgbuf, &msg);

    if(len > 0)
    {
        _mavlink_send_uart(channel,(const char*) msgbuf, len);
    }
}

/** @brief  mavlinkMsg_sendLogData
    @return none
*/
static void mavlinkMsg_sendLogData(mavlink_channel_t channel, mavlink_log_data_t logData)
{
	mavlink_message_t 	msg;
	mavlink_log_data_t 	log_data;
	uint16_t 			len = 0;

	log_data.count 	= logData.count;
	memcpy(log_data.data, logData.data, LOG_DATA_LEN);
	log_data.id 	= logData.id;
	log_data.ofs 	= logData.ofs;

    /*
      save and restore sequence number for chan, as it is used by
      generated encode functions
     */

    mavlink_status_t    *chan_status = mavlink_get_channel_status(channel);
    uint8_t saved_seq = chan_status->current_tx_seq;

    mavlink_msg_log_data_encode_chan(1, MAV_COMP_ID_SYSTEM_CONTROL, channel, &msg, &log_data);

    chan_status->current_tx_seq = saved_seq;

    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    len = mavlink_msg_to_send_buffer(msgbuf, &msg);

    if(len > 0)
    {
        _mavlink_send_uart(channel,(const char*) msgbuf, len);
    }
}

/** @brief  mavlinkMsg_sendLogRequestData
    @return none
*/
static void mavlinkMsg_sendLogRequestData(mavlink_channel_t channel, mavlink_log_request_data_t logRequestData)
{
	mavlink_message_t 			msg;
	mavlink_log_request_data_t 	log_request_data;
	uint16_t 					len = 0;

	log_request_data.count 				= logRequestData.count;
	log_request_data.id 				= logRequestData.id;
	log_request_data.ofs 				= logRequestData.ofs;
	log_request_data.target_component 	= logRequestData.target_component;
	log_request_data.target_system 		= logRequestData.target_system;

    /*
      save and restore sequence number for chan, as it is used by
      generated encode functions
     */

    mavlink_status_t    *chan_status = mavlink_get_channel_status(channel);
    uint8_t saved_seq = chan_status->current_tx_seq;

    mavlink_msg_log_request_data_encode_chan(1, MAV_COMP_ID_SYSTEM_CONTROL, channel, &msg, &log_request_data);

    chan_status->current_tx_seq = saved_seq;

    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    len = mavlink_msg_to_send_buffer(msgbuf, &msg);

    if(len > 0)
    {
        _mavlink_send_uart(channel,(const char*) msgbuf, len);
    }
}

/** @brief  mavlinkMsg_sendLogRequestData
    @return none
*/
static void mavlinkMsg_sendLogRequestEnd(mavlink_channel_t channel, mavlink_log_request_end_t logRequestEnd)
{
	mavlink_message_t 			msg;
	mavlink_log_request_end_t 	log_request_end;
	uint16_t 					len = 0;

	log_request_end.target_component 	= logRequestEnd.target_component;
	log_request_end.target_system 		= logRequestEnd.target_system;

    /*
      save and restore sequence number for chan, as it is used by
      generated encode functions
     */

    mavlink_status_t    *chan_status = mavlink_get_channel_status(channel);
    uint8_t saved_seq = chan_status->current_tx_seq;

    mavlink_msg_log_request_end_encode_chan(1, MAV_COMP_ID_SYSTEM_CONTROL, channel, &msg, &log_request_end);

    chan_status->current_tx_seq = saved_seq;

    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    len = mavlink_msg_to_send_buffer(msgbuf, &msg);

    if(len > 0)
    {
        _mavlink_send_uart(channel,(const char*) msgbuf, len);
    }
}

/** @brief  mavlinkMsg_sendLogRequestData
    @return none
*/
static void mavlinkMsg_sendCmdLong( float param1,
									float param2,
									float param3,
									float param4,
									float param5,
									float param6,
									float param7,
									uint16_t command,
									uint8_t target_system,
									uint8_t target_component,
									uint8_t confirmation,
									mavlink_channel_t channel)
{
	mavlink_message_t 			msg;
	mavlink_command_long_t 		command_long;
	uint16_t 					len = 0;

	command_long.command = command;
	command_long.confirmation = confirmation;
	command_long.param1 = param1;
	command_long.param2 = param2;
	command_long.param3 = param3;
	command_long.param4 = param4;
	command_long.param5 = param5;
	command_long.param6 = param6;
	command_long.param7 = param7;
	command_long.target_component = target_component;
	command_long.target_system = target_system;

    /*
      save and restore sequence number for chan, as it is used by
      generated encode functions
     */

    mavlink_status_t    *chan_status = mavlink_get_channel_status(channel);
    uint8_t saved_seq = chan_status->current_tx_seq;

    /// encode msg
    mavlink_msg_command_long_encode_chan(1, MAV_COMP_ID_SYSTEM_CONTROL, channel, &msg, &command_long);


    chan_status->current_tx_seq = saved_seq;

    uint8_t msgbuf[MAVLINK_MAX_PACKET_LEN];
    len = mavlink_msg_to_send_buffer(msgbuf, &msg);

    if(len > 0)
    {
        _mavlink_send_uart(channel,(const char*) msgbuf, len);
    }
}

/** @brief  mavlinkMsg_RecieverProcess
    @return none
*/
static void mavlinkMsg_RecieverProcess(mav_state_t *mavState, mavlinkMsgHandle_t *handle, mavlink_channel_t channel)
{
	uint32_t msgId = mavState->rxmsg.msgid;

	switch(msgId)
	{
		case MAVLINK_MSG_ID_HEARTBEAT :
		{
			mavlink_heartbeat_t heartbeat;
			mavlink_msg_heartbeat_decode(&mavState->rxmsg, &heartbeat);

			handle->vehicle_system_id = mavState->rxmsg.sysid;
			handle->vehicle_component_id = mavState->rxmsg.compid;

			memcpy(&handle->heartBeat, &heartbeat, sizeof(mavlink_heartbeat_t));

			(channel == MAVLINK_COMM_1) ? \
			(mavlinkPrivate.seenHeartBeatCOM2 = true, mavlinkPrivate.enableSendHeartBeat[0] = true) : \
			(mavlinkPrivate.seenHeartBeatCOM4 = true, mavlinkPrivate.enableSendHeartBeat[1] = true);

		}break;
		case MAVLINK_MSG_ID_LOG_DATA :
		{
			mavlink_log_data_t log_data;
			mavlink_msg_log_data_decode(&mavState->rxmsg, &log_data);

			memcpy(&handle->logData, &log_data, sizeof(mavlink_msg_log_data_t));

			(channel == MAVLINK_COMM_1) ? (mavlinkPrivate.isMsg_logDataCOM2 = true) : (mavlinkPrivate.isMsg_logDataCOM4 = true);

		}break;
		case MAVLINK_MSG_ID_LOG_REQUEST_DATA :
		{
			mavlink_log_request_data_t log_request_data;
			mavlink_msg_log_request_data_decode(&mavState->rxmsg, &log_request_data);

			memcpy(&handle->logRequestData, &log_request_data, sizeof(mavlink_msg_log_request_data_t));

			(channel == MAVLINK_COMM_1) ? (mavlinkPrivate.isMsg_logRequestDataCOM2 = true) : (mavlinkPrivate.isMsg_logRequestDataCOM4 = true);

		}break;
		case MAVLINK_MSG_ID_LOG_REQUEST_END :
		{
			mavlink_log_request_end_t log_request_end;
			mavlink_msg_log_request_end_decode(&mavState->rxmsg, &log_request_end);

			memcpy(&handle->logRequestEnd, &log_request_end, sizeof(mavlink_msg_log_request_end_t));

			(channel == MAVLINK_COMM_1) ? (mavlinkPrivate.isMsg_logRequestEndCOM2 = true) : (mavlinkPrivate.isMsg_logRequestEndCOM4 = true);
		}break;
#if (USE_MAVLINK_CONTROL == 1)
		case MAVLINK_MSG_ID_MOUNT_ORIENTATION:
		{
			mavlink_mount_orientation_t mount_orientation;
			mavlink_msg_mount_orientation_decode(&mavState->rxmsg, &mount_orientation);

			memcpy(&handle->mount_orientation, &mount_orientation, sizeof(mavlink_msg_mount_orientation_t));
		}break;
		case MAVLINK_MSG_ID_COMMAND_ACK:
		{
			mavlink_command_ack_t command_ack;

			memset(&command_ack, 0, sizeof(mavlink_command_ack_t));

			mavlink_msg_command_ack_decode(&mavState->rxmsg, &command_ack);

			memcpy(&handle->command_ack, &command_ack, sizeof(mavlink_msg_command_ack_t));

			if(command_ack.command == MAV_CMD_JUMP_TAG)
			{
				if(command_ack.result == MAV_RESULT_ACCEPTED)
				{
					printf("\n[MAV_CMD_JUMP_TAG] cmd accepted\n");
				}
			}
		}break;
#endif
		default :
		{

		}break;
	}
}

/** @brief  mavlinkMsg_readData
    @return none
*/
void mavlinkMsg_readData(void)
{
	if(mavlinkProtocol_serialPort3_readData(&mav_state_COM2) == true)
	{
//		mavlink_status_t *chanStatus = mavlink_get_channel_status(MAVLINK_COMM_1);

		if(1)//chanStatus->parse_state == MAVLINK_PARSE_STATE_IDLE)
		{
			mavlinkMsg_RecieverProcess(&mav_state_COM2, &mavlinkCOM2, MAVLINK_COMM_1);
		}
	}

//	if(mavlinkProtocol_serialPort4_readData(&mav_state_COM4) == true)
//	{
////		mavlink_status_t *chanStatus = mavlink_get_channel_status(MAVLINK_COMM_2);
//
//		if(1)//chanStatus->parse_state == MAVLINK_PARSE_STATE_IDLE)
//		{
//			mavlinkMsg_RecieverProcess(&mav_state_COM4, &mavlinkCOM4, MAVLINK_COMM_2);
//		}
//	}
}

#endif
/**
    @}
*/

/** @group __MAVLINK_MSG_SEND_RECIEVER_PROCESS
    @{
*/#ifndef __MAVLINK_MSG_SEND_RECIEVER_PROCESS
#define __MAVLINK_MSG_SEND_RECIEVER_PROCESS
/** @brief  mavlinkMsg_isRecievedId
    @return none
*/
static uint8_t mavlinkMsg_isRecievedId(bool flag)
{
	uint8_t ret = 0;

	(flag == true) ? (ret = 2) : (ret = 0);

	return ret;
}

/** @brief  mavlinkMsg_checkConnection
    @return none
*/
uint8_t mavlinkMsg_checkConnection(uint8_t channel)
{
	uint8_t ret = 0;
	static uint32_t timeOutMavlinkConnection = 0;
	static uint8_t count = 0;
	static bool connect[2];

	if(HAL_GetTick() - timeOutMavlinkConnection > 1000 || timeOutMavlinkConnection == 0)
	{
		timeOutMavlinkConnection = HAL_GetTick();

		if(++count > 10) count = 11;

		if(channel == MAVLINK_COMM_1)
		{
			ret = mavlinkMsg_isRecievedId(mavlinkPrivate.seenHeartBeatCOM2);
			(ret != 0) ? (connect[0] = true) : (connect[0] = false);
		}
		else
		{
			ret = mavlinkMsg_isRecievedId(mavlinkPrivate.seenHeartBeatCOM4);
			(ret != 0) ? (connect[1] = true) : (connect[1] = false);
		}

		if(connect[0] == true || connect[1] == true)
		{
			count = 0;
			connect[0] = connect[1] = 0;
			if(ret == 0)
			{
				printf("\n[mavlinkMsg_checkConnection]  com2 lost connection !!!\n");
			}
		}
		else
		{
			if(count > 10)
			{
				printf("\n[mavlinkMsg_checkConnection]  com2 not found heartbeat !!!\n");
			}
			else
			{
				printf("\n[mavlinkMsg_checkConnection] waitting com2 connection !!!\n");
			}
		}
	}

	return ret;
}

/** @brief  mavlinkMsg_isLogRequestData
    @return none
*/
uint8_t mavlinkMsg_isLogRequestData(uint8_t channel)
{
//	return ((channel == 1) ? (mavlinkMsg_isRecievedId(mavlinkPrivate.isMsg_logRequestDataCOM2)) : (mavlinkMsg_isRecievedId(mavlinkPrivate.isMsg_logRequestDataCOM4)));
	uint8_t value = 0;

	if(channel == 1)
	{
		(mavlinkPrivate.isMsg_logRequestDataCOM2 == true) ? (value = 2, mavlinkPrivate.isMsg_logRequestDataCOM2 = false) : ( value = 0);
	}
	else
	{
		(mavlinkPrivate.isMsg_logRequestDataCOM4 == true) ? (value = 4, mavlinkPrivate.isMsg_logRequestDataCOM4 = false) : ( value = 0);
	}

	return value;
}

/** @brief  mavlinkMsg_isLogData
    @return none
*/
uint8_t mavlinkMsg_isLogData(uint8_t channel)
{
//	return ((channel == 1) ? (mavlinkMsg_isRecievedId(mavlinkPrivate.isMsg_logDataCOM2)) : (mavlinkMsg_isRecievedId(mavlinkPrivate.isMsg_logDataCOM4)));
	uint8_t value = 0;

	if(channel == 1)
	{
		(mavlinkPrivate.isMsg_logDataCOM2 == true) ? (value = 2, mavlinkPrivate.isMsg_logDataCOM2 = false) : ( value = 0);
	}
	else
	{
		(mavlinkPrivate.isMsg_logDataCOM4 == true) ? (value = 4, mavlinkPrivate.isMsg_logDataCOM4 = false) : ( value = 0);
	}

	return value;
}

/** @brief  mavlinkMsg_isLogRequestEnd
    @return none
*/
uint8_t mavlinkMsg_isLogRequestEnd(uint8_t channel)
{
//	return ((channel == 1) ? (mavlinkMsg_isRecievedId(mavlinkPrivate.isMsg_logRequestEndCOM2)) : (mavlinkMsg_isRecievedId(mavlinkPrivate.isMsg_logRequestEndCOM4)));
	uint8_t value = 0;

	if(channel == 1)
	{
		(mavlinkPrivate.isMsg_logRequestEndCOM2 == true) ? (value = 2, mavlinkPrivate.isMsg_logRequestEndCOM2 = false) : ( value = 0);
	}
	else
	{
		(mavlinkPrivate.isMsg_logRequestEndCOM4 == true) ? (value = 4, mavlinkPrivate.isMsg_logRequestEndCOM4 = false) : ( value = 0);
	}

	return value;
}

/** @brief  mavlinkMsg_send_logData
    @return none
*/
void mavlinkMsg_send_logData(uint32_t offset, uint16_t id, uint8_t count, uint8_t *data, uint8_t channel)
{
	mavlink_log_data_t log_data;

	log_data.ofs = offset;
	log_data.id = id;
	log_data.count = count;
	memcpy(log_data.data, data, LOG_DATA_LEN);

	mavlinkMsg_sendLogData((mavlink_channel_t)channel, log_data);
	printf("\n[mavlinkMsg_send_logData] channel %d send msg successful\n", channel);
}

/** @brief  mavlinkMsg_send_logRequestData
    @return none
*/
void mavlinkMsg_send_logRequestData(uint32_t offset, uint32_t count, uint16_t id, uint8_t channel)
{
	mavlink_log_request_data_t log_request_data;

	log_request_data.count = count;
	log_request_data.id = id;
	log_request_data.ofs = offset;
	log_request_data.target_component = 100;
	log_request_data.target_system = 10;

	mavlinkMsg_sendLogRequestData((mavlink_channel_t)channel, log_request_data);
//	printf("\n[mavlinkMsg_send_logRequestData] channel %d send msg successful |\ncount = %d\nid = %d\nofs = %d\ntarget_component = %d\ntarget_system = %d\n"
//			, channel, (int)log_request_data.count, (int)log_request_data.id, (int)log_request_data.ofs, log_request_data.target_component, log_request_data.target_system);
}

/** @brief  mavlinkMsg_send_logRequestEnd
    @return none
*/
void mavlinkMsg_send_logRequestEnd(uint8_t channel)
{
	mavlink_log_request_end_t log_request_end;

	log_request_end.target_component = 100;
	log_request_end.target_system = 10;

	mavlinkMsg_sendLogRequestEnd(channel, log_request_end);
	printf("\n[mavlinkMsg_send_logRequestEnd] channel %d send msg successful | \ntarget_component = %d\ntarget_system%d\n"
			, channel, log_request_end.target_component, log_request_end.target_system);
}

/** @brief  mavlinkMsg_process
 *  @param[in] channel kenh gui msg
    @return none
*/
void mavlinkMsg_send_heartbeat(uint8_t channel)
{
	mavlinkMsg_sendHeartBeat((mavlink_channel_t)channel);
}

/** @brief  mavlinkMsg_send_cmdJumTarget
 *  @param[in] channel kenh gui msg
    @return none
*/
void mavlinkMsg_send_cmdJumTarget(uint8_t channel)
{
	mavlinkMsg_sendCmdLong(0, 0, 0, 0, 0, 0, 0, MAV_CMD_JUMP_TAG, 10, MAV_COMP_ID_GIMBAL, 0, (mavlink_channel_t)channel);
}

/** @brief  mavlinkMsg_process
    @return none
*/
void mavlinkMsg_process(void)
{
	static uint32_t timeSendHB = 0;

//	if(HAL_GetTick() - timeSendHB > 500)
//	{
//		timeSendHB = HAL_GetTick();
//
//		mavlinkMsg_sendHeartBeat(MAVLINK_COMM_2);
//	}


	/// read data
	mavlinkMsg_readData();

	/// send heartbeat <-> reciever heartbeat from app
	if(mavlinkPrivate.enableSendHeartBeat[0] == true)
	{
		mavlinkPrivate.enableSendHeartBeat[0] = false;

		mavlinkMsg_sendHeartBeat(MAVLINK_COMM_1);

		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
	}

//	if(mavlinkPrivate.enableSendHeartBeat[1] == true)
//	{
//		mavlinkPrivate.enableSendHeartBeat[1] = false;
//
//		mavlinkMsg_sendHeartBeat(MAVLINK_COMM_2);
//
//		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
//	}
}


#endif
/**
    @}
*/
/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

