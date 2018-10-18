#ifndef __LIST_H__
#define __LIST_H__

typedef int IteratorHandler(void*);
typedef struct ListItem
{
	void* pItem;
	struct ListItem* pNext;
}ListItem;
typedef struct List 
{
	int itemLen;
	int count;
	ListItem* pHead;
	ListItem* pBufferHead;
	int bufferCnt;
}List;


/********************************* 
*****@function Create a list 
*****@para      nItemLen                     
*****@return    Return a poniter point to the created list
**********************************/
List* List_Create(int nItemLen);

/********************************* 
*****@function  Destroy the specified list
*****@para      pList           
*****@return    Return  RET_FUNCTION_OK if opreate sucessfully 
**********************************/
int List_Destroy(List* pList);

/********************************* 
*****@function  Add a item to the specified list 
*****@para      pList           
*****@para      pItem           
*****@return    Return  RET_FUNCTION_OK if opreate sucessfully 
**********************************/
int List_Add(List* pList, void* pItem);


/********************************* 
*****@function  Get the item in the specified list 
*****@para      pList           
*****@para      nIndex          
*****@return    Return a pointer point the item 
**********************************/
void* List_Find(List* pList, int nIndex);

/********************************* 
*****@function  Update the specified item in the list
*****@para      pList           
*****@para      nIndex          
*****@para      nItem          
*****@return    Return  RET_FUNCTION_OK if opreate sucessfully 
**********************************/
int List_Update(List *pList, int nIndex, void *pItem);

/********************************* 
*****@function  Get the item's copy in the specified list 
*****@para      pList          
*****@para      nIndex         
*****@return    Retrun a pointer point to the item's copy
**********************************/
void* List_FindCopy(List* pList, int nIndex);

/********************************* 
*****@function  Remove a item in the list
*****@para      pList           
*****@para      nIndex          
*****@return    Return  RET_FUNCTION_OK if opreate sucessfully 
**********************************/
int List_Remove(List* pList, unsigned int nIndex);

/*********************************
*****@function  Handle all items in the list
*****@para      pList           
*****@para      nIndex         
*****@return    
**********************************/
int List_Iterator(List* pList, IteratorHandler handler);

/*********************************
*****@function  Get the item's count in the list 
*****@para      pList           
*****@return    Return the item's count in the list
**********************************/
int List_Count(List* pList);
#endif
