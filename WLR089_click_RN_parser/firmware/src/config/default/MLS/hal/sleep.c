/**
* \file  sleep.c
*
* \brief This is the implementation for hardware sleep

*		
*
*/
/*******************************************************************************
Copyright (C) 2020-21 released Microchip Technology Inc. and its subsidiaries. 

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED AS IS WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR 
*******************************************************************************/


 
/************************************************************************/
/* Includes                                                             */
/************************************************************************/
#include "definitions.h"
#include "sleep.h"

#ifdef CONF_PMM_ENABLE

/************************************************************************/
/* Function definitions                                                 */
/************************************************************************/

/**
 * \brief Puts the system in given sleep mode
 *
 * \param[in] mode - sleep mode
 */
void HAL_Sleep(HAL_SleepMode_t mode)
{
	switch (mode)
	{
		case SLEEP_MODE_STANDBY:
		{
            PM_StandbyModeEnter();
			break;
		}
		case SLEEP_MODE_BACKUP:
		{
            PM_BackupModeEnter();
			break;
		}
		
		default:
		{
			/* other sleep modes are not implemented currently */
			break;
		}
	}
}

#endif /* #ifdef CONF_PMM_ENABLE */

/* eof sleep.c */
