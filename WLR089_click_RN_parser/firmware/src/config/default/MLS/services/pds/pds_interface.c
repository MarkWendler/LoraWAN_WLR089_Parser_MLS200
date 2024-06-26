/**
* \file  pds_interface.c
*
* \brief     This is the PDS interface source file which has had implimentations for all public
*	API's.
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



/******************************************************************************
                   Includes section
******************************************************************************/
#include "pds_interface.h"
#include "pds_common.h"
#include "pds_task_handler.h"
#include "pds_wl.h"

/******************************************************************************
                   Global section
******************************************************************************/


/******************************************************************************
                   Static section
******************************************************************************/
#if (ENABLE_PDS == 1)	
bool isFileSet[PDS_MAX_FILE_IDX];
static bool pdsUnInitFlag = false;
#endif
PdsFileMarks_t fileMarks[PDS_MAX_FILE_IDX];


/******************************************************************************
                   Implementations section
******************************************************************************/

/**************************************************************************//**
\brief Initializes the PDS.

\param[in] none
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_Init(void)
{
#if (ENABLE_PDS == 1)	
	PdsStatus_t status = pdsWlInit();
	pdsUnInitFlag = false;
	return status;
#else
	return PDS_OK;
#endif
}

/**************************************************************************//**
\brief Disables storing to PDS.

\param[in] none
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_UnInit(void)
{
#if (ENABLE_PDS == 1)
	pdsUnInitFlag = true;
#endif
	return PDS_OK;
}

/**************************************************************************//**
\brief 	This function will set the store operation bit in the file marks for the
		item in PDS

\param[in] pdsFileItemIdx - The file id to register file to PDS.
\param[in] item - The item id of the item in PDS.
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_Store(PdsFileItemIdx_t pdsFileItemIdx, uint8_t item)
{
	PdsStatus_t status = PDS_OK;
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		if ((0 != fileMarks[pdsFileItemIdx].numItems) && 				\
				(0 != fileMarks[pdsFileItemIdx].fileMarkListAddr) &&	\
				(0 != fileMarks[pdsFileItemIdx].itemListAddr)			\
			   )
		{
			if (PDS_MAX_FILE_IDX > pdsFileItemIdx)
			{
				*((fileMarks[pdsFileItemIdx].fileMarkListAddr) + item) = PDS_OP_STORE;
				isFileSet[pdsFileItemIdx] = true;
				pdsPostTask(PDS_STORE_DELETE_TASK_ID);
			}
			else
			{
				status = PDS_INVLIAD_FILE_IDX;
			}
		}
		else
		{
			status = PDS_INVLIAD_FILE_IDX;
		}
	}
#endif	
	return status; 
}

/**************************************************************************//**
\brief This function will restore an item from PDS to RAM.

\param[in] pdsFileItemIdx - The file id to register file to PDS.
\param[in] item - The item id of the item in PDS.
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_Restore(PdsFileItemIdx_t pdsFileItemIdx, uint8_t item)
{
	PdsStatus_t status = PDS_OK;
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		PdsMem_t buffer;
		uint8_t *ptr;
		ItemMap_t itemInfo;
		ItemHeader_t itemHeader;
		uint16_t size;
		
		if ((0 != fileMarks[pdsFileItemIdx].numItems) && 				\
		(0 != fileMarks[pdsFileItemIdx].fileMarkListAddr) &&	\
		(0 != fileMarks[pdsFileItemIdx].itemListAddr)			\
		)
		{
			memset(&buffer, 0, sizeof(PdsMem_t));
			memcpy((void *)&itemInfo, (void *)(fileMarks[pdsFileItemIdx].itemListAddr + (fileMarks[pdsFileItemIdx].numItems - 1)), sizeof(ItemMap_t));
			size = itemInfo.itemOffset + itemInfo.size + sizeof(ItemHeader_t);
			status = pdsWlRead(pdsFileItemIdx, &buffer, size);
			if (status != PDS_OK)
			{
				return status;
			}
			
			memcpy((void *)&itemInfo, (void *)(fileMarks[pdsFileItemIdx].itemListAddr + item), sizeof(ItemMap_t));
			ptr = ((uint8_t *)&(buffer.NVM_Struct.pdsNvmData.WL_Struct.pdsWlData));
			ptr += itemInfo.itemOffset;
			
			if (item == itemInfo.itemId)
			{
				memcpy((void *)(&itemHeader), (void *)(ptr), sizeof(ItemHeader_t));
				if (false == itemHeader.delete)
				{
					ptr += sizeof(ItemHeader_t);
					memcpy((void *)(itemInfo.ramAddress), (void *)(ptr), itemInfo.size);
				}
				else
				{
					return PDS_ITEM_DELETED;
				}
			}
			else
			{
				return PDS_NOT_FOUND;
			}
			
			return status;
		}
		else
		{
			status = PDS_INVLIAD_FILE_IDX;
		}
	}
#endif	
	return status;
}

/**************************************************************************//**
\brief	This function will set the delete operation for the item
		in the file mask.

\param[in] pdsFileItemIdx - The file id to register file to PDS.
\param[in] item - The item id of the item in PDS.
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_Delete(PdsFileItemIdx_t pdsFileItemIdx, uint8_t item)
{
	PdsStatus_t status = PDS_OK;
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		if ((0 != fileMarks[pdsFileItemIdx].numItems) && 			\
		(0 != fileMarks[pdsFileItemIdx].fileMarkListAddr) &&	\
		(0 != fileMarks[pdsFileItemIdx].itemListAddr)			\
		)
		{
			if (PDS_MAX_FILE_IDX > pdsFileItemIdx)
			{
				*((fileMarks[pdsFileItemIdx].fileMarkListAddr) + item) = PDS_OP_DELETE;
				isFileSet[pdsFileItemIdx] = true;
				pdsPostTask(PDS_STORE_DELETE_TASK_ID);
			}
			else
			{
				status = PDS_INVLIAD_FILE_IDX;
			}
		}
		else
		{
			status = PDS_INVLIAD_FILE_IDX;
		}
	}
#endif	
	return status; 
}



/**************************************************************************//**
\brief	This function checks if all the registered files are restorable.

\param[out] status - The return status of the function's operation.
******************************************************************************/
bool PDS_IsRestorable(void)
{
	bool return_status = false;
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		for (uint8_t pdsFileItemIdx = 0; pdsFileItemIdx < PDS_MAX_FILE_IDX; pdsFileItemIdx++)
		{
			if ((0 != fileMarks[pdsFileItemIdx].numItems) && 			\
			(0 != fileMarks[pdsFileItemIdx].fileMarkListAddr) &&	\
			(0 != fileMarks[pdsFileItemIdx].itemListAddr)			\
			)
			{
				if ( !(isFileFound(pdsFileItemIdx)) )
				{
					return return_status;
				}
			}
		}
	}
	return_status = true;
