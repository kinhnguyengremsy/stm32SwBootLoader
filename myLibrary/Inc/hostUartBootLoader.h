/** 
  ******************************************************************************
  * @file    hostUartBootLoader.h
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

#ifndef __HOST_UART_BOOTLOADER_H
#define __HOST_UART_BOOTLOADER_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief  hostUartBootLoaderConfiguration
    @return none
*/
void hostUartBootLoaderConfiguration(void);

/** @brief  hostUartBootLoaderProcess
    @return none
*/
void hostUartBootLoaderProcess(void);
#endif /* __HOST_UART_BOOTLOADER_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


