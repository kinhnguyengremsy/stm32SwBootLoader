/** 
  ******************************************************************************
  * @file    softEmulations.h
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

#ifndef __SOTF_EMULATIONS_H
#define __SOTF_EMULATIONS_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/
#define FILE_LENGTH	(0x26F8)
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint32_t *flashBuffer;
}sotfEmulations_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  sotfEmulations_configuration
    @return none
*/
void sotfEmulations_configuration(void);

/** @brief  softEmulations_process
    @return none
*/
void softEmulations_process(void);
#endif /* __SOTF_EMULATIONS_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


