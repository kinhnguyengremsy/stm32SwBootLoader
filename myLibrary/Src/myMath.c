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
  * All rights reterved.Firmware coding style V1.0.beta
  *
  * The information contained herein is confidential
  * property of Company. The use, copying, transfer or
  * disclosure of such information is prohibited except
  * by exprets written agreement with Company.
  *
  ******************************************************************************
*/
/* Includes------------------------------------------------------------------------------*/
#include "myMath.h"
/* Private typedef------------------------------------------------------------------------------*/
/* Private define------------------------------------------------------------------------------*/
#define IS_CAP_LETTER(c)    (((c) >= 'A') && ((c) <= 'F'))
#define IS_LC_LETTER(c)     (((c) >= 'a') && ((c) <= 'f'))
#define IS_09(c)            (((c) >= '0') && ((c) <= '9'))
#define ISVALIDHEX(c)       (IS_CAP_LETTER(c) || IS_LC_LETTER(c) || IS_09(c))
#define ISVALIDDEC(c)       IS_09(c)
#define CONVERTDEC(c)       (c - '0')

#define CONVERTHEX_ALPHA(c) (IS_CAP_LETTER(c) ? ((c) - 'A'+10) : ((c) - 'a'+10))
#define CONVERTHEX(c)       (IS_09(c) ? ((c) - '0') : CONVERTHEX_ALPHA(c))
/* Private macro------------------------------------------------------------------------------*/
/* Private variables------------------------------------------------------------------------------*/
/* Private function prototypes------------------------------------------------------------------------------*/
/* Private functions------------------------------------------------------------------------------*/

/** @group __STRING_CONVERTER_FUNCTION
    @{
*/#ifndef __STRING_CONVERTER_FUNCTION
#define __STRING_CONVERTER_FUNCTION
/** @brief		Convert an integer to a string
 *  @param[in]	pStr : The string output pointer
 *  @param[in]	intNumber : The integer to be converted
	@return		None
	@copyright to dji
*/
void integerToString(uint8_t *pStr, uint32_t intNumber)
{
	uint32_t i = 0;
	uint32_t divider = 1000000000;
	uint32_t pos = 0;
	uint32_t status = 0;

	for(i = 0; i < 10; i++)
	{
		pStr[pos++] = (intNumber / divider) + 48;

		intNumber %= divider;
		divider /= 10;

		if((pStr[pos - 1] == '0') & (status == 0))
		{
			pos = 0;
		}
		else
		{
			status++;
		}
	}
}

/** @brief		Convert a string to an integer
 *  @param[in]	pInputStr : The string to be converted
 *  @param[in]	pIntegerNumber : The integer value
	@return		true : Corret
				false : Error
	@copyright to dji
*/
bool stringToInteger(uint8_t *pInputStr, uint32_t *pIntegerNumber)
{
	uint32_t i = 0;
	bool ret = false;
	uint32_t val = 0;

	if((pInputStr[0] == '0') && ((pInputStr[1] == 'x') || (pInputStr[1] == 'X')))
	{
		i = 2;

		while((i < 11) && (pInputStr[i] != '\0'))
		{
			if(ISVALIDHEX(pInputStr[i]))
			{
				val = (val << 4) + CONVERTHEX(pInputStr[i]);
			}
			else
			{
				/// return 0, Invalid input
				ret = false;
				break;
			}
		}

		/// valid retult
		if(pInputStr[i] == '\0')
		{
			*pIntegerNumber = val;
			ret = true;
		}
	}

	/* max 10-digit decimal input */
	else
	{
		while((i < 11) && (ret != true))
		{
			if(pInputStr[i] == '\0')
			{
				*pIntegerNumber = val;
				ret = true;
			}
			else if(((pInputStr[i] == 'k') || (pInputStr[i] == 'K')) && (i > 0))
			{
				val = val << 10;
				*pIntegerNumber = val;
				ret = true;
			}
			else if(((pInputStr[i] == 'm') || (pInputStr[i] == 'M')) && (i > 0))
			{
				val = val << 20;
				*pIntegerNumber = val;
				ret = true;
			}
			else if (ISVALIDDEC(pInputStr[i]))
			{
				val *= 10;
				val += CONVERTDEC(pInputStr[i]);
			}
			else
			{
				/// return 0, Invalid input
				ret = false;
				break;
			}
		}
	}

	return ret;
}


#endif
/**
    @}
*/

/************************ (C) COPYRIGHT GREMSY *****END OF FILE****************/

