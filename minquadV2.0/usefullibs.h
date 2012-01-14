#ifndef __USEFULLIBS
#define __USEFULLIBS

// undef ULIBS_AUTO_TEST to undeclare void UsefulLibsAutoTest() function
#define ULIBS_AUTO_TEST

#define MASK_SUBMENU            0x01
#define MASK_WRITE              0x02
#define MASK_READ               0x04
#define MASK_BOOL               0x08
#define MASK_VALUE              0x10
#define MASK_BAR                0x20

typedef enum{
    ITEMTYPE_SUBMENU        =   MASK_SUBMENU | MASK_READ | MASK_WRITE,
    ITEMTYPE_VALUE_RW       =   MASK_READ | MASK_WRITE | MASK_VALUE,
    ITEMTYPE_VALUE_R        =   MASK_READ | MASK_VALUE,
    ITEMTYPE_BAR_R          =   MASK_READ | MASK_BAR,
    ITEMTYPE_BAR_RW         =   MASK_READ | MASK_WRITE | MASK_BAR,
    ITEMTYPE_VALUE_BAR_RW   =   MASK_READ | MASK_WRITE | MASK_VALUE | MASK_BAR,
    ITEMTYPE_VALUE_BAR_R    =   MASK_READ | MASK_VALUE | MASK_BAR,
    ITEMTYPE_BOOLEAN_RW     =   MASK_READ | MASK_WRITE | MASK_BOOL,
    ITEMTYPE_BOOLEAN_R      =   MASK_READ | MASK_BOOL,
}ITEMTYPE;

typedef struct{
    const char *Label;    
    int *Value; // o valor eh uma referencia de algum lugar da memoria    
    const int *MaxVal;
    const int *MinVal;
    const int *Interval;
    ITEMTYPE ItemType;
}Item;

// podia fazer um __atributte__ pra nao permitir valor != de ponteiro e ITEMTYPE deve ser valido
Item* create_item(const char* label, ITEMTYPE type, const int *minval, const int *maxval, const int *interval, int *val);

/* PILHA - LIFO */

struct StackNode{
    Item* Value;
    struct StackNode* Prev;
};

typedef struct StackNode* _StackNodePtr;

typedef struct{
    const char* Label;
    uint16 Size;
    _StackNodePtr Top;
}StackItems;

void stack_items_dispose(StackItems* StackItemsHandler);
uchar stack_items_push(StackItems* StackItemsHandler, Item* Value);
uchar stack_items_pop(StackItems* StackItemsHandler);
_StackNodePtr stack_itemsGet_top(StackItems* StackItemsHandler);
StackItems* stack_items_create(Item* FirstItem, const char* Label);

/* QUEUE - FIFO */
struct QueueNode{
    Item* Value;
    struct QueueNode* Next;
};

typedef struct QueueNode* _QueueNodePtr;

typedef struct{
    _QueueNodePtr First;
    _QueueNodePtr Last;
    const char *Label;
    uint16 Size;
}QueueItems;

void queue_items_dispose(QueueItems* QueueItemsHandler);
uchar queue_items_pop(QueueItems* QueueItemsHandler);
uchar queue_items_push(QueueItems* QueueItemsHandler, Item* Value);
QueueItems* queue_items_create(Item* First, const char* Label);
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
    const char *Label;
    uint16 Size;
}ListItems;

void list_items_dispose(ListItems* ListItemsHandler);
uchar list_items_insert(ListItems* ListItemsHandler, Item* Value, uint16 index);
uchar list_items_remove(ListItems* ListItemsHandler, uint16 index);
ListItems* list_items_create(Item* First, const char* Label);
_ListNodePtr list_items_get_node(ListItems* ListItemsHandler, uint16 index);
Item* list_items_get_item(ListItems* ListItemsHandler, uint16 index);

#ifdef ULIBS_AUTO_TEST
void useful_libs_auto_test();
void print_test();
#endif //ULIBS_AUTO_TEST

#endif // __USEFULLIBS