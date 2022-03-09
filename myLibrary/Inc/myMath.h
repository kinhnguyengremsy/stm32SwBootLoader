/** 
  ******************************************************************************
  * @file    myMath.h
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

#ifndef __MY_MATH_H
#define __MY_MATH_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Exported define ------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/* Exported macro ------------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @brief		Convert an integer to a string
 *  @param[in]	pStr : The string output pointer
 *  @param[in]	intNumber : The integer to be converted
	@return		None
	@copyright to dji
*/
void integerToString(uint8_t *pStr, uint32_t intNumber);

/** @brief		Convert a string to an integer
 *  @param[in]	pInputStr : The string to be converted
 *  @param[in]	pIntegerNumber : The integer value
	@return		true : Corret
				false : Error
	@copyright to dji
*/
bool stringToInteger(uint8_t *pInputStr, uint32_t *pIntegerNumber);

#endif /* __MY_MATH_H */

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/


