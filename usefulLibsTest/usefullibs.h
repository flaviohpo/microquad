#ifndef __USEFULLIBS
#define __USEFULLIBS

// undef ULIBS_AUTO_TEST to undeclare void UsefulLibsAutoTest() function
#define ULIBS_AUTO_TEST

/*
typedef enum{
    BLACK = 0x00,
    GRAY = 0x49,
    MARROON = 0x40,
    OLIVE = 0x6c,
    GREEN = 0x08,
    TEAL = 0x09,
    NAVY = 0x01,
    PURPLE = 0x41,
    MAGENTA = 0xe3,
    BLUE = 0x03,
    AQUA = 0x1f,
    LIME = 0x1c,
    YELLOW = 0xfc,
    RED = 0xe0,
    SILVER = 0xb6,
    WHITE = 0xff,
    CYAN = 0x1F,
    ORANGE = (YELLOW | RED)
}COLOUR;
*/
typedef struct{
    char* Tittle;
    char Selectable;
    char BarVisible;
    char ValueVisible;
    int Value;
//    COLOUR BarColor;
//    COLOUR ValColor;
}Item;

Item* CreateItem(char* tittle, char sel, char barvis, char valvis, int val);

/* PILHA - LIFO */

struct StackNode{
    Item* Value;
    struct StackNode* Prev;
};

typedef struct StackNode* _StackNode;

typedef struct{
    char* Label;
    int Size;
    _StackNode Top;
}StackItems;

void StackItemsDispose(StackItems* StackItemsHandler);
char StackItemsPush(StackItems* StackItemsHandler, Item* Value);
char StackItemsPop(StackItems* StackItemsHandler);
_StackNode StackItemsGetTop(StackItems* StackItemsHandler);
StackItems* StackItemsCreate(Item* FirstItem, char* Label);

/* QUEUE - FIFO */
struct QueueNode{
    Item* Value;
    struct QueueNode* Next;
};

typedef struct QueueNode* _QueueNode;

typedef struct{
    _QueueNode First;
    _QueueNode Last;
    char *Label;
    int Size;
}QueueItems;

void QueueItemsDispose(QueueItems* QueueItemsHandler);
char QueueItemsPop(QueueItems* QueueItemsHandler);
char QueueItemsPush(QueueItems* QueueItemsHandler, Item* Value);
QueueItems* QueueItemsCreate(Item* First, char* Label);
_QueueNode QueueItemsGetLast(QueueItems* QueueItemsHandler);
_QueueNode QueueItemsGetFirst(QueueItems* QueueItemsHandler);

/* LISTA */
struct ListNode{
    Item* Value;
    struct ListNode* Next;
};

typedef struct ListNode* _ListNode;

typedef struct{
    _ListNode First;
    char *Label;
    int Size;
}ListItems;

void ListItemsDispose(ListItems* ListItemsHandler);
char ListItemsInsert(ListItems* ListItemsHandler, Item* Value, int index);
char ListItemsRemove(ListItems* ListItemsHandler, int index);
ListItems* ListItemsCreate(Item* First, char* Label);
_ListNode ListItemsGetNode(ListItems* ListItemsHandler, int index);

#ifdef ULIBS_AUTO_TEST
void UsefulLibsAutoTest();
void print_test();
#endif //ULIBS_AUTO_TEST

#endif // __USEFULLIBS