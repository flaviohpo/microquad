#include "mytypes.h"
#include "usefullibs.h"
#include "color.h"
#include <stdlib.h>
#include <stdio.h>

#ifdef ULIBS_AUTO_TEST
#include "msp430f2618.h"
#include "lcd6100.h"
#include "color.h"
#include "delay.h"
#endif //ULIBS_AUTO_TEST

Item* create_item(const char* label, ITEMTYPE type, const int *minval, const int *maxval, const int *interval, int *val){
    Item* oItem = (Item*)malloc(sizeof(Item));
    if(oItem != NULL){
        oItem->Label        =   label;
        oItem->ItemType     =   type;
        oItem->MaxVal       =   maxval;
        oItem->MinVal       =   minval;
        oItem->Interval     =   interval;
        oItem->Value        =   val;
    }
    return oItem;
}

/* STACK */

StackItems* stack_items_create(Item* First, const char* Label){
    StackItems* oStack = (StackItems*)malloc(sizeof(StackItems));
    if(oStack != NULL){
        oStack->Label = Label;
        _StackNodePtr oNode = (_StackNodePtr)malloc(sizeof(struct StackNode));
        if(oNode != NULL){
            oNode->Prev = NULL;
            oNode->Value = First;
            oStack->Top = oNode;
            oStack->Size = 1;
        }
    }
    return oStack;
}

_StackNodePtr stack_items_get_top(StackItems* StackItemsHandler){
    return StackItemsHandler->Top;
}

uchar stack_items_push(StackItems* StackItemsHandler, Item* Value){
    uchar Result = 0;
    _StackNodePtr oNode = (_StackNodePtr)malloc(sizeof(struct StackNode));
    if(oNode != NULL){
        oNode->Value = Value;
        oNode->Prev = StackItemsHandler->Top;
        StackItemsHandler->Top = oNode;
        (StackItemsHandler->Size)++;
        Result = 1;
    }
    return Result;
}

uchar stack_items_pop(StackItems* StackItemsHandler){
    uchar result = 0;
    if(StackItemsHandler->Size > 0){
        _StackNodePtr oNodeTemp = StackItemsHandler->Top;
        StackItemsHandler->Top = StackItemsHandler->Top->Prev;
        (StackItemsHandler->Size)--;
        result = 1;
        free(oNodeTemp);
    }
    return result;
}

void stack_items_dispose(StackItems* StackItemsHandler){
    while(stack_items_pop(StackItemsHandler));
    free(StackItemsHandler);
}

/* QUEUE */

QueueItems* queue_items_create(Item* First, const char* Label){
    QueueItems* oQueue = (QueueItems*)malloc(sizeof(QueueItems));
    if(oQueue != NULL){
        oQueue->Label = Label;
        _QueueNodePtr oNode = (_QueueNodePtr)malloc(sizeof(struct QueueNode));
        if(oNode != NULL){
            oNode->Next = NULL;
            oNode->Value = First;
            oQueue->First = oNode;
            oQueue->Last = oNode;
            oQueue->Size = 1;
        }
    }
    return oQueue;
}

_QueueNodePtr queue_items_get_last(QueueItems* QueueItemsHandler){
    return QueueItemsHandler->Last;
}

_QueueNodePtr queue_items_get_first(QueueItems* QueueItemsHandler){
    return QueueItemsHandler->First;  
}

uchar queue_items_push(QueueItems* QueueItemsHandler, Item* Value){
    uchar Result = 0;
    _QueueNodePtr oNode = (_QueueNodePtr)malloc(sizeof(struct QueueNode));
    if(oNode != NULL){
        QueueItemsHandler->Last->Next = oNode;
        oNode->Next = NULL;
        oNode->Value = Value;
        QueueItemsHandler->Last = oNode;
        (QueueItemsHandler->Size)++;
        Result = 1;
    }

    return Result;
}

