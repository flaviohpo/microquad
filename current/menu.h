#include "shorttypes.h"

typedef enum{
    ACTION_NONE = 0,
    ACTION_UP,
    ACTION_DOWN,
    ACTION_LEFT,
    ACTION_RIGHT,
    ACTION_ENTER,
    ACTION_RETURN
}ACTION;

typedef struct{
    // main properties
    ListItems *oListItems;
    uint16 SelectedItem;
    uchar JanelaSize;
    uint16 FirstShowed;
    char AutoRefresh            : 1;
    // attributes
    uchar BarsPositionH;
    uchar ValuePositionH;
    uchar CheckPositionH;
    uchar ArrowPositionH;
    uchar BarsLenght;
}MENU;

void draw_menu(MENU* menu, uchar clear); // redesenha todo o menu
MENU* menu_create(char *tittle, Item *FirstItem, uchar JanelaSize, char autorefresh, uchar barsposh, uchar valueposh, uchar arrowposh, uchar barslen);
uchar menu_add_item(MENU* menu, Item *it);
void menu_refresh(MENU* menu);
uint16 menu_process(ACTION act);
