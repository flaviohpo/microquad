#include "usefullibs.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef ULIBS_AUTO_TEST
#include "msp430f2618.h"
#include "lcd6100.h"
#include "delay.h"
#endif //ULIBS_AUTO_TEST

Item* CreateItem(char* tittle, char sel, char barvis, char valvis, int val){
    Item* oItem = (Item*)malloc(sizeof(Item));
    if(oItem != NULL){
        oItem->Tittle=tittle;
        oItem->Selectable=sel;
        oItem->BarVisible=barvis;
        oItem->ValueVisible=valvis;
        oItem->Value=val;
    }
    else{
        printf("ERR:ln 21");
        __bis_SR_register(LPM4_bits);
    }
    return oItem;
}

/* STACK */

StackItems* StackItemsCreate(Item* First, char* Label){
    StackItems* oStack = (StackItems*)malloc(sizeof(StackItems));
    if(oStack != NULL){
        oStack->Label = Label;
        _StackNode oNode = (_StackNode)malloc(sizeof(struct StackNode));
        if(oNode != NULL){
            oNode->Prev = NULL;
            oNode->Value = First;
            oStack->Top = oNode;
            oStack->Size = 1;
        }
    }
    else{
        printf("ERR:ln 42");
        __bis_SR_register(LPM4_bits);
    }
    return oStack;
}

_StackNode StackItemsGetTop(StackItems* StackItemsHandler){
    return StackItemsHandler->Top;
}

char StackItemsPush(StackItems* StackItemsHandler, Item* Value){
    char Result = 0;
    _StackNode oNode = (_StackNode)malloc(sizeof(struct StackNode));
    if(oNode != NULL){
        oNode->Value = Value;
        oNode->Prev = StackItemsHandler->Top;
        StackItemsHandler->Top = oNode;
        (StackItemsHandler->Size)++;
        Result = 1;
    }
    else{
        printf("ERR:ln 63");
        __bis_SR_register(LPM4_bits);
    }
    return Result;
}

char StackItemsPop(StackItems* StackItemsHandler){
    char result = 0;
    if(StackItemsHandler->Size > 0){
        _StackNode oNodeTemp = StackItemsHandler->Top;
        StackItemsHandler->Top = StackItemsHandler->Top->Prev;
        (StackItemsHandler->Size)--;
        result = 1;
        free(oNodeTemp);
    }
    return result;
}

void StackItemsDispose(StackItems* StackItemsHandler){
    while(StackItemsPop(StackItemsHandler));
    free(StackItemsHandler);
}

/* QUEUE */

QueueItems* QueueItemsCreate(Item* First, char* Label){
    QueueItems* oQueue = (QueueItems*)malloc(sizeof(QueueItems));
    if(oQueue != NULL){
        oQueue->Label = Label;
        _QueueNode oNode = (_QueueNode)malloc(sizeof(struct QueueNode));
        if(oNode != NULL){
            oNode->Next = NULL;
            oNode->Value = First;
            oQueue->First = oNode;
            oQueue->Last = oNode;
            oQueue->Size = 1;
        }
    }
    else{
        printf("ERR:ln 101");
        __bis_SR_register(LPM4_bits);
    }
    return oQueue;
}

_QueueNode QueueItemsGetLast(QueueItems* QueueItemsHandler){
    return QueueItemsHandler->Last;
}

_QueueNode QueueItemsGetFirst(QueueItems* QueueItemsHandler){
    return QueueItemsHandler->First;  
}

char QueueItemsPush(QueueItems* QueueItemsHandler, Item* Value){
    char Result = 0;
    _QueueNode oNode = (_QueueNode)malloc(sizeof(struct QueueNode));
    if(oNode != NULL){
        QueueItemsHandler->Last->Next = oNode;
        oNode->Next = NULL;
        oNode->Value = Value;
        QueueItemsHandler->Last = oNode;
        (QueueItemsHandler->Size)++;
        Result = 1;
    }
    else{
        printf("ERR:ln 127");
        __bis_SR_register(LPM4_bits);
    }
    return Result;
}

char QueueItemsPop(QueueItems* QueueItemsHandler){
    char result = 0;
    if(QueueItemsHandler->Size > 0){
        _QueueNode oNodeTemp = QueueItemsHandler->First;
        if(QueueItemsHandler->Size == 1){
            QueueItemsHandler->Last=NULL;
            free(oNodeTemp);
            QueueItemsHandler->First=NULL;                      
        }
        else{
            QueueItemsHandler->First = QueueItemsHandler->First->Next;
            free(oNodeTemp);
        }
        (QueueItemsHandler->Size)--;
        result = 1;
    }
    return result;
}

void QueueItemsDispose(QueueItems* QueueItemsHandler){
    while(QueueItemsPop(QueueItemsHandler));
    free(QueueItemsHandler);
}

