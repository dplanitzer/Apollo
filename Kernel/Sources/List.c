//
//  List.c
//  Apollo
//
//  Created by Dietmar Planitzer on 2/17/21.
//  Copyright © 2021 Dietmar Planitzer. All rights reserved.
//

#include "List.h"


void List_InsertBeforeFirst(List* _Nonnull pList, ListNode* _Nonnull pNode)
{
    pNode->prev = NULL;
    pNode->next = pList->first;
    
    if (pList->first) {
        pList->first->prev = pNode;
    }
    
    pList->first = pNode;
    if (pList->last == NULL) {
        pList->last = pNode;
    }
}

void List_InsertAfterLast(List* _Nonnull pList, ListNode* _Nonnull pNode)
{
    pNode->prev = pList->last;
    pNode->next = NULL;
    
    if (pList->last) {
        pList->last->next = pNode;
    }
    
    pList->last = pNode;
    if (pList->first == NULL) {
        pList->first = pNode;
    }
}

// Inserts the node 'pNode' after 'pAfterNode'. The node 'pNode' is added as the
// first node in the list if 'pAfterNode' is NULL.
void List_InsertAfter(List* _Nonnull pList, ListNode* _Nonnull pNode, ListNode* _Nullable pAfterNode)
{
    if (pAfterNode) {
        pNode->prev = pAfterNode;
        pNode->next = pAfterNode->next;
    
        if (pAfterNode->next) {
            pAfterNode->next->prev = pNode;
        }
        pAfterNode->next = pNode;
    
        if (pList->last == pAfterNode) {
            pList->last = pNode;
        }
    } else {
        List_InsertBeforeFirst(pList, pNode);
    }
}

void List_Remove(List* _Nonnull pList, ListNode* _Nonnull pNode)
{
    if (pList->first != pNode || pList->last != pNode) {
        if (pNode->next) {
            pNode->next->prev = pNode->prev;
        }
        if (pNode->prev) {
            pNode->prev->next = pNode->next;
        }
        
        if (pList->first == pNode) {
            pList->first = pNode->next;
        }
        if (pList->last == pNode) {
            pList->last = pNode->prev;
        }
    } else {
        pList->first = NULL;
        pList->last = NULL;
    }
    
    pNode->prev = NULL;
    pNode->next = NULL;
}

// Splits 'pList' into two separate lists: a head list 'pHeadList' and a tail
// list 'pTailList'. The node 'pFirstNodeOfTail' will be the first node of the
// tail list and its predecessor (if any) will be the last node of the head list.
// if 'pFirstNodeOfTail' is null then all nodes are moved from 'pList' to the
// head list and 'pTailList' is initialized to an empty list. If 'pFirstNodeOfTail'
// has no predecessor then all nodes of 'pList' are moved to 'pTailList' and
// 'pHeadList' is initialized to an empty list.
// 'pList' is always initialized to an empty list on return.
void List_Split(List* _Nonnull pList, ListNode* _Nullable pFirstNodeOfTail, List* _Nonnull pHeadList, List* _Nonnull pTailList)
{
    ListNode* pOrigListFirst = pList->first;
    ListNode* pOrigListLast = pList->last;
    ListNode* pLastNodeOfHead = (pFirstNodeOfTail) ? pFirstNodeOfTail->prev : NULL;
    
    if (pLastNodeOfHead) {
        pHeadList->first = pOrigListFirst;
        pHeadList->last = pLastNodeOfHead;
        
        pLastNodeOfHead->next = NULL;
    } else {
        List_Init(pHeadList);
    }
    
    if (pFirstNodeOfTail) {
        pTailList->first = pFirstNodeOfTail;
        pTailList->last = pOrigListLast;
        
        pFirstNodeOfTail->prev = NULL;
    } else {
        List_Init(pTailList);
    }
    
    if (pList != pHeadList && pList != pTailList) {
        List_Init(pList);
    }
}
