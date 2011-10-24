#ifndef __USEFULLIBS
#define __USEFULLIBS

#include "shorttypes.h"

// undef ULIBS_AUTO_TEST to undeclare void UsefulLibsAutoTest() function
#define ULIBS_AUTO_TEST

typedef struct{
    char *Label;
    char BarVisible     : 1;
    char ValueVisible   : 1;
    char CheckVisible   : 1;
    char Checked        : 1;
    int MaxVal;
    int MinVal;
    int Interval;
    int *Value; // o valor eh uma referencia de algum lugar da memoria
}Item;

Item* create_item(char* label, char barvis, char valvis, char checkvis, char checked, int maxval, int minval, int interval, int *val);

/* PILHA - LIFO */

struct StackNode{
    Item* Value;
    struct StackNode* Prev;
};

typedef struct StackNode* _StackNodePtr;

typedef struct{
    char* Label;
    uint16 Size;
    _StackNodePtr Top;
}StackItems;

void stack_items_dispose(StackItems* StackItemsHandler);
uchar stack_items_push(StackItems* StackItemsHandler, Item* Value);
uchar stack_items_pop(StackItems* StackItemsHandler);
_StackNodePtr stack_itemsGet_top(StackItems* StackItemsHandler);
StackItems* stack_items_create(Item* FirstItem, char* Label);

/* QUEUE - FIFO */
struct QueueNode{
    Item* Value;
    struct QueueNode* Next;
};

typedef struct QueueNode* _QueueNodePtr;

typedef struct{
    _QueueNodePtr First;
    _QueueNodePtr Last;
    char *Label;
    uint16 Size;
}QueueItems;

void queue_items_dispose(QueueItems* QueueItemsHandler);
uchar queue_items_pop(QueueItems* QueueItemsHandler);
uchar queue_items_push(QueueItems* QueueItemsHandler, Item* Value);
QueueItems* queue_items_create(Item* First, char* Label);
_QueueNodePtr queue_items_getLast(QueueItems* QueueItemsHandler);
_QueueNodePtr queue_items_getFirst(QueueItems* QueueItemsHandler);

/* LISTA */
struct ListNode{
    Item* Value;
    struct ListNode* Next;
};

typedef struct ListNode* _ListNodePtr;

typedef struct{
    _ListNodePtr First;
    char *Label;
    uint16 Size;
}ListItems;

void list_items_dispose(ListItems* ListItemsHandler);
uchar list_items_insert(ListItems* ListItemsHandler, Item* Value, uint16 index);
uchar list_items_remove(ListItems* ListItemsHandler, uint16 index);
ListItems* list_items_create(Item* First, char* Label);
_ListNodePtr list_items_get_node(ListItems* ListItemsHandler, uint16 index);

#ifdef ULIBS_AUTO_TEST
void useful_libs_auto_test();
void print_test();
#endif //ULIBS_AUTO_TEST

#endif // __USEFULLIBS