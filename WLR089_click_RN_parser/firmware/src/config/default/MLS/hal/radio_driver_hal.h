/**
* \file  radio_driver_hal.h
*
* \brief This is the Radio Driver HAL source file which 
*        contains LoRa-specific Radio Driver Hardware Abstract Layer
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



#ifndef RADIO_DRIVER_HAL_H
#define	RADIO_DRIVER_HAL_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "definitions.h"

/***************************************** MACROS *****************************/
#define DIO0        0x01
#define DIO1        0x02
#define DIO2        0x04
#define DIO3        0x08
#define DIO4        0x10
#define DIO5        0x20

#define REG_FIFO_ADDRESS	  0
#define REG_WRITE_CMD        0x80

/***************************************** TYPES ******************************/
typedef void (*DioInterruptHandler_t)(void);

typedef enum _RFCtrl1
{
	RFO_LF = 0,
	RFO_HF = 1,
	PA_BOOST = 2
}RFCtrl1_t;

typedef enum _RFCtrl2
{
	RX = 0,
	TX = 1
}RFCtrl2_t;

/*********************************************************************//**
\brief	Possible Radio Clock sources supported.
*************************************************************************/
typedef enum _RadioClockSources_t
{
	TCXO              = 0,
	XTAL
}RadioClockSources_t;

/*********************************** Prototypes*********************************/

/** 
 * \brief This function is used to initialize the Radio Hardware
 * The SPI interface,DIO and reset pins are initialized by this api
 */
void HAL_RadioInit(void);
/**
 * \brief This function is used to deinitialize the Radio SPI
 */
void HAL_RadioDeInit(void);  
/**
 * \brief This function is used to initialize the Radio SPI after PMM wakeup
 */

void HAL_Radio_resources_init(void);
/** 
 * \brief This function resets the Radio hardware by pulling the reset pin low
 */
void RADIO_Reset(void);

/** 
 * \brief This function is used to write a byte of data to the radio register
 * \param[in] reg Radio register to be written
 * \param[in] value Value to be written into the radio register
 */
void RADIO_RegisterWrite(uint8_t reg, uint8_t value);

/** 
 * \brief This function is used to read a byte of data from the radio register
 * \param[in] reg Radio register to be read
 * \retval  Value read from the radio register
 */
uint8_t RADIO_RegisterRead(uint8_t reg);

/** 
 * \brief This function is used to  write a stream of data into the Radio Frame buffer
 * \param[in] FIFO offset to be written to
 * \param[in] buffer Pointer to the data to be written into the frame buffer
 * \param[in] bufferLen Length of the data to be written
 */
void RADIO_FrameWrite(uint8_t offset, uint8_t* buffer, uint8_t bufferLen);

/** 
 * \brief This function is used to  read a stream of data from the Radio Frame buffer
 * \param[in] FIFO offset to be read from
 * \param[in] buffer Pointer to the data where the data is read and stored
 * \param[in] bufferLen Length of the data to be read from the frame buffer
 */
void RADIO_FrameRead(uint8_t offset, uint8_t* buffer, uint8_t bufferLen);

/** 
 * \brief This function is used to enable DIO0 interrupt
 */
void HAL_EnableDIO0Interrupt(void);

/** 
 * \brief This function is used to disable DIO0 interrupt
 */
void HAL_DisbleDIO0Interrupt(void);

/** 
 * \brief This function is used to enable DIO1 interrupt
 */
void HAL_EnableDIO1Interrupt(void);

/** 
 * \brief This function is used to disable DIO1 interrupt
 */
void HAL_DisbleDIO1Interrupt(void);

/** 
 * \brief This function is used to enable DIO2 interrupt
 */
void HAL_EnableDIO2Interrupt(void);

/** 
 * \brief This function is used to disable DIO2 interrupt
 */
void HAL_DisbleDIO2Interrupt(void);

/** 
 * \brief This function is used to enable DIO3 interrupt
 */
void HAL_EnableDIO3Interrupt(void);

