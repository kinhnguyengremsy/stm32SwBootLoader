/** 
  ******************************************************************************
  * @file    mavlinkMsgHandle.h
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

#ifndef __MAVLINKMSGHANDLE_H
#define __MAVLINKMSGHANDLE_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/
#define IS_RECIEVER		1
#define LOG_DATA_LEN	90
/* Exported types ------------------------------------------------------------*/

typedef enum
{
	TIMEOUT_STATE_IDLE,
	TIMEOUT_STATE_RUNNING,
	TIMEOUT_STATE_DONE,
	TIMEOUT_STATE_ERROR,

}timeOutState_t;

typedef struct _mavlink_msg_heartbeat_t
{
    uint32_t custom_mode; /*<  A bitfield for use for autopilot-specific flags*/
    uint8_t type; /*<  Type of the MAV (quadrotor, helicopter, etc.)*/
    uint8_t autopilot; /*<  Autopilot type / class.*/
    uint8_t base_mode; /*<  System mode bitmap.*/
    uint8_t system_status; /*<  System status flag.*/
    uint8_t mavlink_version; /*<  MAVLink version, not writable by user, gets added by protocol because of magic data type: uint8_t_mavlink_version*/

}mavlink_msg_heartbeat_t;

typedef struct _mavlink_log_request_data_t
{
	uint32_t ofs; /*<  Offset into the log*/
	uint32_t count; /*< [bytes] Number of bytes*/
	uint16_t id; /*<  Log id (from LOG_ENTRY reply)*/
	uint8_t target_system; /*<  System ID*/
	uint8_t target_component; /*<  Component ID*/

}mavlink_msg_log_request_data_t;

typedef struct _mavlink_msg_log_data_t
{
	uint32_t ofs; /*<  Offset into the log*/
	uint16_t id; /*<  Log id (from LOG_ENTRY reply)*/
	uint8_t count; /*< [bytes] Number of bytes (zero for end of log)*/
	uint8_t data[90]; /*<  log data*/

}mavlink_msg_log_data_t;

typedef struct _mavlink_log_request_end_t
{
	uint8_t target_system; /*<  System ID*/
	uint8_t target_component; /*<  Component ID*/

}mavlink_msg_log_request_end_t;

typedef struct __mavlinkMsg_t
{
    uint8_t         vehicle_system_id;
    uint8_t         vehicle_component_id;

	/// heart beat
	mavlink_msg_heartbeat_t heartBeat;

	/// log data
	mavlink_msg_log_data_t logData;

	/// log request data
	mavlink_msg_log_request_data_t logRequestData;

	/// log request end
	mavlink_msg_log_request_end_t logRequestEnd;

}mavlinkMsgHandle_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  mavlinkMsg_configuration
    @return none
*/
void mavlinkMsg_configuration(void);

/** @brief  mavlinkMsg_checkConnection
    @return none
*/
uint8_t mavlinkMsg_checkConnection(uint8_t channel);

/** @brief  mavlinkMsg_checkConnection
    @return none
*/
uint8_t mavlinkMsg_isLogRequestData(uint8_t channel);

/** @brief  mavlinkMsg_isLogData
    @return none
*/
uint8_t mavlinkMsg_isLogData(uint8_t channel);

/** @brief  mavlinkMsg_isLogRequestEnd
    @return none
*/
uint8_t mavlinkMsg_isLogRequestEnd(uint8_t channel);

/** @brief  mavlinkMsg_process
    @return none
*/
void mavlinkMsg_process(void);

/** @brief  mavlinkMsg_send_logData
    @return none
*/
void mavlinkMsg_send_logData(uint32_t offset, uint16_t id, uint8_t count, uint8_t *data, uint8_t channel);

/** @brief  mavlinkMsg_send_logRequestData
    @return none
*/
void mavlinkMsg_send_logRequestData(uint32_t offset, uint32_t count, uint16_t id, uint8_t channel);

/** @brief  mavlinkMsg_send_logRequestEnd
    @return none
*/
void mavlinkMsg_send_logRequestEnd(uint8_t channel);

#endif /* __MAVLINKMSGHANDLE_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