#endif
	return return_status;
}

/**************************************************************************//**
\brief This function will erase all the items stored in the PDS.

\param[out] status - The return status of the function's operation.
******************************************************************************/
PdsStatus_t PDS_DeleteAll(void)
{
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		pdsWlDeleteAll();
	}
#endif
	return PDS_OK;
}

/**************************************************************************//**
\brief 	This function will restore all the items from the PDS to RAM
		from all registered files.

\param[out] status - The return status of the function's operation.
******************************************************************************/
PdsStatus_t PDS_RestoreAll(void)
{
	PdsStatus_t status = PDS_OK;
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		uint8_t *ptr;
		ItemMap_t itemInfo;
		ItemHeader_t itemHeader;
		PdsMem_t buffer;
		uint16_t size;
		
		for (uint8_t pdsFileItemIdx = 0; pdsFileItemIdx < PDS_MAX_FILE_IDX; pdsFileItemIdx++)
		{
			if ((0 != fileMarks[pdsFileItemIdx].numItems) && 			\
			(0 != fileMarks[pdsFileItemIdx].fileMarkListAddr) &&	\
			(0 != fileMarks[pdsFileItemIdx].itemListAddr)			\
			)
			{
				memset(&buffer, 0, sizeof(PdsMem_t));
				memcpy((void *)&itemInfo, (void *)(fileMarks[pdsFileItemIdx].itemListAddr + (fileMarks[pdsFileItemIdx].numItems - 1)), sizeof(ItemMap_t));
				size = itemInfo.itemOffset + itemInfo.size + sizeof(ItemHeader_t);
				status = pdsWlRead(pdsFileItemIdx, &buffer, size);
				if (status != PDS_OK)
				{
					return status;
				}
				
				for (uint8_t itemIdx = 0; itemIdx < fileMarks[pdsFileItemIdx].numItems; itemIdx++)
				{
					memcpy((void *)&itemInfo, (void *)(fileMarks[pdsFileItemIdx].itemListAddr + itemIdx), sizeof(ItemMap_t));
					ptr = (uint8_t *)(&(buffer.NVM_Struct.pdsNvmData.WL_Struct.pdsWlData));
					ptr += itemInfo.itemOffset;
					memcpy((void *)(&itemHeader), (void *)(ptr), sizeof(ItemHeader_t));
					ptr += sizeof(ItemHeader_t);
					if (false == itemHeader.delete)
					{
						memcpy((void *)(itemInfo.ramAddress), (void *)(ptr), itemHeader.size);
					}
				}
				if(fileMarks[pdsFileItemIdx].fIDcb != NULL)
				{
					fileMarks[pdsFileItemIdx].fIDcb();
				}
			}
		}
	}
