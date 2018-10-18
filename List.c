#include "List.h"
#include "CommonDefine.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#define LIST_MAX_BUFF_CNT 10
static ListItem* List_FindListItem(List* pList, unsigned int nIndex)
{
	ListItem* pListItem = pList->pHead;
	while(pListItem != NULL)
	{
		if(nIndex == 0)
		{
			break;
		}
		pListItem = pListItem->pNext;
		nIndex--;
	}
	if(nIndex == 0)
	{
		return pListItem;
	}
	else
	{
		return NULL;
	}
}
static ListItem* CreateListItem(List* pList, void* pData)
{
	ListItem *pListItem = NULL;
	if(pList->bufferCnt > 0)
	{
		pListItem = pList->pBufferHead;
		pList->pBufferHead = pListItem->pNext;
		pList->bufferCnt--;
	}else
	{
		pListItem = (ListItem*)malloc(sizeof(ListItem));
		if(pListItem == NULL)
		{
			return NULL;
		}
		memset(pListItem, 0, sizeof(ListItem));
		pListItem->pItem = malloc(pList->itemLen);
		if(pListItem->pItem == NULL)
		{
			free(pListItem);
			pListItem = NULL;
			return NULL;
		}
		memset(pListItem->pItem, 0, pList->itemLen);
	}
	pListItem->pNext = NULL;
	memcpy(pListItem->pItem, pData, pList->itemLen);
	return pListItem;
}

List* List_Create(int nItemLen)
{
	List* pList = (List*)malloc(sizeof(List));
	if(pList != NULL)
	{
		memset(pList,0,sizeof(List));
		pList->count = 0;
		pList->itemLen = nItemLen;
		pList->pHead = NULL;
		pList->pBufferHead = NULL;
		pList->bufferCnt = 0;
	}
	return pList;
}

static void List_DestroyListItem(ListItem* pListItem)
{
	ListItem* pTmpItem;
	while(pListItem != NULL)
	{
		pTmpItem = pListItem;
		pListItem = pTmpItem->pNext;
		free(pTmpItem->pItem);
		pTmpItem->pItem = NULL;
		free(pTmpItem);
		pTmpItem = NULL;
	}
}
int List_Destroy(List* pList)
{
	if(pList == NULL)
	{
		return RET_BAD_PARA;
	}
	List_DestroyListItem(pList->pHead);
	List_DestroyListItem(pList->pBufferHead);
	free(pList);
	pList = NULL;
	return RET_FUNCTION_OK;
}

int List_Add(List* pList, void* pItem)
{
	ListItem* pLastListItem;
	ListItem* pListItem;
	int res = RET_FUNCTION_OK;
	if(pItem == NULL)
	{
		return RET_BAD_PARA;
	}
	pLastListItem = List_FindListItem(pList, pList->count - 1);
	pListItem = CreateListItem(pList, pItem);
	if(pListItem == NULL)
	{
		return RET_USER_DEFINE - 1;
	}
	if(pLastListItem == NULL)
	{
		pList->pHead = pListItem;
	}
	else
	{
		pLastListItem->pNext = pListItem;
	}
	pList->count++;
	return res;
}

void* List_Find(List* pList, int nIndex)
{
	ListItem* pListItem;
	if(pList == NULL)
	{
		return NULL;
	}
	pListItem = List_FindListItem(pList, nIndex);
	if(pListItem == NULL)
	{
		return NULL;
	}
	return pListItem->pItem;
}

int List_Update(List *pList, int nIndex, void *pItem)
{
	void *pDest;
	if(pList == NULL || pItem == NULL)
	{
		return RET_BAD_PARA;
	}
	memcpy(List_Find(pList, nIndex), pItem, pList->itemLen);
	return RET_FUNCTION_OK;
}

void* List_FindCopy(List* pList, int nIndex)
{
	ListItem* pListItem;
	void* pData;
	if(pList == NULL)
	{
		return NULL;
	}
	pListItem = List_FindListItem(pList, nIndex);
	if(pListItem == NULL)
	{
		return NULL;
	}
	pData = (void *)malloc(pList->itemLen);
	if(pData != NULL)
	{
		memset(pData, 0, pList->itemLen);
		memcpy(pData, pListItem->pItem, pList->itemLen);
	}
	return pData;
}

int List_Remove(List* pList, unsigned int nIndex)
{
	ListItem* pListItem;
	ListItem* pPrevItem;
	ListItem* pNextItem;
	if(pList == NULL || nIndex >= pList->count)
	{
		return RET_BAD_PARA;
	}
	pListItem = List_FindListItem(pList, nIndex);
	pPrevItem = List_FindListItem(pList, nIndex - 1);
	pNextItem = List_FindListItem(pList, nIndex + 1);
	if(pPrevItem == NULL)
	{
		pList->pHead = pNextItem;
	}
	else
	{
		pPrevItem->pNext = pNextItem;
	}
	if(pList->bufferCnt < LIST_MAX_BUFF_CNT)
	{
		ListItem *pTmpListItem = pList->pBufferHead;
		pList->pBufferHead = pListItem;
		pList->pBufferHead->pNext = pTmpListItem;
		pList->bufferCnt++;
	}
	else
	{
		free(pListItem->pItem);
		pListItem->pItem = NULL;
		free(pListItem);
		pListItem = NULL;
		if(pPrevItem != NULL)
		{
			pPrevItem->pNext = pPrevItem;
		}
		else
		{
			pList->pHead = pNextItem; 
		}
		
	}
	pList->count--;
	return RET_FUNCTION_OK;
}

int List_Iterator(List* pList, IteratorHandler handler)
{
	ListItem * pListItem;
	ListItem *pNextItem;
	if (pList == NULL || handler == NULL)
	{
		return RET_BAD_PARA;
	}
	if (pList->count > 0)
	{
		pListItem = pList->pHead;
		do
		{
			pNextItem = pListItem->pNext;
			(handler)(pListItem->pItem);
			pListItem = pNextItem;
		} while (pListItem != NULL);
	}
	return RET_FUNCTION_OK;
}

int List_Count(List* pList)
{
	if (pList == NULL)
	{
		return 0;
	}	
	else
	{
		return pList->count;
	}
}