uchar queue_items_pop(QueueItems* QueueItemsHandler){
    uchar result = 0;
    if(QueueItemsHandler->Size > 0){
        _QueueNodePtr oNodeTemp = QueueItemsHandler->First;
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

void queue_items_dispose(QueueItems* QueueItemsHandler){
    while(queue_items_pop(QueueItemsHandler));
    free(QueueItemsHandler);
}

/* LIST */
ListItems* list_items_create(Item* First, const char* Label){
    ListItems* oList = (ListItems*)malloc(sizeof(ListItems));
    if(oList != NULL){
        oList->Label = Label;
        _ListNodePtr oNode = (_ListNodePtr)malloc(sizeof(struct ListNode));
        if(oNode != NULL){
            oNode->Next = NULL;
            oNode->Value = First;
            oList->First = oNode;
            oList->Size = 1;
        }
    }

    return oList;    
}

_ListNodePtr list_items_get_node(ListItems* ListItemsHandler, uint16 index){
    _ListNodePtr Iterator = ListItemsHandler->First;
    uint16 i = 0;
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

uchar list_items_insert(ListItems* ListItemsHandler, Item* Value, uint16 index){
    uchar result = 0;
    _ListNodePtr oNode = (_ListNodePtr)malloc(sizeof(struct ListNode));
    if(oNode != NULL){
        oNode->Value = Value;
        if(index == 0){
            oNode->Next = ListItemsHandler->First;
            ListItemsHandler->First=oNode;
        }
        else{
            _ListNodePtr Iterator = list_items_get_node(ListItemsHandler, index - 1);
            oNode->Next = Iterator->Next;
            Iterator->Next = oNode;
        }
        result = 1;
        (ListItemsHandler->Size)++;
    }

    return result;
}

uchar list_items_remove(ListItems* ListItemsHandler, uint16 index){
    uchar result = 0;
    if(ListItemsHandler->Size > 0){
        _ListNodePtr NodeRM = list_items_get_node(ListItemsHandler, index);
        if(ListItemsHandler->Size == 1){
            ListItemsHandler->First = NULL;
        }
        else{
            _ListNodePtr Iterator = list_items_get_node(ListItemsHandler, index - 1);
            Iterator->Next = NodeRM->Next;
        }
        (ListItemsHandler->Size)--;
        free(NodeRM);
        result = 1;
    }
    return result;
}

Item* list_items_get_item(ListItems* ListItemsHandler, uint16 index){
    return list_items_get_node(ListItemsHandler, index)->Value;
}

void list_items_dispose(ListItems* ListItemsHandler){
    while(list_items_remove(ListItemsHandler, ListItemsHandler->Size - 1));
    free(ListItemsHandler);
}

#ifdef ULIBS_AUTO_TEST

const uint16 stack_position = 0, queue_position = 7, list_position = 14;

StackItems *oStack;
QueueItems *oQueue;
ListItems *oList; 

/* Auto test routine */
void useful_libs_auto_test(){
    
    uint16 i = 0;
    
    lcd_clear(WHITE);
    lcd_setcolor(BLACK,WHITE);
    
    // cria item ptr
    Item *oItem = create_item("Item0",0,0,0,0,0);
    oStack = stack_items_create(oItem, "Stack"); 
    oItem = create_item("Item0", 0,0,0,0,0);
    oQueue = queue_items_create(oItem, "Queue");
    oItem = create_item("Item0", 0,0,0,0,0);
    oList = list_items_create(oItem, "List");
    
    if(oList == NULL){
        lcd_goto(0,0);
        printf("ERR:ln 284");
        __bis_SR_register(LPM4_bits);
    }
    else{
        oItem = create_item("item1",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item1",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item1",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item2",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item2",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item2",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item3",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item3",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item3",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item4",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item4",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item4",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item5",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item5",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item5",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item6",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item6",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item6",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item7",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item7",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item7",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item8",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item8",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item8",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
        
        oItem = create_item("item9",0,0,0,0,0);
        stack_items_push(oStack, oItem);
        oItem = create_item("item9",0,0,0,0,0);
        queue_items_push(oQueue, oItem);
        oItem = create_item("item9",0,0,0,0,0);
        list_items_insert(oList, oItem, oList->Size);
       
        print_test();
        delayms(3000);
        
        uint16 lenght = oStack->Size;
        
        for(i = lenght - 1; i >= 0; i--){
            stack_items_pop(oStack);
            queue_items_pop(oQueue);
            list_items_remove(oList, i);
            print_test();
            delayms(1000);
        }
    }
}

void print_test(){
    uint16 i = 0;
    lcd_clear(WHITE);
    lcd_goto(stack_position, 0);
    printf(oStack->Label);
    printf("%d",oStack->Size);
    lcd_goto(stack_position, 1);
    printf((stack_items_get_top(oStack)->Value)->Label);
    
    lcd_goto(queue_position, 0);
    printf(oQueue->Label);
    printf("%d",oQueue->Size);
    lcd_goto(queue_position, 1);
    printf((queue_items_get_last(oQueue)->Value)->Label);
    lcd_goto(queue_position, 2);
    printf((queue_items_get_first(oQueue)->Value)->Label);
    
    lcd_goto(list_position, 0);
    printf(oList->Label);
    printf("%d",oList->Size);
    for(i = 0; i<oList->Size; i++){
        lcd_goto(list_position, 1+i);
        printf((list_items_get_node(oList, i)->Value)->Label);
    }
}

#endif // ULIBS_AUTO_TEST