#endif	
	return status;
}

/**************************************************************************//**
\brief	This function will set the store operation to all the items stored 
		in all the registered files in PDS.

\param[in] none
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_StoreAll(void)
{
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		for (uint8_t pdsFileItemIdx = 0; pdsFileItemIdx < PDS_MAX_FILE_IDX; pdsFileItemIdx++)
		{
			if ((0 != fileMarks[pdsFileItemIdx].numItems) && 			\
			(0 != fileMarks[pdsFileItemIdx].fileMarkListAddr) &&	\
			(0 != fileMarks[pdsFileItemIdx].itemListAddr)			\
			)
			{
				for (uint8_t itemIdx = 0; itemIdx < fileMarks[pdsFileItemIdx].numItems; itemIdx++)
				{
					*(fileMarks[pdsFileItemIdx].fileMarkListAddr + itemIdx) = PDS_OP_STORE;
				}
				isFileSet[pdsFileItemIdx] = true;
			}
		}
		pdsPostTask(PDS_STORE_DELETE_TASK_ID);
	}
#endif	
	return PDS_OK;
}

/**************************************************************************//**
\brief This function registers a file to the PDS.

\param[in] argFileId - The file id to register file to PDS.
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_RegFile(PdsFileItemIdx_t argFileId, PdsFileMarks_t argFileMarks)
{
	PdsStatus_t status = PDS_OK;
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		if (PDS_MAX_FILE_IDX > argFileId)
		{
			memcpy(&fileMarks[argFileId], &argFileMarks, sizeof(PdsFileMarks_t));
		}
		else
		{
			status = PDS_INVLIAD_FILE_IDX;
		}
	}
#endif	
	return status;
}

/**************************************************************************//**
\brief This function registers a file to the PDS.

\param[in] argFileId - The file id to register file to PDS.
\param[out] status - The return status of the function's operation of type PdsStatus_t.
******************************************************************************/
PdsStatus_t PDS_UnRegFile(PdsFileItemIdx_t argFileId)
{
	PdsStatus_t status = PDS_OK;
#if (ENABLE_PDS == 1)
	if (false == pdsUnInitFlag)
	{
		if (PDS_MAX_FILE_IDX > argFileId)
		{
			memset(&fileMarks[argFileId], 0, sizeof(PdsFileMarks_t));
		}
		else
		{
			status = PDS_INVLIAD_FILE_IDX;
		}
	}
#endif
	return status;
}

/* eof pds_interface.c */