/** 
 * \brief This function is used to disable DIO3 interrupt
 */
void HAL_DisbleDIO3Interrupt(void);

/** 
 * \brief This function is used to enable DIO4 interrupt
 */
void HAL_EnableDIO4Interrupt(void);

/** 
 * \brief This function is used to disable DIO4 interrupt
 */
void HAL_DisbleDIO4Interrupt(void);

/** 
 * \brief This function is used to enable DIO5 interrupt
 */
void HAL_EnableDIO5Interrupt(void);

/** 
 * \brief This function is used to disable DIO5 interrupt
 */
void HAL_DisbleDIO5Interrupt(void);

/** 
 * \brief This function is used to read the status of  DIO0 pin
 */
uint8_t HAL_DIO0PinValue(void);

/** 
 * \brief This function is used to read the status of  DIO1 pin
 */
uint8_t HAL_DIO1PinValue(void);

/** 
 * \brief This function is used to read the status of  DIO2 pin
 */
uint8_t HAL_DIO2PinValue(void);

/** 
 * \brief This function is used to read the status of  DIO2 pin
 */
uint8_t HAL_DIO3PinValue(void);

/** 
 * \brief This function is used to read the status of  DIO4 pin
 */
uint8_t HAL_DIO4PinValue(void);

/** 
 * \brief This function is used to read the status of  DIO5 pin
 */
uint8_t HAL_DIO5PinValue(void);

/**
 * \brief This function sets the interrupt handler for given DIO interrupt
 *
 * \param[in] dioPin  - DIO pin
 * \param[in] handler - function to be called upon given DIO interrupt
 */
void HAL_RegisterDioInterruptHandler(uint8_t dioPin, DioInterruptHandler_t handler);

/**
 * \brief This function gets the clock source of Radio defined by user
 *
 * \param[in] None
 * \param[out] Type of clock source TCXO or XTAL
 */
RadioClockSources_t HAL_GetRadioClkSrc(void);

/**
 * \brief This function gets the time taken for the radio clock to stabilize
 *
 * \param[in] None
 * \param[out] Time value in ms
 */
uint8_t HAL_GetRadioClkStabilizationDelay(void);

/**
 * \brief This function Powering up the TCXO oscillator
 *
 * \param[in] None
 * \param[out] None
 */
void HAL_TCXOPowerOn(void);

/**
 * \brief This function Powering off the TCXO oscillator
 *
 * \param[in] None
 * \param[out] None
 */
void HAL_TCXOPowerOff(void);

/** 
 * \brief This function is used to get the interrupt status
 * The interrupt status is cleared after calling this function
 * \retval Returns the mask of received interrupts
 */
uint8_t INTERRUPT_GetDioStatus(void);

/** 
 * \brief This function is used to get the interrupt status
 * The interrupt status is not cleared after calling this function 
 * \retval Returns the mask of received interrupts
 */
uint8_t INTERRUPT_PeekDioStatus(void);

/**
 * \brief This function Enables RF Control pins
 *
 * \param[in] RFCtrl1
 * 				RFO_LF = 0
 *				RFO_HF = 1
 *				PA_BOOST = 2
 * \param[in] RFCtrl2
 *				RX = 0
 *				TX = 1 
 * \param[out] None
 */
void HAL_EnableRFCtrl(RFCtrl1_t RFCtrl1, RFCtrl2_t RFCtrl2);
/**
 * \brief This function Disables RF Control pins
 *
 * \param[in] RFCtrl1
 * 				RFO_LF = 0
 *				RFO_HF = 1
 *				PA_BOOST = 2
 * \param[in] RFCtrl2
 *				RX = 0
 *				TX = 1 
 * \param[out] None
 */
void HAL_DisableRFCtrl(RFCtrl1_t RFCtrl1, RFCtrl2_t RFCtrl2);

#ifdef	__cplusplus
}
#endif

#endif	/* RADIO_DRIVER_HAL_H */

/**
 End of File
*/