/* LIST */
ListItems* ListItemsCreate(Item* First, char* Label){
    ListItems* oList = (ListItems*)malloc(sizeof(ListItems));
    if(oList != NULL){
        oList->Label = Label;
        _ListNode oNode = (_ListNode)malloc(sizeof(struct ListNode));
        if(oNode != NULL){
            oNode->Next = NULL;
            oNode->Value = First;
            oList->First = oNode;
            oList->Size = 1;
        }
    }
    else{
        printf("ERR:ln 164");
        __bis_SR_register(LPM4_bits);
    }
    return oList;    
}

_ListNode ListItemsGetNode(ListItems* ListItemsHandler, int index){
    _ListNode Iterator = ListItemsHandler->First;
    int i = 0;
    if(index < ListItemsHandler->Size){
        while(i++ < index){
            Iterator = Iterator->Next;
        }
        return Iterator;
    }
    else{
        return NULL;
    }
}

char ListItemsInsert(ListItems* ListItemsHandler, Item* Value, int index){
    char result = 0;
    _ListNode oNode = (_ListNode)malloc(sizeof(struct ListNode));
    if(oNode != NULL){
        oNode->Value = Value;
        if(index == 0){
            oNode->Next = ListItemsHandler->First;
            ListItemsHandler->First=oNode;
        }
        else{
            _ListNode Iterator = ListItemsGetNode(ListItemsHandler, index - 1);
            oNode->Next = Iterator->Next;
            Iterator->Next = oNode;
        }
        result = 1;
        (ListItemsHandler->Size)++;
    }
    else{
        printf("ERR:ln 207");
        __bis_SR_register(LPM4_bits);
    }
    return result;
}

char ListItemsRemove(ListItems* ListItemsHandler, int index){
    char result = 0;
    if(ListItemsHandler->Size > 0){
        _ListNode NodeRM = ListItemsGetNode(ListItemsHandler, index);
        if(ListItemsHandler->Size == 1){
            ListItemsHandler->First = NULL;
        }
        else{
            _ListNode Iterator = ListItemsGetNode(ListItemsHandler, index - 1);
            Iterator->Next = NodeRM->Next;
        }
        (ListItemsHandler->Size)--;
        free(NodeRM);
        result = 1;
    }
    return result;
}

void ListItemsDispose(ListItems* ListItemsHandler){
    while(ListItemsRemove(ListItemsHandler, ListItemsHandler->Size - 1));
    free(ListItemsHandler);
}

#ifdef ULIBS_AUTO_TEST

const int stack_position = 0, queue_position = 7, list_position = 14;

StackItems *oStack;
QueueItems *oQueue;
ListItems *oList; 

/* Auto test routine */
void UsefulLibsAutoTest(){
    
    int i = 0;
    
    lcd_clear(WHITE);
    lcd_set_colour(WHITE, BLACK);
    
    char *c = NULL;
    
    printf(c);

    // cria item ptr
    Item *oItem = CreateItem("Item0", 0,0,0,0);
    oStack = StackItemsCreate(oItem, "Stack"); 
    oItem = CreateItem("Item0", 0,0,0,0);
    oQueue = QueueItemsCreate(oItem, "Queue");
    oItem = CreateItem("Item0", 0,0,0,0);
    oList = ListItemsCreate(oItem, "List");
    
    if(oList == NULL){
        lcd_goto(0,0);
        printf("ERR:ln 284");
        __bis_SR_register(LPM4_bits);
    }
    else{
        oItem = CreateItem("item1",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item1",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item1",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item2",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item2",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item2",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item3",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item3",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item3",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item4",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item4",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item4",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item5",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item5",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item5",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item6",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item6",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item6",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item7",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item7",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item7",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item8",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item8",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item8",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
        
        oItem = CreateItem("item9",0,0,0,0);
        StackItemsPush(oStack, oItem);
        oItem = CreateItem("item9",0,0,0,0);
        QueueItemsPush(oQueue, oItem);
        oItem = CreateItem("item9",0,0,0,0);
        ListItemsInsert(oList, oItem, oList->Size);
       
        print_test();
        delayms(3000);
        
        int lenght = oStack->Size;
        
        for(i = lenght - 1; i >= 0; i--){
            StackItemsPop(oStack);
            QueueItemsPop(oQueue);
            ListItemsRemove(oList, i);
            print_test();
            delayms(1000);
        }
    }
}

void print_test(){
    int i = 0;
    lcd_clear(WHITE);
    lcd_goto(stack_position, 0);
    printf(oStack->Label);
    printf("%d",oStack->Size);
    lcd_goto(stack_position, 1);
    printf((StackItemsGetTop(oStack)->Value)->Tittle);
    
    lcd_goto(queue_position, 0);
    printf(oQueue->Label);
    printf("%d",oQueue->Size);
    lcd_goto(queue_position, 1);
    printf((QueueItemsGetLast(oQueue)->Value)->Tittle);
    lcd_goto(queue_position, 2);
    printf((QueueItemsGetFirst(oQueue)->Value)->Tittle);
    
    lcd_goto(list_position, 0);
    printf(oList->Label);
    printf("%d",oList->Size);
    for(i = 0; i<oList->Size; i++){
        lcd_goto(list_position, 1+i);
        printf((ListItemsGetNode(oList, i)->Value)->Tittle);
    }
}

#endif // ULIBS_AUTO_TEST
