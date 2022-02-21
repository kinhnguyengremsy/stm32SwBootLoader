/** 
  ******************************************************************************
  * @file    uartCallback.h
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

#ifndef __UART_CALLBACK_H
#define __UART_CALLBACK_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
typedef struct
{
	bool rxComplete;
	bool txComplete;

}uartCallback_t;
/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/


#endif /* __UART_CALLBACK_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


