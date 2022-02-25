/** 
  ******************************************************************************
  * @file    mavlinkControl.h
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

#ifndef __MAVLINK_CONTROL_H
#define __MAVLINK_CONTROL_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  mavlinhControlConfiguration
    @return none
*/
void mavlinhControlConfiguration(void);

/** @brief  mavlinkControl_process
    @return none
*/
void mavlinkControl_process(void);
#endif /* __MAVLINK_CONTROL_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


