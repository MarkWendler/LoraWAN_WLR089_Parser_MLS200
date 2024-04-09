/**
 * \file  parser_system.c
 *
 * \brief This file contains all the commands used for testing the system peripherals
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "parser_system.h"
#include "parser.h"
#include "parser_utils.h"
#include "parser_tsp.h"
#include "sys.h"
#include "sw_timer.h"
#include "conf_pmm.h"
#ifdef CONF_PMM_ENABLE
#include "pmm.h"
#endif
#if (ENABLE_PDS == 1)
#include "pds_interface.h"
#endif
#include "lorawan.h"
#include "radio_driver_hal.h"


#define STANDBY_STR_IDX        0U
#define BACKUP_STR_IDX         1U
#define OFF_STR_IDX            2U

static bool SleepEnabled = false;
#ifdef CONF_PMM_ENABLE
static const char* gapParserSysStatus[] = {
	"ok",
	"invalid_param",
	"err"
};

static const char* gapParseSleepMode[] = {
	"standby",
	"backup",
};
static void parserSleepCallback(uint32_t sleptDuration);
static void app_resources_uninit(void);

bool deviceResetsForWakeup = false;

#endif /* #ifdef CONF_PMM_ENABLE */

void Parser_SystemGetVer(parserCmdInfo_t* pParserCmdInfo)
{
	Parser_GetSwVersion(aParserData);
	pParserCmdInfo->pReplyCmd = aParserData;
}

void Parser_SystemReboot(parserCmdInfo_t* pParserCmdInfo)
{
	// Go for reboot, no reply necessary
	NVIC_SystemReset();
}

void Parser_SystemFactReset(parserCmdInfo_t* pParserCmdInfo)
{
	// Call PDS Delete All API to clear NVM Memory.
#if (ENABLE_PDS == 1)		
	PDS_DeleteAll();
#endif	
	// Go for reboot, no reply necessary
	NVIC_SystemReset();
}

void Parser_EicCallback(uintptr_t context)
{
    if (SleepEnabled)
    {
        SleepEnabled = false;
    }
#ifdef CONF_PMM_ENABLE
    PMM_Wakeup(0xFFFFFFFF, context);
#endif
}

#ifdef CONF_PMM_ENABLE

void Parser_SystemSleep(parserCmdInfo_t* pParserCmdInfo)
{
	/** Refer gapParseSleepMode[] array indices
	 *         [0] --> "standby"
	 *         [1] --> "backup"
	 */
	uint8_t sleepModeValue = 0xFF;
	PMM_SleepReq_t sleepRequest = {
		.sleep_mode = 0xFF,
		.sleepTimeMs = 0,
		.pmmWakeupCallback = NULL
	};
	uint32_t sleepDuration = strtoul(pParserCmdInfo->pParam2, NULL, 10);

	/* Parser parameter validation */
	for (uint8_t iCount = 0; iCount < sizeof(gapParseSleepMode) / sizeof(gapParseSleepMode[0]); iCount++) {
		if (0 == stricmp(pParserCmdInfo->pParam1, gapParseSleepMode[iCount])) {
			sleepModeValue = iCount;
			break;
		}
	}

	/* Sleep parameters validation */
	if ((1 /* invalid range */ < sleepModeValue) || \
         (PMM_SLEEPTIME_MIN_MS > sleepDuration) || \
         (PMM_SLEEPTIME_MAX_MS < sleepDuration)
		) {
		pParserCmdInfo->pReplyCmd = (char *) gapParserSysStatus[INVALID_PARAM_IDX];
		return;
	} else {
		sleepRequest.sleepTimeMs = sleepDuration;
		sleepRequest.pmmWakeupCallback = parserSleepCallback;
		sleepRequest.sleep_mode = (0 /* standby */ == sleepModeValue) ? SLEEP_MODE_STANDBY : SLEEP_MODE_BACKUP;
	}

	/* Sleep invocation */
	SleepEnabled = true;
	if (LORAWAN_ReadyToSleep(deviceResetsForWakeup)) {
		app_resources_uninit();
		if (PMM_SLEEP_REQ_DENIED == PMM_Sleep(&sleepRequest)) {
            PORT_Initialize();
            
            SERCOM4_SPI_Initialize();
            HAL_RadioInit();
            HAL_Radio_resources_init();
            
            SERCOM0_USART_Initialize();
            SERCOM0_USART_WriteCallbackRegister(Parser_WriteCallback, 0);
            SERCOM0_USART_ReadCallbackRegister(Parser_ReadCallback, 0);
            
			pParserCmdInfo->pReplyCmd = (char *) gapParserSysStatus[ERR_STATUS_IDX];
		}
	}
}
#endif /* #ifdef CONF_PMM_ENABLE */

#ifdef CONF_PMM_ENABLE
/*********************************************************************//**
\brief	Callback function of parser to power manager.
\param[in]	sleptDuration - duration for which sleep is done
*************************************************************************/

static void parserSleepCallback(uint32_t sleptDuration)
{
	char buffer[50];
	int n;

	PORT_Initialize();

	SERCOM4_SPI_Initialize();
	HAL_RadioInit();
	HAL_Radio_resources_init();

	SERCOM0_USART_Initialize();
	SERCOM0_USART_WriteCallbackRegister(Parser_WriteCallback, 0);
	SERCOM0_USART_ReadCallbackRegister(Parser_ReadCallback, 0);

	n = sprintf(buffer, "\nsleep_ok %ld ms\n\r", sleptDuration);
	SERCOM0_USART_Write(buffer, n);
}

static void app_resources_uninit(void)
{
	while (SERCOM0_USART_WriteIsBusy()) {
	}
	SERCOM0_USART_TransmitterDisable();
	SERCOM0_USART_ReceiverDisable();
	SERCOM0_REGS->USART_INT.SERCOM_CTRLA &= ~(SERCOM_USART_INT_CTRLA_ENABLE_Msk);
	while (SERCOM0_REGS->USART_INT.SERCOM_SYNCBUSY) {
	}
	/* Disable Transceiver SPI Module */
	HAL_RadioDeInit();
}

#endif /* #ifdef CONF_PMM_ENABLE */ 

/*EOF*/
